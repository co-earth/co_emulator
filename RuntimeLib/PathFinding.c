#include "World.h"
#include "Movement.h"

typedef struct _RTPathFindingOpenNode* RTPathFindingOpenNodeRef;
typedef struct _RTPathFindingNode* RTPathFindingNodeRef;

struct _RTPathFindingOpenNode {
	RTPathFindingOpenNodeRef Next;
	Bool IsOpen;
};

struct _RTPathFindingNode {
	Int32 X;
	Int32 Y;
	Int32 TotalCost;
	RTPathFindingNodeRef Parent;
	RTPathFindingNodeRef Next;
};

struct _RTPathFindingContext {
	Int32 TargetX;
	Int32 TargetY;
	Int32 PathLength;
	Int32 PathLengthPrevious;
	Int32 PathX[RUNTIME_MOVEMENT_MAX_PATH_FIND_NODE_COUNT];
	Int32 PathY[RUNTIME_MOVEMENT_MAX_PATH_FIND_NODE_COUNT];
	RTPathFindingNodeRef OpenList;
	RTPathFindingOpenNodeRef OpenNodeHead;
	RTPathFindingOpenNodeRef OpenNodeTail;
	struct _RTPathFindingNode Nodes[RUNTIME_MOVEMENT_MAX_PATH_FIND_NODE_COUNT];
	struct _RTPathFindingOpenNode OpenNodes[RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE];
};

const Int32 PathFindingOffsetX[8] = { 1, -1,  0,  0, -1,  1,  1, -1 };
const Int32 PathFindingOffsetY[8] = { 0,  0,  1, -1,  1, -1,  1, -1 };
static struct _RTPathFindingContext PathFindingContext = { 0 };

RTPathFindingNodeRef RTPathFindingAStar(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	Int32 StartX,
	Int32 StartY,
	Int32 EndX,
	Int32 EndY,
	UInt32 CollisionMask
) {
	struct _RTPathFindingContext* Context = &PathFindingContext;

	RTPathFindingOpenNodeRef OpenNode;
	while (Context->OpenNodeHead) {
		OpenNode = Context->OpenNodeHead;
		Context->OpenNodeHead = Context->OpenNodeHead->Next;
		OpenNode->IsOpen = false;
		OpenNode->Next = NULL;
	}

	Context->TargetX = EndX;
	Context->TargetY = EndY;
	Context->Nodes[0].X = StartX;
	Context->Nodes[0].Y = StartY;
	Context->OpenNodeHead = Context->OpenNodeTail = Context->OpenNodes + StartX + StartY * RUNTIME_WORLD_SIZE;
	Context->OpenNodeHead->IsOpen = true;
	Context->Nodes[0].TotalCost = MAX(ABS(EndX - StartX), ABS(EndY - StartY));
	Context->Nodes[0].Parent = NULL;
	Context->Nodes[0].Next = NULL;
	Context->OpenList = Context->Nodes;

	Int32 NodeIndex = 1;
	while (Context->OpenList) {
		RTPathFindingNodeRef Node = Context->OpenList;
		Context->OpenList = Context->OpenList->Next;

		if (Node->X == Context->TargetX && Node->Y == Context->TargetY) {
			return Node;
		}

		for (Int32 Index = 0; Index < 8; Index += 1) {
			Int32 X = Node->X + PathFindingOffsetX[Index];
			Int32 Y = Node->Y + PathFindingOffsetY[Index];

			if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) {
				continue;
			}

			if (Context->OpenNodes[X + Y * RUNTIME_WORLD_SIZE].IsOpen) {
				continue;
			}

			if (RTWorldIsTileColliding(Runtime, World, X, Y, CollisionMask)) {
				OpenNode = Context->OpenNodes + X + Y * RUNTIME_WORLD_SIZE;
				OpenNode->IsOpen = true;
				Context->OpenNodeTail->Next = OpenNode;
				Context->OpenNodeTail = OpenNode;
				continue;
			}

			RTPathFindingNodeRef NewNode = Context->Nodes + NodeIndex;
			NodeIndex += 1;

			if (NodeIndex >= RUNTIME_MOVEMENT_MAX_PATH_FIND_NODE_COUNT) {
				return NULL;
			}

			NewNode->Parent = Node;
			NewNode->Next = NULL;
			NewNode->TotalCost = MAX(ABS(EndX - X), ABS(EndY - Y));
			NewNode->X = X;
			NewNode->Y = Y;

			OpenNode = Context->OpenNodes + X + Y * RUNTIME_WORLD_SIZE;
			OpenNode->IsOpen = true;
			Context->OpenNodeTail->Next = OpenNode;
			Context->OpenNodeTail = OpenNode;

			if ((!Context->OpenList) || (Context->OpenList->TotalCost > NewNode->TotalCost)) {
				NewNode->Next = Context->OpenList;
				Context->OpenList = NewNode;
			}
			else {
				RTPathFindingNodeRef TempNode = Context->OpenList;
				while (TempNode->Next) {
					if (TempNode->Next->TotalCost >= NewNode->TotalCost) {
						NewNode->Next = TempNode->Next;
						TempNode->Next = NewNode;
						break;
					}

					TempNode = TempNode->Next;
				}

				if (!TempNode->Next) {
					TempNode->Next = NewNode;
				}
			}
		}
	}

	return NULL;
}

Int32 RTPathFindLastVisibleNodeIndex(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	Int32 PathIndex,
	UInt32 CollisionMask,
	UInt32 IgnoreMask
) {
	struct _RTPathFindingContext* Context = &PathFindingContext;

	Int32 PathIndexA = PathIndex + 1;
	Int32 PathIndexB = Context->PathLengthPrevious - 1;
	Bool HasCollision = !RTWorldTraceMovement(
		Runtime,
		World,
		Context->PathX[PathIndex],
		Context->PathY[PathIndex],
		Context->PathX[PathIndexB],
		Context->PathY[PathIndexB],
		NULL,
		NULL,
		CollisionMask,
		IgnoreMask
	);

	if (!HasCollision) {
		return PathIndexB;
	}

	while (PathIndexB - PathIndexA > 1) {
		Int32 PathIndexC = (PathIndexA + PathIndexB + 1) >> 1;

		HasCollision = !RTWorldTraceMovement(
			Runtime,
			World,
			Context->PathX[PathIndex],
			Context->PathY[PathIndex],
			Context->PathX[PathIndexC],
			Context->PathY[PathIndexC],
			NULL,
			NULL,
			CollisionMask,
			IgnoreMask
		);

		if (HasCollision) {
			PathIndexB = PathIndexC;
		}
		else {
			PathIndexA = PathIndexC;
		}
	}

	return PathIndexA;
}

Bool RTMovementFindPath(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMovementRef Movement
) {
	struct _RTPathFindingContext* Context = &PathFindingContext;

	if (Movement->PositionBegin.X == Movement->PositionEnd.X &&
		Movement->PositionBegin.Y == Movement->PositionEnd.Y) {
		return false;
	}
	
	Int32 WaypointIndex = 0;
	RTPositionRef Waypoint = &Movement->Waypoints[WaypointIndex];
	RTPathFindingNodeRef Node = RTPathFindingAStar(
		Runtime,
		World,
		Movement->PositionBegin.X,
		Movement->PositionBegin.Y,
		Movement->PositionEnd.X,
		Movement->PositionEnd.Y,
		Movement->CollisionMask,
		Movement->IgnoreMask
	);

	if (!Node) {
		Int32 EndX = 0;
		Int32 EndY = 0;
		RTWorldTraceMovement(
			Runtime,
			World,
			Context->Nodes[0].X,
			Context->Nodes[0].Y,
			Context->TargetX,
			Context->TargetY,
			&EndX,
			&EndY,
			Movement->CollisionMask,
			Movement->IgnoreMask
		);

		if (EndX == Context->Nodes[0].X && EndY == Context->Nodes[0].Y) {
			Movement->WaypointCount = 0;
		}
		else {
			Waypoint->X = Movement->PositionBegin.X;
			Waypoint->Y = Movement->PositionBegin.Y;
			WaypointIndex += 1;
			Waypoint = &Movement->Waypoints[WaypointIndex];

			Waypoint->X = EndX;
			Waypoint->Y = EndY;
			Movement->WaypointCount = 2;
		}

		return false;
	}

	RTPathFindingNodeRef TempNode = Node;
	Int32 PathIndex = 0;
	while (TempNode) {
		TempNode = TempNode->Parent;
		PathIndex += 1;
	}

	Context->PathLengthPrevious = 0;
	while (Node) {
		Context->PathX[PathIndex - 1 - Context->PathLengthPrevious] = Node->X;
		Context->PathY[PathIndex - 1 - Context->PathLengthPrevious] = Node->Y;
		Context->PathLengthPrevious += 1;

		Node = Node->Parent;
	}

	Waypoint->X = Context->Nodes[0].X;
	Waypoint->Y = Context->Nodes[0].Y;
	WaypointIndex += 1;
	Waypoint = &Movement->Waypoints[WaypointIndex];

	Context->PathLength = 1;
	PathIndex = 0;
	while (PathIndex != Context->PathLengthPrevious - 1) {
		PathIndex = RTPathFindLastVisibleNodeIndex(
			Runtime,
			World,
			PathIndex,
			Movement->CollisionMask,
			Movement->IgnoreMask
		);
		Waypoint->X = Context->PathX[PathIndex];
		Waypoint->Y = Context->PathY[PathIndex];
		WaypointIndex += 1;
		Waypoint = &Movement->Waypoints[WaypointIndex];

		Context->PathLength += 1;
		if (Context->PathLength >= RUNTIME_MOVEMENT_MAX_WAYPOINT_COUNT) {
			Movement->WaypointCount = 2;
			return false;
		}
	}

	Movement->WaypointCount = Context->PathLength;
	return true;
}

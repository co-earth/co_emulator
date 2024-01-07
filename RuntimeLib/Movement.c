#include "Movement.h"
#include "Runtime.h"

Void RTMovementInitialize(
	RTRuntimeRef Runtime,
	RTMovementRef Movement,
	Int32 X,
	Int32 Y,
	Int32 Speed,
	UInt32 CollisionMask
) {
	memset(Movement, 0, sizeof(struct _RTMovement));

	Movement->PositionBegin.X = X;
	Movement->PositionBegin.Y = Y;
	Movement->PositionCurrent.X = X;
	Movement->PositionCurrent.Y = Y;
	Movement->PositionEnd.X = X;
	Movement->PositionEnd.Y = Y;
	Movement->Speed = (Float32)Speed / RUNTIME_MOVEMENT_SPEED_SCALE;
	Movement->CollisionMask = CollisionMask;
	Movement->IgnoreMask = 0;
	Movement->TickCount = GetTickCount();
}

Void RTMovementStartDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
) {
	RTPositionRef WaypointA = &Movement->Waypoints[0];
	RTPositionRef WaypointB = &Movement->Waypoints[1];
	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointA->Y;
	Int32 AbsDeltaX = (DeltaX >= 0) ? DeltaX : -DeltaX;
	Int32 AbsDeltaY = (DeltaY >= 0) ? DeltaY : -DeltaY;

	if (AbsDeltaX >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
		AbsDeltaX = RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1;
	}

	if (AbsDeltaY >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
		AbsDeltaY = RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1;
	}

	Movement->WaypointIndex = 0;
	Movement->Distance = Runtime->MovementDistanceCache[AbsDeltaX + AbsDeltaY * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH];
	Movement->Base = 0;
	Movement->Sin = (Float32)DeltaY / Movement->Distance;
	Movement->Cos = (Float32)DeltaX / Movement->Distance;
	Movement->IsMoving = true;
	Movement->IsDeadReckoning = true;
	Movement->TickCount = GetTickCount();
}

Void RTMovementRestartDeadReckoning(
	RTRuntimeRef Runtime,
	RTMovementRef Movement
) {
	assert(Movement->IsMoving);

	RTPositionRef WaypointA = &Movement->Waypoints[0];
	RTPositionRef WaypointB = &Movement->Waypoints[1];
	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointA->Y;
	Int32 AbsDeltaX = (DeltaX >= 0) ? DeltaX : -DeltaX;
	Int32 AbsDeltaY = (DeltaY >= 0) ? DeltaY : -DeltaY;

	if (AbsDeltaX >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
		AbsDeltaX = RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1;
	}

	if (AbsDeltaY >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
		AbsDeltaY = RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1;
	}

	Movement->WaypointIndex = 0;
	Movement->Distance = Runtime->MovementDistanceCache[AbsDeltaX + AbsDeltaY * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH];
	Movement->Base += Movement->Distance;
	Movement->Sin = (Float32)DeltaY / Movement->Distance;
	Movement->Cos = (Float32)DeltaX / Movement->Distance;
	Movement->IsDeadReckoning = true;
}

Void RTMovementEndDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
) {
	Movement->PositionBegin.X = Movement->PositionEnd.X;
	Movement->PositionBegin.Y = Movement->PositionEnd.Y;
	Movement->IsMoving = false;
	Movement->IsDeadReckoning = false;
}

Void RTMovementUpdateDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
) {
	if (!Movement->IsDeadReckoning) return;

	UInt32 ElapsedTime = GetTickCount() - Movement->TickCount;
	Float32 Distance = Movement->Speed * (Float32)ElapsedTime / 1000;
	Float32 Overflow = Distance - Movement->Base;
	if (Overflow < 0) return;

	RTPositionRef WaypointA = &Movement->Waypoints[Movement->WaypointIndex];
	RTPositionRef WaypointB = &Movement->Waypoints[Movement->WaypointIndex + 1];

	while (Overflow >= Movement->Distance) {
		Movement->Base += Movement->Distance;
		WaypointA = WaypointB;

		if (Movement->WaypointIndex + 2 >= Movement->WaypointCount) {
			Movement->IsDeadReckoning = false;
			Movement->PositionCurrent.X = WaypointA->X;
			Movement->PositionCurrent.Y = WaypointA->Y;
			return;
		}

		Movement->WaypointIndex += 1;
		WaypointB = &Movement->Waypoints[Movement->WaypointIndex + 1];

		Int32 DeltaX = WaypointB->X - WaypointA->X;
		Int32 DeltaY = WaypointB->Y - WaypointA->Y;
		Int32 AbsDeltaX = (DeltaX >= 0) ? DeltaX : -DeltaX;
		Int32 AbsDeltaY = (DeltaY >= 0) ? DeltaY : -DeltaY;

		if (AbsDeltaX >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
			AbsDeltaX = RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1;
		}

		if (AbsDeltaY >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
			AbsDeltaY = RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1;

		}

		Overflow -= Movement->Distance;
		Movement->Distance = Runtime->MovementDistanceCache[AbsDeltaX + AbsDeltaY * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH];
		Movement->Sin = (Float32)DeltaY / Movement->Distance;
		Movement->Cos = (Float32)DeltaX / Movement->Distance;
	}

	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointA->Y;
	Int32 AbsDeltaX = (DeltaX >= 0) ? DeltaX : -DeltaX;
	Int32 AbsDeltaY = (DeltaY >= 0) ? DeltaY : -DeltaY;

	if (AbsDeltaX > AbsDeltaY) {
		Int32 PositionX = (Int32)(Overflow * Movement->Cos);
		Int32 OffsetX = (PositionX < 0) ? -PositionX : PositionX;
		Int32 OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

		if (DeltaX < 0) {
			OffsetX = -OffsetX;
		}

		if (DeltaY < 0) {
			OffsetY = -OffsetY;
		}

		Movement->PositionCurrent.X = WaypointA->X + OffsetX;
		Movement->PositionCurrent.Y = WaypointA->Y + OffsetY;
	}
	else {
		Int32 PositionY = (Int32)(Overflow * Movement->Sin);
		Int32 OffsetY = (PositionY < 0) ? -PositionY : PositionY;
		Int32 OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

		if (DeltaX < 0) {
			OffsetX = -OffsetX;
		}

		if (DeltaY < 0) {
			OffsetY = -OffsetY;
		}

		Movement->PositionCurrent.X = WaypointA->X + OffsetX;
		Movement->PositionCurrent.Y = WaypointA->Y + OffsetY;
	}
}

Int32 RTMovementDistance(
	RTMovementRef Lhs,
	RTMovementRef Rhs
) {
	return RTCalculateDistance(
		Lhs->PositionCurrent.X, 
		Lhs->PositionCurrent.Y, 
		Rhs->PositionCurrent.X, 
		Rhs->PositionCurrent.Y
	);
}

Bool RTMovementIsInRange(
	RTRuntimeRef Runtime,
	RTMovementRef Movement,
	Int32 X,
	Int32 Y
) {
	if (Movement->IsDeadReckoning) {
		RTMovementUpdateDeadReckoning(Runtime, Movement);
	}

	Int32 DeltaX = X - Movement->PositionCurrent.X;
	Int32 DeltaY = Y - Movement->PositionCurrent.Y;

	return (DeltaX * DeltaX + DeltaY * DeltaY) < (RUNTIME_MOVEMENT_MAX_DISTANCE_IN_RANGE * RUNTIME_MOVEMENT_MAX_DISTANCE_IN_RANGE);
}

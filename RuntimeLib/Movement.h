#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

struct _RTMovement {
    UInt32 TickCount;
    UInt32 Flags;
    UInt32 CollisionMask;
    UInt32 IgnoreMask;
    Bool IsMoving;
    Bool IsDeadReckoning;
    Int32 WaypointIndex;
    Int32 WaypointCount;
    RTPosition Waypoints[RUNTIME_MOVEMENT_MAX_WAYPOINT_COUNT];
    RTPosition PositionBegin;
    RTPosition PositionCurrent;
    RTPosition PositionEnd;
    Float32 Base;
    Float32 Distance;
    Float32 Speed;
    Float32 Sin;
    Float32 Cos;
};

Void RTMovementInitialize(
    RTRuntimeRef Runtime,
    RTMovementRef Movement,
    Int32 X,
    Int32 Y,
    Int32 Speed,
    UInt32 CollisionMask
);

Void RTMovementStartDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Void RTMovementRestartDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Void RTMovementEndDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Void RTMovementUpdateDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Int32 RTMovementDistance(
    RTMovementRef Lhs,
    RTMovementRef Rhs
);

Bool RTMovementIsInRange(
    RTRuntimeRef Runtime,
    RTMovementRef Movement,
    Int32 X,
    Int32 Y
);

Bool RTMovementFindPath(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMovementRef Movement
);

EXTERN_C_END
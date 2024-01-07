#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTLevel {
    UInt8 Level;
    UInt64 Exp;
};

#pragma pack(pop)

UInt8 RTRuntimeGetLevelByExp(
    RTRuntimeRef Runtime,
    UInt64 Exp
);

UInt64 RTRuntimeGetExpByLevel(
    RTRuntimeRef Runtime,
    UInt8 Level
);

EXTERN_C_END
#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTGiftBoxInfo {
    UInt8 Index;
    UInt32 ReceivedCount;
    Timestamp ElapsedTime;
    Timestamp CooldownTime;
    UInt8 Active;
};

#pragma pack(pop)

EXTERN_C_END
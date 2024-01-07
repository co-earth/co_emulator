#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_NPC_TYPE_SHOP = 1 << 0,
};

enum {
    RUNTIME_NPC_ID_WAR_0                = 68,
    RUNTIME_NPC_ID_WAR_1                = 67,
    RUNTIME_NPC_ID_WAR_WARP             = 66,
    RUNTIME_NPC_ID_WAR_BATTLEFIELD      = 65,
    RUNTIME_NPC_ID_WAR_LOBBY            = 64,
    RUNTIME_NPC_ID_DEAD                 = 63,
    RUNTIME_NPC_ID_RETURN               = 62,
    RUNTIME_NPC_ID_NAVIGATION           = 61,
    RUNTIME_NPC_ID_UNKNOWN_1            = 60,
    RUNTIME_NPC_ID_UNKNOWN_2            = 59,
    RUNTIME_NPC_ID_QUEST_DUNGEON_WARP   = 58,
    RUNTIME_NPC_ID_QUEST_DUNGEON_EXIT   = 57,
    RUNTIME_NPC_ID_UNKNOWN_3            = 56,
    RUNTIME_NPC_ID_PRISON               = 55,
    RUNTIME_NPC_ID_UNKNOWN_4            = 54,
    RUNTIME_NPC_ID_UNKNOWN_5            = 53,
    RUNTIME_NPC_ID_GM                   = 52,

    RUNTIME_NPC_ID_RESERVED_BEGIN       = RUNTIME_NPC_ID_GM,
    RUNTIME_NPC_ID_RESERVED_END         = RUNTIME_NPC_ID_WAR_0,
};

struct _RTNpc {
    Int32 ID;
    Int32 WorldID;
    UInt32 Type;
    Int32 X;
    Int32 Y;
};

#pragma pack(pop)

EXTERN_C_END
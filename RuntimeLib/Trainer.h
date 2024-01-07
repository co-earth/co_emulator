#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTTrainerSkillData {
    Int32 ID;
    Int32 SlotID;
    Int32 Level;
    Int32 SkillBookID;
};

struct _RTTrainerData {
    Int32 Index;
    Int32 WorldID;
    Int32 NpcID;
    Int32 SkillCount;
    struct _RTTrainerSkillData Skills[RUNTIME_TRAINER_MAX_SKILL_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
#pragma once

#include "Base.h"
#include "Constants.h"
#include "Character.h"
#include "Mob.h"

EXTERN_C_BEGIN

struct _RTDropItem {
    RTItem ItemID;
    UInt64 ItemOptions;
    Int32 DropRate;
    Int32 OptionPoolIndex;
    Int32 DurationIndex;
};

struct _RTWorldDropItem {
    struct _RTDropItem Item;
    Int32 MinMobLevel;
    Int32 MaxMobLevel;
};

struct _RTMobDropItem {
    struct _RTDropItem Item;
    Int32 MobSpeciesID;
};

struct _RTQuestDropItem {
    struct _RTDropItem Item;
    Int32 MobSpeciesID;
};

struct _RTDropTable {
    Int32 WorldItemCount;
    Int32 MobItemCount;
    Int32 QuestItemCount;
    struct _RTWorldDropItem WorldItems[RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT];
    struct _RTMobDropItem MobItems[RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT];
    struct _RTQuestDropItem QuestItems[RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT];
};

struct _RTDropResult {
    RTItem ItemID;
    UInt64 ItemOptions;
    RTItemDuration Duration;
};

Bool RTCalculateDrop(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
);

Bool RTCalculateQuestDrop(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
);

EXTERN_C_END
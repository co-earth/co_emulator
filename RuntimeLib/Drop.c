#include "Character.h"
#include "Drop.h"
#include "Mob.h"
#include "World.h"

RTDropItemRef kDropItemList[RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT + RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT + RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT];

Bool RTCalculateDrop(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
) {
    Int32 DropItemCount = 0;
    Int32 DropRateLimit = 0;

    for (Int32 Index = 0; Index < World->DropTable.WorldItemCount; Index++) {
        RTWorldDropItemRef DropItem = &World->DropTable.WorldItems[Index];

        if (DropItem->MinMobLevel <= Mob->Data->Level && Mob->Data->Level <= DropItem->MaxMobLevel) {
            kDropItemList[DropItemCount] = &DropItem->Item;
            DropItemCount += 1;
            DropRateLimit += DropItem->Item.DropRate;
        }
    }

    for (Int32 Index = 0; Index < World->DropTable.MobItemCount; Index++) {
        RTMobDropItemRef DropItem = &World->DropTable.MobItems[Index];

        if (DropItem->MobSpeciesID == Mob->Spawn.MobSpeciesID) {
            kDropItemList[DropItemCount] = &DropItem->Item;
            DropItemCount += 1;
            DropRateLimit += DropItem->Item.DropRate;
        }
    }

    Int32 DropRate = RandomRange(&World->Seed, 0, DropRateLimit);
    Int32 DropRateOffset = 0;

    for (Int32 Index = 0; Index < DropItemCount; Index++) {
        RTDropItemRef DropItem = kDropItemList[Index];

        if (DropRate <= DropItem->DropRate + DropRateOffset) {
            Result->ItemID = DropItem->ItemID;
            Result->ItemOptions = DropItem->ItemOptions;
            Result->Duration.DurationIndex = DropItem->DurationIndex;
            // TODO: Calculate OptionPoolData
            return true;
        }

        DropRateOffset += DropItem->DropRate;
    }

    return false;
}

Bool RTCalculateQuestDrop(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
) {
    Int32 DropItemCount = 0;
    Int32 DropRateLimit = 0;

    for (Int32 Index = 0; Index < World->DropTable.QuestItemCount; Index++) {
        RTQuestDropItemRef DropItem = &World->DropTable.QuestItems[Index];
        if (DropItem->MobSpeciesID != Mob->Spawn.MobSpeciesID) continue;

        if (RTCharacterHasQuestItemCounter(Runtime, Character, DropItem->Item.ItemID, DropItem->Item.ItemOptions)) {
            kDropItemList[DropItemCount] = &DropItem->Item;
            DropItemCount += 1;
            DropRateLimit += DropItem->Item.DropRate;
        }
    }

    Int32 DropRate = RandomRange(&World->Seed, 0, DropRateLimit);
    Int32 DropRateOffset = 0;

    for (Int32 Index = 0; Index < DropItemCount; Index++) {
        RTDropItemRef DropItem = kDropItemList[Index];

        if (DropRate <= DropItem->DropRate + DropRateOffset) {
            Result->ItemID = DropItem->ItemID;
            Result->ItemOptions = RTQuestItemOptions(DropItem->ItemOptions, 1);
            Result->Duration.DurationIndex = DropItem->DurationIndex;

            // TODO: Calculate OptionPoolData
            return true;
        }

        DropRateOffset += DropItem->DropRate;
    }

    return false;
}

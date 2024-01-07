#include "Runtime.h"
#include "Dungeon.h"
#include "World.h"
#include "Mob.h"

Bool RTDungeonIsPatternPartCompleted(
    RTRuntimeRef Runtime,
    RTWorldRef World
) {
    if (World->PatternPartIndex == -1) return true;

    // TODO: Check mission items

    for (Int32 Index = 0; Index < World->MissionMobCount; Index++) {
        if (World->MissionMobs[Index].Count < World->MissionMobs[Index].MaxCount) {
            return false;
        }
    }

    return true;
}

Bool RTDungeonStartNextPatternPart(
    RTRuntimeRef Runtime,
    RTWorldRef World
) {
    if (!RTDungeonIsPatternPartCompleted(Runtime, World)) return false;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);

    if (World->PatternPartIndex >= 0) {
        RTRuntimeBroadcastEvent(
            Runtime,
            RUNTIME_EVENT_QUEST_DUNGEON_PATTERN_PART_COMPLETED,
            World,
            kEntityNull,
            kEntityNull,
            -1,
            -1
        );
    }

    Int32 PatternPartIndex = World->PatternPartIndex + 1;
    if (PatternPartIndex >= DungeonData->PatternPartCount) {
        return true;
    }

    RTMissionDungeonPatternPartDataRef PatternPartData = RTRuntimeGetPatternPartByID(
        Runtime,
        DungeonData->PatternPartIndices[PatternPartIndex]
    );

    RTWorldSetMobTable(Runtime, World, &PatternPartData->MobTable);

    World->MissionItemCount = 0;
    memset(World->MissionItems, 0, sizeof(struct _RTQuestUnitItemData) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

    World->MissionMobCount = PatternPartData->MissionMobCount;
    memcpy(World->MissionMobs, PatternPartData->MissionMobs, sizeof(struct _RTQuestUnitMobData) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

    for (Int32 Index = 0; Index < World->MissionMobCount; Index++) {
        World->MissionMobs[Index].Count = 0;
    }
    
    World->PatternPartIndex += 1;

    return true;
}

Bool RTDungeonStart(
	RTRuntimeRef Runtime,
	RTWorldRef World
) {
    if (World->Active) return true;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);
    assert(DungeonData);

    World->PatternPartIndex = -1;

    RTDungeonStartNextPatternPart(Runtime, World);

    // TODO: Start mission timeout
    // TODO: Generate maze 

    World->Active = true;
	return true;
}

Bool RTDungeonEnd(
    RTRuntimeRef Runtime,
    RTWorldRef World
) {
    if (!World->Active || World->Closed) return true;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);
    assert(DungeonData);

    if (!RTDungeonIsPatternPartCompleted(Runtime, World)) return false;

    Int32 PatternPartIndex = World->PatternPartIndex + 1;
    if (PatternPartIndex >= DungeonData->PatternPartCount) {
        World->Closed = true;
        return true;
    }

    return false;
}

Bool RTDungeonTriggerEvent(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    Int32 TriggerID
) {
    assert(World->Type == RUNTIME_WORLD_TYPE_DUNGEON || World->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON);

    Bool Triggered = false;
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);
    for (Int32 Index = 0; Index < DungeonData->TriggerCount; Index++) {
        RTDungeonTriggerDataRef TriggerData = &DungeonData->TriggerData[Index];
        if (TriggerData->TriggerID != TriggerID) continue;

        for (Int32 Index = 0; Index < TriggerData->LiveMobCount; Index++) {
            RTMobRef Mob = RTWorldGetMob(Runtime, World, TriggerData->LiveMobs[Index]);
            if (!Mob || Mob->IsDead) return false;
        }

        for (Int32 Index = 0; Index < TriggerData->DeadMobCount; Index++) {
            RTMobRef Mob = RTWorldGetMob(Runtime, World, TriggerData->DeadMobs[Index]);
            if (!Mob || Mob->IsDead) return false;
        }

        for (Int32 Index = 0; Index < DungeonData->TriggerActionCount; Index++) {
            RTDungeonTriggerActionDataRef ActionData = &DungeonData->TriggerActionData[Index];
            if (ActionData->ActionGroupID != TriggerData->ActionGroupID) continue;

            // TODO: Add support for ActionData event delay
            if (ActionData->ActionType == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_EVENT_CALL) {
                RTDungeonTriggerEvent(Runtime, World, ActionData->TargetID);
                Triggered = true;
            }

            RTMobRef Mob = RTWorldGetMob(Runtime, World, ActionData->TargetID);
            
            if (ActionData->ActionType == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_SPAWN) {
                if (Mob->IsPermanentDeath) break;
                if (RTMobIsAlive(Mob)) break;

                RTWorldSpawnMob(Runtime, World, Mob);
                Triggered = true;
            }

            if (ActionData->ActionType == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_KILL) {
                if (!RTMobIsAlive(Mob)) break;
                
                RTWorldDespawnMob(Runtime, World, Mob);
                Triggered = true;
            }

            if (ActionData->ActionType == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_REVIVE) {
                if (RTMobIsAlive(Mob)) break;

                RTWorldSpawnMob(Runtime, World, Mob);
                Triggered = true;
            }

            if (ActionData->ActionType == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_DELETE) {
                Mob->IsPermanentDeath = true;
                RTWorldDespawnMob(Runtime, World, Mob);
                Triggered = true;
            }
        }

        return Triggered;
    }

    return false;
}

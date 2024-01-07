#include "Character.h"
#include "Mob.h"
#include "Runtime.h"
#include "World.h"

RTRuntimeRef RTRuntimeCreate(
    RTEventCallback Callback,
    Void* UserData
) {
    RTRuntimeRef Runtime = (RTRuntimeRef)malloc(sizeof(struct _RTRuntime));
    if (!Runtime) FatalError("Memory allocation failed!");
    memset(Runtime, 0, sizeof(struct _RTRuntime));

    for (Int32 X = 0; X < RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH; X++) {
        for (Int32 Y = 0; Y < RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH; Y++) {
            Int32 Index = X + Y * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH;
            Runtime->MovementDistanceCache[Index] = (Float32)sqrt((Float64)X * (Float64)X + (Float64)Y * (Float64)Y);
        }
    }

    Runtime->Context = RTRuntimeDataContextCreate();
    Runtime->Callback = Callback;
    Runtime->UserData = UserData;
    return Runtime;
}

Bool RTRuntimeLoadData(
    RTRuntimeRef Runtime,
    CString Directory
) {
    return RTRuntimeDataContextLoad(Runtime->Context, Directory);
}

Void RTRuntimeDestroy(
    RTRuntimeRef Runtime
) {
    RTRuntimeDataContextDestroy(Runtime->Context);
    free(Runtime);
}

Void RTRuntimeUpdate(
    RTRuntimeRef Runtime
) {
    for (Int32 Index = 0; Index < Runtime->WorldCount; Index++) {
        RTWorldRef World = &Runtime->Worlds[Index];
        RTWorldUpdate(Runtime, World);
    }

    for (Int32 Index = 0; Index < Runtime->DungeonInstanceCount; Index++) {
        RTWorldRef World = &Runtime->Dungeons[Index];
        if (!World->Active || World->Closed) continue;

        RTWorldUpdate(Runtime, World);
    }

    /* Movement Debugging
    for (Int32 Index = 0; Index < Runtime->Characters.Count; Index++) {
        RTCharacterRef Character = (RTCharacterRef)ArrayGetElementAtIndex(&Runtime->Characters, Index);
        if (Character->Movement.Flags & RUNTIME_MOVEMENT_IS_DEAD_RECKONING) {
            RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);

            LogMessageFormat(
                LOG_LEVEL_INFO,
                "(%d, %d)",
                Character->Movement.PositionCurrent.X,
                Character->Movement.PositionCurrent.Y
            );
        }
    }
    */
}

Void RTRuntimeBroadcastEvent(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldRef World,
    RTEntity SourceID,
    RTEntity TargetID,
    Int32 X,
    Int32 Y
) {
    Runtime->Event.Type = EventType;
    Runtime->Event.World = World;
    Runtime->Event.SourceID = SourceID;
    Runtime->Event.TargetID = TargetID;
    Runtime->Event.X = X;
    Runtime->Event.Y = Y;
    Runtime->Callback(Runtime, &Runtime->Event, Runtime->UserData);
}

Void RTRuntimeBroadcastEventData(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldRef World,
    RTEntity SourceID,
    RTEntity TargetID,
    Int32 X,
    Int32 Y,
    RTEventData Data
) {
    Runtime->Event.Type = EventType;
    Runtime->Event.World = World;
    Runtime->Event.SourceID = SourceID;
    Runtime->Event.TargetID = TargetID;
    Runtime->Event.X = X;
    Runtime->Event.Y = Y;
    Runtime->Event.Data = Data;
    Runtime->Callback(Runtime, &Runtime->Event, Runtime->UserData);
}

RTEntity RTRuntimeCreateEntity(
    RTRuntimeRef Runtime,
    UInt8 WorldIndex,
    UInt8 EntityType
) {
    assert(Runtime->EntityCount < RUNTIME_MEMORY_MAX_ENTITY_COUNT);

    for (Int32 Index = 0; Index < RUNTIME_MEMORY_MAX_ENTITY_COUNT; Index += 1) {
        if (Runtime->Entities[Index].Serial) continue;

        Runtime->Entities[Index].EntityIndex = Index + 1;
        Runtime->Entities[Index].WorldIndex = WorldIndex;
        Runtime->Entities[Index].EntityType = EntityType;
        Runtime->EntityCount += 1;
        return Runtime->Entities[Index];
    }

    FatalError("Maximum amount of entities reached!");
    return kEntityNull;
}

Void RTRuntimeDeleteEntity(
    RTRuntimeRef Runtime,
    RTEntity Entity
) {
    Int32 Index = Entity.EntityIndex - 1;

    assert(Index < RUNTIME_MEMORY_MAX_ENTITY_COUNT);
    assert(Runtime->Entities[Index].Serial == Entity.Serial);

    Runtime->Entities[Index] = kEntityNull;
    Runtime->EntityCount -= 1;
}

Void _RTRuntimeInitializeCharacter(
    RTCharacterRef Character
) {

}

Void _RTRuntimeDeinitializeCharacter(
    RTCharacterRef Character
) {

}

RTCharacterRef RTRuntimeCreateCharacter(
    RTRuntimeRef Runtime
) {
    assert(Runtime->CharacterCount < RUNTIME_MEMORY_MAX_CHARACTER_COUNT);

    RTEntity Entity = RTRuntimeCreateEntity(Runtime, 0, RUNTIME_ENTITY_TYPE_CHARACTER);
    Int32 Index = Entity.EntityIndex - 1;
    Runtime->EntityToCharacter[Index] = Runtime->CharacterCount;
    Runtime->CharacterCount += 1;

    RTCharacterRef Character = &Runtime->Characters[Runtime->EntityToCharacter[Index]];
    memset(Character, 0, sizeof(struct _RTCharacter));
    Character->ID = Entity;
    return Character;
}

RTWorldRef RTRuntimeGetWorldByID(
    RTRuntimeRef Runtime,
    Int32 WorldID
) {
    for (Int32 Index = 0; Index < Runtime->WorldCount; Index++) {
        RTWorldRef World = &Runtime->Worlds[Index];
        if (World->WorldIndex == WorldID) return World;
    }

    return NULL;
}

RTWorldRef RTRuntimeGetWorldByCharacter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    for (Int32 Index = 0; Index < Runtime->WorldCount; Index++) {
        RTWorldRef World = &Runtime->Worlds[Index];
        if (World->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) continue;
        if (World->WorldIndex == Character->Info.Position.WorldID) return World;
    }

    for (Int32 Index = 0; Index < Runtime->DungeonInstanceCount; Index++) {
        RTWorldRef World = &Runtime->Dungeons[Index];
        if (World->WorldIndex == Character->Info.Position.WorldID && World->Owner.Serial == Character->ID.Serial) return World;
    }

    return NULL;
}

Void _RTRuntimeInitializeWorld(
    RTRuntimeRef Runtime,
    RTWorldRef World
) {
    World->Tiles = malloc(sizeof(UInt32) * RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE);
    if (!World->Tiles) FatalError("Memory allocation failed!");
    memset(World->Tiles, 0, sizeof(UInt32) * RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE);
    ArrayInitializeEmpty(&World->Characters, sizeof(RTEntity), 0);
}

Void _RTRuntimeDeinitializeWorld(
    RTWorldRef World
) {
    ArrayDeinitialize(&World->Characters);
}

RTWorldIndex RTRuntimeCreateWorld(
    RTRuntimeRef Runtime
) {
    assert(Runtime->WorldCount < RUNTIME_MEMORY_MAX_WORLD_COUNT);

    RTWorldIndex WorldIndex = Runtime->WorldCount;
    Runtime->WorldCount += 1;

    RTWorldRef World = &Runtime->Worlds[WorldIndex];
    memset(World, 0, sizeof(struct _RTWorld));
    _RTRuntimeInitializeWorld(Runtime, World);
    return WorldIndex;
}

RTNpcRef RTRuntimeGetNpcByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
) {
    for (Int32 Index = 0; Index < Runtime->NpcCount; Index++) {
        RTNpcRef Npc = &Runtime->Npcs[Index];
        if (Npc->WorldID == WorldID && Npc->ID == NpcID) return Npc;
    }

    return NULL;
}

RTWarpRef RTRuntimeGetWarpByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID,
    Int32 WarpIndex
) {
    for (Int32 Index = 0; Index < Runtime->WarpCount; Index++) {
        RTWarpRef Warp = &Runtime->Warps[Index];
        if (Warp->WorldID == WorldID && Warp->NpcID == NpcID && Warp->Index == WarpIndex) return Warp;
    }

    return NULL;
}

RTWarpRef RTRuntimeGetWarpByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 WarpIndex
) {
    if (WarpIndex < 0 || WarpIndex >= Runtime->WarpCount) return NULL;

    return &Runtime->Warps[WarpIndex];
}

RTCharacterRef RTRuntimeGetCharacter(
    RTRuntimeRef Runtime,
    RTEntity Entity
) {    
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);
    assert(Entity.EntityIndex > 0);

    Int32 Index = Entity.EntityIndex - 1;
    assert(Runtime->CharacterCount > Index);
    return &Runtime->Characters[Runtime->EntityToCharacter[Index]];
}

UInt32 RTRuntimeGetCharacterIndex(
    RTRuntimeRef Runtime,
    RTEntity Entity
) {
    RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Entity);
    assert(Character);
    return Character->CharacterIndex;
}

RTWorldItemRef RTRuntimeGetItem(
    RTRuntimeRef Runtime,
    RTEntity Entity
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_ITEM);

    RTWorldRef World = RTRuntimeGetWorldByID(Runtime, Entity.WorldIndex);
    if (!World) return NULL;

    return RTWorldGetItemByEntity(Runtime, World, Entity);
}

RTWorldItemRef RTRuntimeGetItemByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 ItemID
) {
    RTWorldRef World = RTRuntimeGetWorldByID(Runtime, WorldID);
    assert(World);
   
    for (Int32 Index = 0; Index < World->ItemCount; Index++) {
        RTWorldItemRef Item = &World->Items[Index];
        if (Item->Index == ItemID) return Item;
    }

    return NULL;
}

RTItemDataRef RTRuntimeGetItemDataByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemIndex
) {
    UInt32 ItemID = (ItemIndex & RUNTIME_ITEM_MASK_INDEX);
    for (Int32 Index = 0; Index < Runtime->ItemDataCount; Index++) {
        RTItemDataRef Item = &Runtime->ItemData[Index];
        if (Item->ItemID == ItemID) {
            return Item;
        }
    }

    return NULL;
}

RTQuestDataRef RTRuntimeGetQuestByIndex(
    RTRuntimeRef Runtime,
    Int32 QuestIndex
) {
    for (Int32 Index = 0; Index < Runtime->QuestDataCount; Index++) {
        RTQuestDataRef Quest = &Runtime->QuestData[Index];
        if (Quest->ID == QuestIndex) {
            return Quest;
        }
    }

    return NULL;
}

RTQuestRewardItemSetDataRef RTRuntimeGetQuestRewardItemSetByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex
) {
    if (ItemSetIndex < 1) return NULL;

    for (Int32 Index = 0; Index < Runtime->QuestRewardItemSetDataCount; Index++) {
        RTQuestRewardItemSetDataRef ItemSet = &Runtime->QuestRewardItemSetData[Index];
        if (ItemSet->ID == ItemSetIndex) {
            return ItemSet;
        }
    }

    return NULL;
}

RTQuestRewardItemDataRef RTRuntimeGetQuestRewardItemByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex,
    Int32 ItemIndex,
    Int32 BattleStyleIndex
) {
    RTQuestRewardItemSetDataRef ItemSet = RTRuntimeGetQuestRewardItemSetByIndex(Runtime, ItemSetIndex);
    if (!ItemSet) return NULL;

    for (Int32 Index = 0; Index < ItemSet->Count; Index++) {
        RTQuestRewardItemDataRef Item = &ItemSet->Items[Index];
        if (Item->Index == ItemIndex && (Item->BattleStyleIndex == BattleStyleIndex || Item->BattleStyleIndex == 0)) {
            return Item;
        }
    }

    return NULL;
}

RTShopDataRef RTRuntimeGetShopByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
) {
    for (Int32 Index = 0; Index < Runtime->ShopDataCount; Index++) {
        RTShopDataRef Shop = &Runtime->ShopData[Index];
        if (Shop->WorldID == WorldID && Shop->NpcID == NpcID) {
            return Shop;
        }
    }

    return NULL;
}

RTShopDataRef RTRuntimeGetShopByIndex(
    RTRuntimeRef Runtime,
    Int32 ShopIndex
) {
    for (Int32 Index = 0; Index < Runtime->ShopDataCount; Index++) {
        RTShopDataRef Shop = &Runtime->ShopData[Index];
        if (Shop->Index == ShopIndex) {
            return Shop;
        }
    }

    return NULL;
}

RTTrainerDataRef RTRuntimeGetTrainerByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
) {
    for (Int32 Index = 0; Index < Runtime->TrainerDataCount; Index++) {
        RTTrainerDataRef Trainer = &Runtime->TrainerData[Index];
        if (Trainer->WorldID == WorldID && Trainer->NpcID == NpcID) {
            return Trainer;
        }
    }

    return NULL;
}

RTCharacterSkillDataRef RTRuntimeGetCharacterSkillDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID
) {
    for (Int32 Index = 0; Index < Runtime->CharacterSkillDataCount; Index++) {
        if (Runtime->CharacterSkillData[Index].SkillID == SkillID) {
            return &Runtime->CharacterSkillData[Index];
        }
    }

    return NULL;
}

RTSkillLevelDataRef RTRuntimeGetSkillLevelDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID,
    Int32 SkillLevel
) {
    RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillID);
    if (!SkillData) return NULL;

    for (Int32 Index = 0; Index < SkillData->SkillLevelCount; Index++) {
        if (SkillData->SkillLevels[Index].StartLevel <= SkillLevel && SkillLevel <= SkillData->SkillLevels[Index].EndLevel) {
            return &SkillData->SkillLevels[Index];
        }
    }

    return NULL;
}

RTBattleStyleLevelFormulaDataRef RTRuntimeGetBattleStyleLevelFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    return &Runtime->BattleStyleLevelFormulaData[BattleStyleIndex - 1];

}

RTBattleStyleClassFormulaDataRef RTRuntimeGetBattleStyleClassFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    return &Runtime->BattleStyleClassFormulaData[BattleStyleIndex - 1];
}

RTBattleStyleSlopeDataRef RTRuntimeGetBattleStyleSlopeData(
    RTRuntimeRef Runtime,
    Int32 SlopeID,
    Int32 Penalty
) {
    RTBattleStyleSlopeFormulaDataRef Formula = RTRuntimeGetBattleStyleSlopeFormulaData(Runtime, SlopeID);
    assert(Formula && Formula->SlopeCount > 0);

    Int32 SlopePenalty = 0;
    Int32 SlopeIndex = 0;
    for (Int32 Index = 0; Index < Formula->SlopeCount; Index++) {
        if (Formula->Slopes[Index].Penalty <= Penalty && Formula->Slopes[Index].Penalty >= SlopePenalty) {
            SlopePenalty = Formula->Slopes[Index].Penalty;
            SlopeIndex = Index;
        }
    }

    return &Formula->Slopes[SlopeIndex];
}

RTBattleStyleSlopeFormulaDataRef RTRuntimeGetBattleStyleSlopeFormulaData(
    RTRuntimeRef Runtime,
    Int32 SlopeID
) {
    for (Int32 Index = 0; Index < Runtime->SlopeFormulaDataCount; Index++) {
        if (Runtime->BattleStyleSlopeFormulaData[Index].SlopeID == SlopeID) {
            return &Runtime->BattleStyleSlopeFormulaData[Index];
        }
    }

    return NULL;
}

RTBattleStyleStatsFormulaDataRef RTRuntimeGetBattleStyleStatsFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    return &Runtime->BattleStyleStatsFormulaData[BattleStyleIndex - 1];
}

RTBattleStyleSkillRankDataRef RTRuntimeGetBattleStyleSkillRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 SkillRank
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    RTBattleStyleSkillFormulaDataRef FormulaData = &Runtime->BattleStyleSkillFormulaData[BattleStyleIndex - 1];
    for (Int32 Index = 0; Index < FormulaData->SkillRankCount; Index++) {
        if (FormulaData->SkillRanks[Index].SkillRank == SkillRank) {
            return &FormulaData->SkillRanks[Index];
        }
    }

    return NULL;
}

RTBattleStyleRankDataRef RTRuntimeGetBattleStyleRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 Level
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    RTBattleStyleRankFormulaDataRef FormulaData = &Runtime->BattleStyleRankFormulaData[BattleStyleIndex - 1];
    for (Int32 Index = 0; Index < FormulaData->RankCount; Index++) {
        if (FormulaData->Ranks[Index].Level == Level) {
            return &FormulaData->Ranks[Index];
        }
    }

    return NULL;
}

RTWorldRef RTRuntimeOpenDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTWorldRef World,
    Int32 DungeonID
) {
    if (World->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return NULL;

    RTWorldRef CurrentWorld = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (CurrentWorld->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return NULL;

    if (Runtime->DungeonInstanceCount + 1 >= RUNTIME_MEMORY_MAX_WORLD_COUNT) return NULL;

    RTWorldRef WorldInstance = &Runtime->Dungeons[Runtime->DungeonInstanceCount];
    memcpy(WorldInstance, World, sizeof(struct _RTWorld));

    WorldInstance->Owner = Character->ID;
    WorldInstance->DungeonID = DungeonID;

    Runtime->DungeonInstanceCount += 1;

    return WorldInstance;
}

RTDungeonDataRef RTRuntimeGetDungeonDataByID(
    RTRuntimeRef Runtime,
    Int32 DungeonID
) {
    for (Int32 Index = 0; Index < Runtime->DungeonDataCount; Index++) {
        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Index];
        if (DungeonData->DungeonID == DungeonID) {
            return DungeonData;
        }
    }

    return NULL;
}

RTMissionDungeonPatternPartDataRef RTRuntimeGetPatternPartByID(
    RTRuntimeRef Runtime,
    Int32 PatternPartID
) {
    for (Int32 Index = 0; Index < Runtime->MissionDungeonPatternPartDataCount; Index++) {
        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Index];
        if (PatternPartData->ID == PatternPartID) return PatternPartData;
    }

    return NULL;
}

#pragma once

#include "Base.h"
#include "Constants.h"
#include "Character.h"
#include "Dungeon.h"
#include "Event.h"
#include "Mob.h"
#include "Npc.h"
#include "Quest.h"
#include "Shop.h"
#include "Trainer.h"
#include "Upgrade.h"
#include "Warp.h"
#include "World.h"
#include "Level.h"

EXTERN_C_BEGIN

struct _RTRuntimeConfig {
    UInt64 ExpMultiplier;
    UInt64 SkillExpMultiplier;
};

struct _RTRuntime {
    struct _RTRuntimeConfig Config;
    RTRuntimeDataContextRef Context;

    Int32 EntityCount;
    RTEntity Entities[RUNTIME_MEMORY_MAX_ENTITY_COUNT];
    Int32 EntityToCharacter[RUNTIME_MEMORY_MAX_CHARACTER_COUNT];

    Int32 CharacterCount;
    struct _RTCharacter Characters[RUNTIME_MEMORY_MAX_CHARACTER_COUNT];

    Int32 SlopeFormulaDataCount;
    Int32 CharacterSkillDataCount;
    Int32 ItemDataCount;
    Int32 MobDataCount;
    Int32 NpcCount;
    Int32 QuestDataCount;
    Int32 QuestRewardItemSetDataCount;
    Int32 ShopDataCount;
    Int32 TrainerDataCount;
    Int32 WarpIndexCount;
    Int32 WarpCount;
    Int32 WorldCount;
    Int32 LevelCount;
    Int32 DungeonInstanceCount;
    Int32 DungeonDataCount;
    Int32 DungeonMobSpawnDataCount;
    Int32 MissionDungeonPatternPartDataCount;

    struct _RTBattleStyleLevelFormulaData BattleStyleLevelFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleClassFormulaData BattleStyleClassFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleSlopeFormulaData BattleStyleSlopeFormulaData[RUNTIME_MEMORY_MAX_SLOPE_FORMULA_COUNT];
    struct _RTBattleStyleStatsFormulaData BattleStyleStatsFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleSkillFormulaData BattleStyleSkillFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleRankFormulaData BattleStyleRankFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTCharacterSkillData CharacterSkillData[RUNTIME_MEMORY_MAX_CHARACTER_SKILL_DATA_COUNT];
    struct _RTItemData ItemData[RUNTIME_MEMORY_MAX_ITEM_DATA_COUNT];
    struct _RTMobData MobData[RUNTIME_MEMORY_MAX_MOB_DATA_COUNT];
    struct _RTNpc Npcs[RUNTIME_MEMORY_MAX_NPC_COUNT];
    struct _RTQuestData QuestData[RUNTIME_MEMORY_MAX_QUEST_DATA_COUNT];
    struct _RTQuestRewardItemSetData QuestRewardItemSetData[RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_SET_DATA_COUNT];
    struct _RTShopData ShopData[RUNTIME_MEMORY_MAX_SHOP_DATA_COUNT];
    struct _RTTrainerData TrainerData[RUNTIME_MEMORY_MAX_TRAINER_DATA_COUNT];
    struct _RTUpgradeData UpgradeData[RUNTIME_UPGRADE_TYPE_COUNT];
    struct _RTWarpIndex WarpIndices[RUNTIME_MEMORY_MAX_WARP_INDEX_COUNT];
    struct _RTWarp Warps[RUNTIME_MEMORY_MAX_WARP_COUNT];
    struct _RTWorld Worlds[RUNTIME_MEMORY_MAX_WORLD_COUNT];
    struct _RTWorld Dungeons[RUNTIME_MEMORY_MAX_WORLD_COUNT];
    struct _RTLevel Levels[RUNTIME_MEMORY_MAX_LEVEL_COUNT];
    struct _RTDungeonData DungeonData[RUNTIME_MEMORY_MAX_DUNGEON_COUNT];
    struct _RTMissionDungeonPatternPartData MissionDungeonPatternPartData[RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT];

    Float32 MovementDistanceCache[RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH];

    struct _RTEvent Event;
    RTEventCallback Callback;
    Void* UserData;
};

RTRuntimeRef RTRuntimeCreate(
    RTEventCallback Callback,
    Void* UserData
);

Bool RTRuntimeLoadData(
    RTRuntimeRef Runtime,
    CString Directory
);

Void RTRuntimeDestroy(
    RTRuntimeRef Runtime
);

Void RTRuntimeUpdate(
    RTRuntimeRef Runtime
);

Void RTRuntimeBroadcastEvent(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldRef World,
    RTEntity SourceID,
    RTEntity TargetID,
    Int32 X,
    Int32 Y
);

Void RTRuntimeBroadcastEventData(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldRef World,
    RTEntity SourceID,
    RTEntity TargetID,
    Int32 X,
    Int32 Y,
    RTEventData Data
);

RTEntity RTRuntimeCreateEntity(
    RTRuntimeRef Runtime,
    UInt8 WorldIndex,
    UInt8 EntityType
);

Void RTRuntimeDeleteEntity(
    RTRuntimeRef Runtime,
    RTEntity Entity
);

RTCharacterRef RTRuntimeCreateCharacter(
    RTRuntimeRef Runtime
);

RTWorldRef RTRuntimeGetWorldByID(
    RTRuntimeRef Runtime,
    Int32 WorldID
);

RTWorldRef RTRuntimeGetWorldByCharacter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

RTWorldIndex RTRuntimeCreateWorld(
    RTRuntimeRef Runtime
);

RTNpcRef RTRuntimeGetNpcByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
);

RTWarpRef RTRuntimeGetWarpByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID,
    Int32 WarpIndex
);

RTWarpRef RTRuntimeGetWarpByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 WarpIndex
);

RTCharacterRef RTRuntimeGetCharacter(
    RTRuntimeRef Runtime,
    RTEntity Entity
);

UInt32 RTRuntimeGetCharacterIndex(
    RTRuntimeRef Runtime,
    RTEntity Entity
);

RTWorldItemRef RTRuntimeGetItem(
    RTRuntimeRef Runtime,
    RTEntity Entity
);

RTWorldItemRef RTRuntimeGetItemByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 ItemID
);

RTItemDataRef RTRuntimeGetItemDataByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemIndex
);

RTQuestDataRef RTRuntimeGetQuestByIndex(
    RTRuntimeRef Runtime,
    Int32 QuestIndex
);

RTQuestRewardItemSetDataRef RTRuntimeGetQuestRewardItemSetByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex
);

RTQuestRewardItemDataRef RTRuntimeGetQuestRewardItemByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex,
    Int32 ItemIndex,
    Int32 BattleStyleIndex
);

RTShopDataRef RTRuntimeGetShopByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
);

RTShopDataRef RTRuntimeGetShopByIndex(
    RTRuntimeRef Runtime,
    Int32 ShopIndex
);

RTTrainerDataRef RTRuntimeGetTrainerByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
);

RTCharacterSkillDataRef RTRuntimeGetCharacterSkillDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID
);

RTSkillLevelDataRef RTRuntimeGetSkillLevelDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID,
    Int32 SkillLevel
);

RTBattleStyleLevelFormulaDataRef RTRuntimeGetBattleStyleLevelFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
);

RTBattleStyleClassFormulaDataRef RTRuntimeGetBattleStyleClassFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
);

RTBattleStyleSlopeDataRef RTRuntimeGetBattleStyleSlopeData(
    RTRuntimeRef Runtime,
    Int32 SlopeID,
    Int32 Penalty
);

RTBattleStyleSlopeFormulaDataRef RTRuntimeGetBattleStyleSlopeFormulaData(
    RTRuntimeRef Runtime,
    Int32 SlopeID
);

RTBattleStyleStatsFormulaDataRef RTRuntimeGetBattleStyleStatsFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
);

RTBattleStyleSkillRankDataRef RTRuntimeGetBattleStyleSkillRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 SkillRank
);

RTBattleStyleRankDataRef RTRuntimeGetBattleStyleRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 Level
);

RTWorldRef RTRuntimeOpenDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTWorldRef World,
    Int32 DungeonID
);

RTDungeonDataRef RTRuntimeGetDungeonDataByID(
    RTRuntimeRef Runtime,
    Int32 DungeonID
);

RTMissionDungeonPatternPartDataRef RTRuntimeGetPatternPartByID(
    RTRuntimeRef Runtime,
    Int32 PatternPartID
);

EXTERN_C_END
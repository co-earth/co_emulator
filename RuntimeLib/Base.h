#pragma once

#include <CoreLib/CoreLib.h>
#include <RuntimeDataLib/RuntimeDataLib.h>
#include <math.h>

typedef struct _RTBattleAttributes* RTBattleAttributesRef;
typedef struct _RTBattleStyleLevelFormulaData* RTBattleStyleLevelFormulaDataRef;
typedef struct _RTBattleStyleClassFormulaData* RTBattleStyleClassFormulaDataRef;
typedef struct _RTBattleStyleSlopeData* RTBattleStyleSlopeDataRef;
typedef struct _RTBattleStyleSlopeFormulaData* RTBattleStyleSlopeFormulaDataRef;
typedef struct _RTBattleStyleStatsFormulaData* RTBattleStyleStatsFormulaDataRef;
typedef struct _RTBattleStyleSkillRankData* RTBattleStyleSkillRankDataRef;
typedef struct _RTBattleStyleSkillFormulaData* RTBattleStyleSkillFormulaDataRef;
typedef struct _RTBattleStyleRankData* RTBattleStyleRankDataRef;
typedef struct _RTBattleStyleRankFormulaData* RTBattleStyleRankFormulaDataRef;
typedef struct _RTFinalBattleAttributes* RTFinalBattleAttributesRef;
typedef struct _RTBattleResult* RTBattleResultRef;
typedef struct _RTCharacterInfo* RTCharacterInfoRef;
typedef struct _RTCharacterEquipmentInfo* RTCharacterEquipmentInfoRef;
typedef struct _RTCharacterInventoryInfo* RTCharacterInventoryInfoRef;
typedef struct _RTCharacterSkillSlotInfo* RTCharacterSkillSlotInfoRef;
typedef struct _RTCharacterQuickSlotInfo* RTCharacterQuickSlotInfoRef;
typedef struct _RTCharacterQuestFlagInfo* RTCharacterQuestFlagInfoRef;
typedef struct _RTCharacterQuestSlotInfo* RTCharacterQuestSlotInfoRef;
typedef struct _RTCharacter* RTCharacterRef;
typedef struct _RTDropItem* RTDropItemRef;
typedef struct _RTWorldDropItem* RTWorldDropItemRef;
typedef struct _RTMobDropItem* RTMobDropItemRef;
typedef struct _RTQuestDropItem* RTQuestDropItemRef;
typedef struct _RTDropTable* RTDropTableRef;
typedef struct _RTDropResult* RTDropResultRef;
typedef struct _RTEvent* RTEventRef;
typedef struct _RTGiftBoxInfo* RTGiftBoxInfoRef;
typedef struct _RTItemData* RTItemDataRef;
typedef struct _RTItemSlot* RTItemSlotRef;
typedef struct _RTItemSlotAppearance* RTItemSlotAppearanceRef;
typedef struct _RTMobSpawnData* RTMobSpawnDataRef;
typedef struct _RTMobSkillData* RTMobSkillDataRef;
typedef struct _RTMobData* RTMobDataRef;
typedef struct _RTMob* RTMobRef;
typedef struct _RTMobTable* RTMobTableRef;
typedef struct _RTMovement* RTMovementRef;
typedef struct _RTNpc* RTNpcRef;
typedef struct _RTPosition* RTPositionRef;
typedef struct _RTQuestConditionData* RTQuestConditionDataRef;
typedef struct _RTQuestNpcData* RTQuestNpcDataRef;
typedef struct _RTQuestNpcSetData* RTQuestNpcSetDataRef;
typedef struct _RTQuestMissionData* RTQuestMissionDataRef;
typedef struct _RTQuestData* RTQuestDataRef;
typedef struct _RTQuestRewardItemData* RTQuestRewardItemDataRef;
typedef struct _RTQuestRewardItemSetData* RTQuestRewardItemSetDataRef;
typedef struct _RTQuestSlot* RTQuestSlotRef;
typedef struct _RTQuickSlot* RTQuickSlotRef;
typedef struct _RTRuntime* RTRuntimeRef;
typedef struct _RTShopItemData* RTShopItemDataRef;
typedef struct _RTShopData* RTShopDataRef;
typedef struct _RTSkillSlot* RTSkillSlotRef;
typedef struct _RTTrainerSkillData* RTTrainerSkillDataRef;
typedef struct _RTTrainerData* RTTrainerDataRef;
typedef struct _RTSkillLevelData* RTSkillLevelDataRef;
typedef struct _RTCharacterSkillData* RTCharacterSkillDataRef;
typedef struct _RTUpgradeLevelData* RTUpgradeLevelDataRef;
typedef struct _RTUpgradeData* RTUpgradeDataRef;
typedef struct _RTWarpIndex* RTWarpIndexRef;
typedef struct _RTWarp* RTWarpRef;
typedef struct _RTDungeonTriggerData* RTDungeonTriggerDataRef;
typedef struct _RTDungeonTriggerActionData* RTDungeonTriggerActionDataRef;
typedef struct _RTDungeonData* RTDungeonDataRef;
typedef struct _RTMissionDungeonPatternPartData* RTMissionDungeonPatternPartDataRef;
typedef struct _RTWorldItem* RTWorldItemRef;
typedef struct _RTWorld* RTWorldRef;
typedef struct _RTWorldChunk* RTWorldChunkRef;
typedef struct _RTLevel* RTLevelRef;

struct _RTPosition {
	Int32 X;
	Int32 Y;
};
typedef struct _RTPosition RTPosition;

#pragma pack(push, 1)

enum {
    RUNTIME_ENTITY_TYPE_NONE,
	RUNTIME_ENTITY_TYPE_CHARACTER,
	RUNTIME_ENTITY_TYPE_MOB,
	RUNTIME_ENTITY_TYPE_ITEM,
	RUNTIME_ENTITY_TYPE_PARTY,
	RUNTIME_ENTITY_TYPE_OBJECT,
};

typedef UInt16 RTEntityIndex;
typedef UInt8 RTWorldIndex;
typedef UInt8 RTEntityType;

union _RTEntity {
	struct { UInt16 EntityIndex; UInt8 WorldIndex; UInt8 EntityType; };
	
	UInt32 Serial;
};
typedef union _RTEntity RTEntity;

static const RTEntity kEntityNull = { .Serial = 0 };
 

#pragma pack(pop)

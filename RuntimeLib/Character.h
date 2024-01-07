#pragma once

#include "Base.h"

#include "Ability.h"
#include "BattleSystem.h"
#include "Equipment.h"
#include "Item.h"
#include "Movement.h"
#include "Quickslot.h"
#include "Quest.h"
#include "Skill.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_CHARACTER_SYNC_INFO             = 1 << 0,
    RUNTIME_CHARACTER_SYNC_EQUIPMENT        = 1 << 1,
    RUNTIME_CHARACTER_SYNC_INVENTORY        = 1 << 2,
    RUNTIME_CHARACTER_SYNC_SKILLSLOT        = 1 << 3,
    RUNTIME_CHARACTER_SYNC_QUICKSLOT        = 1 << 4,
    RUNTIME_CHARACTER_SYNC_QUESTSLOT        = 1 << 5,
    RUNTIME_CHARACTER_SYNC_QUESTFLAG        = 1 << 6,
};

enum {
    RUNTIME_CHARACTER_SYNC_PRIORITY_LOW     = 1 << 0,
    RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH    = 1 << 1,
    RUNTIME_CHARACTER_SYNC_PRIORITY_INSTANT = 1 << 2,
};

enum {
    RUNTIME_CHARACTER_LEVEL_BASIC,
    RUNTIME_CHARACTER_LEVEL_OVERLORD,
    RUNTIME_CHARACTER_LEVEL_FORCEWING,

    RUNTIME_CHARACTER_LEVEL_COUNT
};

enum {
    RUNTIME_CHARACTER_STAT_STR,
    RUNTIME_CHARACTER_STAT_DEX,
    RUNTIME_CHARACTER_STAT_INT,
    RUNTIME_CHARACTER_STAT_PNT,

    RUNTIME_CHARACTER_STAT_COUNT
};

enum {
    RUNTIME_CHARACTER_CURRENCY_ALZ,
    RUNTIME_CHARACTER_CURRENCY_GEM,

    RUNTIME_CHARACTER_CURRENCY_COUNT
};

union _RTCharacterStyle {
    struct {
        UInt32 AuraCode : 2;
        UInt32 Gender : 1;
        UInt32 Padding1 : 1;
        UInt32 Unknown1 : 1;
        UInt32 Padding2 : 4;
        UInt32 HairStyle : 3;
        UInt32 Padding3 : 2;
        UInt32 ShowHelmed : 1;
        UInt32 ExtendedBattleStyle : 1;
        UInt32 Face : 2;
        UInt32 Padding4 : 3;
        UInt32 HairColor : 3;
        UInt32 BattleStyle : 3;
        UInt32 BattleRank : 5;
    };
    UInt32 RawValue;
};

struct _RTCharacterLevel {
    UInt8 Level;
    UInt64 Exp;
};

struct _RTCharacterHonor {
    Int8 Rank;
    Int64 Point;
    UInt64 Exp;
};

struct _RTCharacterAbility {
    UInt16 Point;
    UInt32 Exp;
};

struct _RTCharacterResource {
    Int32 DiffHP;
    Int32 DiffMP;
    Int32 DiffSP;
    Int32 DiffBP;
    Int32 DP;
};

struct _RTCharacterSkill {
    UInt8 Rank;
    UInt16 Level;
    UInt64 Exp;
    UInt16 Point;
};

struct _RTCharacterPosition {
    // TODO: Add DungeonID
    UInt8 WorldID;
    UInt16 X;
    UInt16 Y;
};

struct _RTCharacterInfo {
    union _RTCharacterStyle Style;
    struct _RTCharacterLevel Level[RUNTIME_CHARACTER_LEVEL_COUNT];
    struct _RTCharacterHonor Honor;
    struct _RTCharacterAbility Ability;
    struct _RTCharacterSkill Skill;
    struct _RTCharacterResource Resource;
    UInt16 Stat[RUNTIME_CHARACTER_STAT_COUNT];
    UInt64 Currency[RUNTIME_CHARACTER_CURRENCY_COUNT];
    struct _RTCharacterPosition Position;
};

struct _RTCharacterEquipmentInfo {
    UInt32 Count;
    struct _RTItemSlot Slots[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
};

struct _RTCharacterInventoryInfo {
    UInt32 Count;
    struct _RTItemSlot Slots[RUNTIME_INVENTORY_PAGE_SIZE * RUNTIME_INVENTORY_PAGE_COUNT];
};

struct _RTCharacterRecoveryInfo {
    UInt32 Count;
    UInt64 Prices[RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT];
    struct _RTItemSlot Slots[RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT];
};

struct _RTCharacterSkillSlotInfo {
    UInt32 Count;
    struct _RTSkillSlot Skills[RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT];
};

struct _RTCharacterQuickSlotInfo {
    UInt32 Count;
    struct _RTQuickSlot QuickSlots[RUNTIME_CHARACTER_MAX_QUICK_SLOT_COUNT];
};

struct _RTCharacterQuestSlotInfo {
    UInt32 Count;
    struct _RTQuestSlot QuestSlot[RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT];
};

struct _RTCharacterQuestFlagInfo {
    UInt8 Flags[RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT];
};

struct _RTCharacterEssenceAbilityInfo {
    Int32 Count;
    Int32 MaxCount;
    struct _RTEssenceAbilitySlot Slots[RUNTIME_CHARACTER_MAX_ESSENCE_ABILITY_SLOT_COUNT];
};

struct _RTCharacter {
    RTEntity ID;
    UInt32 CharacterIndex;
    UInt32 SyncMask;
    UInt32 SyncPriority;
    Timestamp SyncTimestamp;

//    Int32 Index;
//    Timestamp CreationDate;
//    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1];
    struct _RTCharacterInfo Info;
    struct _RTCharacterEquipmentInfo EquipmentInfo;
    struct _RTCharacterInventoryInfo InventoryInfo;
    struct _RTCharacterSkillSlotInfo SkillSlotInfo;
    struct _RTCharacterQuickSlotInfo QuickSlotInfo;
    struct _RTCharacterQuestSlotInfo QuestSlotInfo;
    struct _RTCharacterQuestFlagInfo QuestFlagInfo;
    
    //GAME_DATA_CHARACTER_ACHIEVEMENT AchievementData;
    struct _RTCharacterEssenceAbilityInfo EssenceAbilityInfo;
    /*
    GAME_DATA_CHARACTER_BLENDEDABILITY BlendedAbilityData;
    GAME_DATA_CHARACTER_HONORMEDAL HonorMedalData;
    GAME_DATA_CHARACTER_OVERLORD OverlordData;
    GAME_DATA_CHARACTER_TRANSFORM TransformData;
    GAME_DATA_CHARACTER_TRANSCENDENCE TranscendenceData;
    GAME_DATA_CHARACTER_MERCENARY MercenaryData;
    GAME_DATA_CHARACTER_CRAFT CraftData;
    */

    struct _RTCharacterInventoryInfo TemporaryInventoryInfo;
    struct _RTCharacterRecoveryInfo RecoveryInfo;

    struct _RTMovement Movement;
    struct _RTBattleAttributes Attributes;
    Int32 SkillComboLevel;
};

#pragma pack(pop)

Void RTCharacterInitialize(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTCharacterInfoRef Info,
    RTCharacterEquipmentInfoRef EquipmentInfo,
    RTCharacterInventoryInfoRef InventoryInfo,
    RTCharacterSkillSlotInfoRef SkillSlotInfo,
    RTCharacterQuickSlotInfoRef QuickSlotInfo,
    RTCharacterQuestSlotInfoRef QuestSlotInfo,
    RTCharacterQuestFlagInfoRef QuestFlagInfo
);

Void RTCharacterInitializeAttributes(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterIsAlive(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterIsUnmovable(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterMovementBegin(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 WorldID,
    Int32 PositionBeginX,
    Int32 PositionBeginY,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY,
    Int32 PositionEndX,
    Int32 PositionEndY
);

Bool RTCharacterMovementChange(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 WorldID,
    Int32 PositionBeginX,
    Int32 PositionBeginY,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY,
    Int32 PositionEndX,
    Int32 PositionEndY
);

Bool RTCharacterMovementEnd(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PositionX,
    Int32 PositionY
);

Bool RTCharacterMovementChangeWaypoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY,
    Int32 PositionNextX,
    Int32 PositionNextY
);

Bool RTCharacterMovementChangePosition(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY
);

Bool RTCharacterQuestBegin(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex
);

Bool RTCharacterQuestClear(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex,
    Int32 ItemIndex,
    Int32 InventorySlotIndex,
    UInt64* ResultExp,
    UInt32* ResultSkillExp
);

Bool RTCharacterQuestCancel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex
);

Bool RTCharacterQuestAction(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 NpcSetIndex,
    Int32 SlotIndex,
    Int32 ActionIndex,
    Int32 ActionSlotIndex
);

Bool RTCharacterIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 MobSpeciesID
);

Bool RTCharacterHasQuestItemCounter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTItem Item,
    UInt64 ItemOptions
);

Bool RTCharacterHasQuestDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 DungeonID
);

RTSkillSlotRef RTCharacterAddSkillSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillID,
    Int32 Level,
    Int32 SlotIndex
);

RTSkillSlotRef RTCharacterGetSkillSlotByIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
);

Void RTCharacterRemoveSkillSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillID,
    Int32 SlotIndex
);

Bool RTCharacterAddQuickSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillIndex,
    Int32 SlotIndex
);

RTQuickSlotRef RTCharacterGetQuickSlotByIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
);

Bool RTCharacterSwapQuickSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SourceSlotIndex,
    Int32 TargetSlotIndex
);

Bool RTCharacterRemoveQuickSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
);

Int32 RTCharacterCalculateRequiredMP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CoefficientA,
    Int32 CoefficientB,
    Int32 SkillLevel
);

Bool RTCharacterBattleRankUp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterAddCurrency(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CurrencyType,
    UInt64 Amount
);

Void RTCharacterAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt64 Exp
);

Int32 RTCharacterAddSkillExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 SkillExp
);

Void RTCharacterAddHonorPoint(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int64 HonorPoint
);

Void RTCharacterAddAbilityExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 Exp
);

Bool RTCharacterAddStats(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32* Stats
);

Void RTCharacterAddHP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 HP
);

Void RTCharacterAddMP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MP
);

Void RTCharacterAddSP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SP
);

Bool RTCharacterChangeSkillLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillID,
    Int32 SlotIndex,
    Int32 CurrentSkillLevel,
    Int32 TargetSkillLevel
);

Void RTCharacterApplyDamage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTEntity Source,
    Int32 Damage
);

EXTERN_C_END
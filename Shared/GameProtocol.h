#pragma once

#include <RuntimeLib/RuntimeLib.h>

#include "Constants.h"

// TODO: Remove GameProtocol and replace it with shared runtime data solution

EXTERN_C_BEGIN

#pragma pack(push, 1)

typedef struct {
    Int32 AccountID;
    Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
    Timestamp SessionTimeout;
    Int32 CharacterSlotID;
    UInt64 CharacterSlotOrder;
    UInt32 CharacterSlotFlags;
    Char CharacterPassword[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 CharacterQuestion;
    Char CharacterAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
} GAME_DATA_ACCOUNT;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_ACHIEVEMENT;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_BLENDEDABILITY;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_HONORMEDAL;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_OVERLORD;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_TRANSFORM;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_TRANSCENDENCE;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_MERCENARY;

typedef struct {
    UInt32 Count;
} GAME_DATA_CHARACTER_CRAFT;

typedef struct {
	Int32 ID;
	UInt64 CreationDate;
	UInt32 Style;
	Int32 Level;
	UInt16 OverlordLevel;
    UInt32 Unknown1[3];
	Int32 SkillRank;
	UInt8 NationMask;
	Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
	UInt64 HonorPoint;
	UInt64 Alz;
	UInt8 MapID;
	UInt16 PositionX;
	UInt16 PositionY;
	UInt16 EquipmentCount;
    struct _RTItemSlot Equipment[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
	struct _RTItemSlotAppearance EquipmentAppearance[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
} GAME_DATA_CHARACTER_INDEX;

typedef struct {
    Int32 ID;
    Int32 Index;
    UInt64 CreationDate;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    struct _RTCharacterInfo CharacterData;
    struct _RTCharacterEquipmentInfo EquipmentData;
    struct _RTCharacterInventoryInfo InventoryData;
    struct _RTCharacterSkillSlotInfo SkillSlotData;
    struct _RTCharacterQuickSlotInfo QuickSlotData;
    struct _RTCharacterQuestSlotInfo QuestSlotData;
    struct _RTCharacterQuestFlagInfo QuestFlagData;
    GAME_DATA_CHARACTER_ACHIEVEMENT AchievementData;
    struct _RTCharacterEssenceAbilityInfo EssenceAbilityData;
    GAME_DATA_CHARACTER_BLENDEDABILITY BlendedAbilityData;
    GAME_DATA_CHARACTER_HONORMEDAL HonorMedalData;
    GAME_DATA_CHARACTER_OVERLORD OverlordData;
    GAME_DATA_CHARACTER_TRANSFORM TransformData;
    GAME_DATA_CHARACTER_TRANSCENDENCE TranscendenceData;
    GAME_DATA_CHARACTER_MERCENARY MercenaryData;
    GAME_DATA_CHARACTER_CRAFT CraftData;
} GAME_DATA_CHARACTER;

#pragma pack(pop)

EXTERN_C_END
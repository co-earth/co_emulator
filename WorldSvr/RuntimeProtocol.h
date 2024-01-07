#pragma once

#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define RUNTIME_DATA_WORLD_SIZE					256
#define RUNTIME_DATA_WORLD_MAX_CHARACTER_COUNT	256
#define RUNTIME_DATA_WORLD_MAX_COUNT			128

#define RUNTIME_DATA_CHARACTER_EQUIPMENT_COUNT 5
#define RUNTIME_DATA_CHARACTER_INIT_SKILL_COUNT 16

typedef struct {
	Void* Context;
} RUNTIME_DATA_CHARACTER_LINK;

typedef struct {
	Int32 StyleIndex;
	Int32 WorldID;
	Int32 X;
	Int32 Y;
	Int32 HP;
	Int32 MP;
	Int32 DeltaHP;
	Int32 DeltaMP;
	Int32 STR;
	Int32 DEX;
	Int32 INT;

	struct {
		Int32 SlotIndex;
		UInt32 ItemID;
	} Equipment[RUNTIME_DATA_CHARACTER_EQUIPMENT_COUNT];

} RUNTIME_DATA_CHARACTER;

typedef struct {
	Int32 Count;
	RUNTIME_DATA_CHARACTER Characters[];
} RUNTIME_DATA_CHARACTER_INIT;

typedef struct {
	UInt16 SkillID;
	UInt8 SkillLevel;
	UInt16 SlotIndex;
} RUNTIME_DATA_CHARACTER_SKILL;

typedef struct {
	Int32 Count;
	
	struct {
		Int32 BattleStyleIndex;
		RUNTIME_DATA_CHARACTER_SKILL Skills[RUNTIME_DATA_CHARACTER_INIT_SKILL_COUNT];
	} Characters[];

} RUNTIME_DATA_CHARACTER_INIT_SKILL;

#pragma pack(pop)

EXTERN_C_END
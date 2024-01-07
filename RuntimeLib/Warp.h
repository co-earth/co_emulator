#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
	RUNTIME_WARP_TYPE_GATE = 0,
	RUNTIME_WARP_TYPE_QUEST_DUNGEON,
	RUNTIME_WARP_TYPE_DUNGEON,
};

struct _RTWarpIndex {
	Int32 Index;
	Int32 WorldID;
	struct _RTPosition Position;
	struct _RTPosition Target[RUNTIME_NATION_COUNT];
	Int32 Fee;
	Int32 Level;
};

struct _RTWarp {
	Int32 Index;
	Int32 Type;
	Int32 WorldID;
	Int32 NpcID;
	Int32 TargetID;
	Int32 Level;
	Int32 Fee;
	UInt32 WarpItemID;
	UInt32 WarpItemOption;
	Int32 QuestID;
};

#pragma pack(pop)

Bool RTRuntimeWarpCharacter(
	RTRuntimeRef Runtime,
	RTEntity Entity,
	Int32 NpcID,
	Int32 WarpIndex,
	Int32 SlotIndex
);

EXTERN_C_END
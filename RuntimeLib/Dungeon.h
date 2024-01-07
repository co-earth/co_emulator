#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"
#include "Mob.h"

EXTERN_C_BEGIN

enum {
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_SPAWN = 1,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_KILL = 2,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_REVIVE = 3,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_DELETE = 4,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_CHANGE = 5,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_EVENT_CALL = 6,
};

struct _RTDungeonTriggerData {
	Int32 DungeonID;
	Int32 TriggerID;
	Int32 TriggerType;
	Int32 TriggerNpcID;
	Int32 ActionGroupID;
	Int32 LiveMobCount;
	Int32 DeadMobCount;
	Int32 LiveMobs[RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT];
	Int32 DeadMobs[RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT];
};

struct _RTDungeonTriggerActionData {
	Int32 DungeonID;
	Int32 ActionGroupID;
	Int32 ActionType;
	Int32 TargetID;
	//Int32 SpawnInterval;
	//Int32 SpawnCount;
	Timestamp Delay;
};

struct _RTDungeonData {
	Int32 DungeonType;
	Int32 CreateType;
	Int32 EntryType;
	Int32 PlayerType;
	Int32 MaxPlayerCount;
	Int32 ObjectCount;
	RTItem EntryItem;
	Int32 RemoveItem;
	Int32 EntryConditionLevel;
	Int32 EntryConditionClass[2];
	Int32 EntryWarpID;
	Int32 FailWarpNpcID;
	Int32 DeadWarpID;
	Int32 DungeonID;
	Int32 PenaltyValue;
	Int32 PatternPartCount;
	Int32 PatternPartIndices[RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT];
	Int32 Reward[8];
	Int32 SuccessWarpNpcID;
	Int32 WorldID;
	Int32 NextDungeonID;
	Int32 WarpNpcSetID;
	Int32 UseOddCircleCount;
	Int32 UseVeradrixCount;
	Int32 IsElite;
	Int32 EliteDungeonBoost;
	Int32 MissionTimeout;
	Int32 TriggerCount;
	Int32 TriggerActionCount;
	struct _RTDungeonTriggerData TriggerData[RUNTIME_MEMORY_DUNGEON_TRIGGER_DATA_COUNT];
	struct _RTDungeonTriggerActionData TriggerActionData[RUNTIME_MEMORY_DUNGEON_TRIGGER_ACTION_DATA_COUNT];
};

struct _RTQuestUnitItemData {
	RTItem Item;
	Int32 Count;
	Int32 MaxCount;
};

struct _RTQuestUnitMobData {
	Int32 MobID;
	Int32 Count;
	Int32 MaxCount;
};

struct _RTQuestDungeonPatternPartData {
	Int32 ID;
	Int32 Type;
	Int32 CreateMethod;
	Int32 MissionNpcID;
	Int32 Mesh[2];
	Int32 MissionGate[2];
	Int32 MissionItemCount;
	Int32 MissionMobCount;
	struct _RTQuestUnitItemData MissionItems[RUNTIME_MAX_QUEST_COUNTER_COUNT];
	struct _RTQuestUnitMobData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];
};

struct _RTMissionDungeonPatternPartData {
	Int32 ID;
	Int32 MissionNpcID;
	Int32 MissionMobCount;
	struct _RTQuestUnitMobData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];
	struct _RTMobTable MobTable;
};

Bool RTDungeonStart(
	RTRuntimeRef Runtime,
	RTWorldRef World
);

Bool RTDungeonEnd(
	RTRuntimeRef Runtime,
	RTWorldRef World
);

Bool RTDungeonStartNextPatternPart(
	RTRuntimeRef Runtime,
	RTWorldRef World
);

Bool RTDungeonTriggerEvent(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	Int32 TriggerID
);

EXTERN_C_END
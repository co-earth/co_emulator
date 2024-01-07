#pragma once

#include "Base.h"
#include "BattleSystem.h"
#include "Movement.h"

EXTERN_C_BEGIN

enum {
	RUNTIME_MOB_AGGRESSIVE_TYPE_PASSIVE,
	RUNTIME_MOB_AGGRESSIVE_TYPE_AGGRESSIVE,
	RUNTIME_MOB_AGGRESSIVE_TYPE_NATIONONLY,
};

enum {
	RUNTIME_MOB_ATTACK_TYPE_CONCENTRATE,
	RUNTIME_MOB_ATTACK_TYPE_SELECT,
	RUNTIME_MOB_ATTACK_TYPE_REVENGE,
	RUNTIME_MOB_ATTACK_TYPE_NONE
};

enum {
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_NONE = 0,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_HP_TRIGGER = 1,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_SITUATIONAL = 2,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_BY_REACH = 3,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_SELECTIVE = 4,
};

enum {
	RUNTIME_MOB_PHASE_NONE,
	RUNTIME_MOB_PHASE_MOVE,
	RUNTIME_MOB_PHASE_FIND_PASSIVE,
	RUNTIME_MOB_PHASE_FIND_AGGRESSIVE,
	RUNTIME_MOB_PHASE_FIND_INFINITE,
	RUNTIME_MOB_PHASE_CHASE_CONCENTRATE,
	RUNTIME_MOB_PHASE_CHASE_SELECT,
	RUNTIME_MOB_PHASE_CHASE_REVENGE,
	RUNTIME_MOB_PHASE_ATTACK_CONCENTRATE,
	RUNTIME_MOB_PHASE_ATTACK_SELECT,
	RUNTIME_MOB_PHASE_ATTACK_REVENGE,

	RUNTIME_MOB_PHASE_DEAD,
	RUNTIME_MOB_PHASE_STUN,
	RUNTIME_MOB_PHASE_CHASE_DUNGEON,
	RUNTIME_MOB_PHASE_REQUEST_HELP,
	RUNTIME_MOB_PHASE_ESCAPE,

	RUNTIME_MOB_PHASE_SPAWN,
	RUNTIME_MOB_PHASE_KILL,

	RUNTIME_MOB_PHASE_COUNT
};

enum {
	RUNTIME_MOB_SUBPHASE_NEUTRAL,
	RUNTIME_MOB_SUBPHASE_AGGRESSIVE,

	RUNTIME_MOB_SUBPHASE_COUNT
};

struct _RTMobSkillData {
	Int32 PhysicalAttackMin;
	Int32 PhysicalAttackMax;
	Int32 Reach;
	Int32 Range;
	Int32 Stance;
	Int32 Distance;
	Int32 DistanceDecreased;
	Int32 Offset;
	Int32 SkillGroup;
	Bool IsDefenseSkill;
	Timestamp Interval;
};

struct _RTMobData {
	Int32 Index;
	Float32 MoveSpeed;
	Float32 ChaseSpeed;
	Float32 Radius;
	Int32 Property;
	Int32 AttackPattern;
	Int32 AggressiveType;
	Int32 Cooperate;
	Int32 Escape;
	Int32 AttackType;
	Int32 Scale;
	Int32 FindCount;
	Int32 FindInterval;
	Int32 MoveInterval;
	Int32 ChaseInterval;
	Int32 AlertRange;
	Int32 AdditionalAlertRange; // TODO: Add calculation for normal and quest dungeon!
	Int32 LimitRangeA;
	Int32 LimitRangeB;
	Int32 Level;
	Int32 HP;
	Int32 Defense;
	Int32 AttackRate;
	Int32 DefenseRate;
	Int32 HpRecharge;
	struct _RTMobSkillData DefaultSkill;
	struct _RTMobSkillData SpecialSkill;
	Int32 AttackSignal;
	Int32 Boss;
	Int32 CanAttack;
	Int32 CanMove;
	UInt64 Exp;
};

struct _RTMobPhaseData {
	Int32 Move;
	Int32 Find;
	Int32 Chase;
	Int32 Attack;
	Int32 Dead;
	Int32 Stun;
};

struct _RTMobSpawnData {
	Int32 PatternPartID;
	Int32 MobSpeciesID;
	Int32 AreaX;
	Int32 AreaY;
	Int32 AreaWidth;
	Int32 AreaHeight;
	Int32 SpawnInterval;
	Int32 SpawnCount;
	Int32 SpawnDefault;
	Int32 Grade;
	Int32 Level;
	Int32 MissionGate;
	Int32 PerfectDrop;
	Int32 SpawnTriggerID;
	Int32 KillTriggerID;
	Int32 Type;
	Int32 Min;
	Int32 Max;
	Int32 Authority;
	Int32 ServerMobID;
	Int32 LootDelay;
};

struct _RTMobAggroData {
	Int32 Count;
	RTEntity Entities[RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT];
	Int64 ReceivedDamage[RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT];
};

struct _RTMob {
	RTEntity ID;
	RTMobDataRef Data;
	RTEntity DropOwner;
	struct _RTMobPhaseData PhaseData;
	Int32 Phase;
	Int32 SubPhase;
	Bool IsSpawned;
	Bool IsKilled;
	Bool IsDead;
	Bool IsInfiniteSpawn;
	Bool IsPermanentDeath;
	Bool IsChasing;
	Bool IsWall;
	Bool IsAttacked;
	Int32 RemainingFindCount;
	Int32 RemainingSpawnCount;
	Timestamp SpawnTimestamp;
	Timestamp DespawnTimestamp;
	Timestamp EscapeTimestamp;
	Timestamp PhaseTimeout;
	Timestamp NextTimestamp;

	RTMobSkillDataRef ActiveSkill;
	RTDataMobAttackDataRef SpecialAttack;
	RTDataMobSkillRef SpecialAttackSkill;

	Timestamp NextPhaseTimestamp;
	Int32 HPTriggerThreshold;

	struct _RTMobSpawnData Spawn;
	struct _RTMobAggroData Aggro;
    struct _RTMovement Movement;
	struct _RTBattleAttributes Attributes;
};

struct _RTMobTable {
	Int32 Count;
	struct _RTMob Mobs[RUNTIME_MEMORY_MAX_MOB_COUNT];
};

Void RTMobInit(
	RTRuntimeRef Runtime,
	RTMobRef Mob
);

Void RTMobApplyDamage(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob,
	RTEntity Source,
	Int32 Damage
);

Void RTMobUpdate(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob
);


Bool RTMobCanRespawn(RTMobRef Mob);

Bool RTMobIsAlive(RTMobRef Mob);


EXTERN_C_END
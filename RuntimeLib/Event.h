#pragma once

#include "Base.h"
#include "Constants.h"
#include "BattleSystem.h"

EXTERN_C_BEGIN

enum {
    RUNTIME_EVENT_CHARACTER_SPAWN,
    RUNTIME_EVENT_CHARACTER_DESPAWN,
    RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE,
    RUNTIME_EVENT_CHARACTER_LEVEL_UP,
    RUNTIME_EVENT_CHARACTER_SKILL_LEVEL_UP,
    RUNTIME_EVENT_CHARACTER_SKILL_RANK_UP,
    RUNTIME_EVENT_CHARACTER_BATTLE_RANK_UP,
    RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS,
    RUNTIME_EVENT_MOB_SPAWN,
    RUNTIME_EVENT_MOB_UPDATE,
    RUNTIME_EVENT_MOB_DESPAWN,
    RUNTIME_EVENT_MOB_MOVEMENT_BEGIN,
    RUNTIME_EVENT_MOB_MOVEMENT_END,
    RUNTIME_EVENT_MOB_CHASE_BEGIN,
    RUNTIME_EVENT_MOB_CHASE_END,
    RUNTIME_EVENT_MOB_ATTACK,
    RUNTIME_EVENT_ITEM_SPAWN,
    RUNTIME_EVENT_ITEM_DESPAWN,
    RUNTIME_EVENT_QUEST_DUNGEON_PATTERN_PART_COMPLETED,
};

enum {
    RUNTIME_DELETE_TYPE_DEAD = 0x10,
    RUNTIME_DELETE_TYPE_WARP,
    RUNTIME_DELETE_TYPE_LOGOUT,
    RUNTIME_DELETE_TYPE_RETURN,
    RUNTIME_DELETE_TYPE_DISAPPEAR,
    RUNTIME_DELETE_TYPE_DEAD_UNKNOWN,
};

union _RTEventData {
    /*
    struct { } CharacterSpawn;
    struct { } CharacterDespawn;
    struct { } CharacterChunkUpdate;
    struct { } CharacterLevelUp;
    struct { } CharacterSkillLevelUp;
    struct { } CharacterSkillRankUp;
    struct { } CharacterBattleRankUp;
    struct { } CharacterUpdateSkillStatus;
    */

    struct {
        UInt32 TickCount;
        UInt16 PositionBeginX;
        UInt16 PositionBeginY;
        UInt16 PositionEndX;
        UInt16 PositionEndY;
        UInt16 MobSpeciesID;
        UInt64 MaxHP;
        UInt64 CurrentHP;
        UInt8 Level;
    } MobSpawnOrUpdate;

    /*
    struct { } MobDespawn;
    */
    struct {
        UInt32 TickCount;
        UInt16 PositionBeginX;
        UInt16 PositionBeginY;
        UInt16 PositionEndX;
        UInt16 PositionEndY;
    } MobMovementBegin;

    struct {
        UInt16 PositionCurrentX;
        UInt16 PositionCurrentY;
    } MobMovementEnd;

    struct {
        Bool IsDefaultSkill;
        Int32 ResultCount;
        UInt64 MobHP;

        struct {
            RTEntity Entity;
            Bool IsDead;
            UInt8 Result;
            UInt32 AppliedDamage;
            UInt64 TargetHP;
        } Results[RUNTIME_MOB_MAX_ATTACK_RESULT_COUNT];
    } MobAttack;

    struct {
        RTEntity Entity;
        UInt64 ItemOptions;
        UInt64 ItemID;
        UInt16 X;
        UInt16 Y;
        UInt16 UniqueKey;
    } ItemSpawn;

    /*
    struct { } ItemDespawn;
    struct { } QuestDungeonPatternPartCompleted;
    */
};

struct _RTEvent {
    Int32 Type;
    RTWorldRef World;
    RTEntity SourceID;
    RTEntity TargetID;
    Int32 X;
    Int32 Y;
    union _RTEventData Data;
};
typedef union _RTEventData RTEventData;

typedef Void (*RTEventCallback)(
    RTRuntimeRef Runtime,
    RTEventRef Event,
    Void* UserData
);

EXTERN_C_END
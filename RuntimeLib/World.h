#pragma once

#include "Base.h"
#include "Constants.h"
#include "Character.h"
#include "Dungeon.h"
#include "Drop.h"
#include "Mob.h"

EXTERN_C_BEGIN

enum {
    RUNTIME_WORLD_TYPE_GLOBAL,
    RUNTIME_WORLD_TYPE_DUNGEON,
    RUNTIME_WORLD_TYPE_QUEST_DUNGEON,
};

enum {
    RUNTIME_WORLD_TILE_NONE = 0b0000,
    RUNTIME_WORLD_TILE_WALL = 0b0001,
    RUNTIME_WORLD_TILE_TOWN = 0b0110,
};

enum {
    RUNTIME_WORLD_FLAGS_HAS_MAPCODE = 1 << 0,
    RUNTIME_WORLD_FLAGS_WAR_ALLOWED = 1 << 1,
    RUNTIME_WORLD_FLAGS_WAR_CONTROL = 1 << 2,
};

struct _RTWorldItem {
    RTEntity ID;
    UInt16 EntityKey;
    Int32 Index;
    RTItem Item;
    UInt64 ItemOptions;
    Int32 X;
    Int32 Y;
    Timestamp Timestamp;
};

struct _RTWorld {
    Int32 WorldIndex;
    Int32 Type;
    RTEntity Owner;
    Int32 DeadWarpIndex;
    Int32 ReturnWarpIndex;
    Int32 MapCodeIndex;
    UInt32 Flags;
    Int32 Seed;

    Int32 CharacterCount;
    RTEntity Characters[RUNTIME_MEMORY_MAX_CHARACTER_COUNT];

    struct _RTMobTable MobTable;

    Int32 ItemCount;
    struct _RTWorldItem Items[RUNTIME_MEMORY_MAX_ITEM_COUNT];
    struct _RTDropTable DropTable;

    Bool Active;
    Bool Closed;
    Int32 DungeonID;
    Int32 PatternPartIndex;
    Int32 MissionItemCount;
    Int32 MissionMobCount;
    struct _RTQuestUnitItemData MissionItems[RUNTIME_MAX_QUEST_COUNTER_COUNT];
    struct _RTQuestUnitMobData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];

    UInt32* Tiles;
};

Void RTWorldUpdate(
    RTRuntimeRef Runtime,
    RTWorldRef World
);

Void RTWorldSpawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
);

Void RTWorldDespawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
);

Void RTWorldSpawnMob(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob
);

RTMobRef RTWorldGetMobByID(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
);

RTMobRef RTWorldGetMob(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    UInt32 MobIndex
);

Void RTWorldDespawnMob(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob
);

RTWorldItemRef RTWorldSpawnItem(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Source,
    RTItem ItemID,
    UInt64 ItemOptions,
    Int32 X,
    Int32 Y
);

RTWorldItemRef RTWorldGetItem(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity,
    UInt32 EntityKey
);

RTWorldItemRef RTWorldGetItemByEntity(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
);

Void RTWorldDespawnItem(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTWorldItemRef Item
);

Bool RTWorldIsTileColliding(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    Int32 X,
    Int32 Y,
    UInt32 CollisionMask
);

Bool RTWorldTraceMovement(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    Int32 StartX,
    Int32 StartY,
    Int32 EndX,
    Int32 EndY,
    Int32* ResultX,
    Int32* ResultY,
    UInt32 CollisionMask,
    UInt32 IgnoreMask
);

Void RTWorldIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    UInt32 MobSpeciesID
);

Void RTWorldSetMobTable(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobTableRef MobTable
);

EXTERN_C_END
#include "Mob.h"
#include "World.h"
#include "Runtime.h"

Void RTWorldUpdate(
    RTRuntimeRef Runtime,
    RTWorldRef World
) {
    for (Int32 Index = 0; Index < World->MobTable.Count; Index++) {
        RTMobRef Mob = &World->MobTable.Mobs[Index];
        RTMobUpdate(Runtime, World, Mob);
    }
}

Void RTWorldSpawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);
    assert(World->CharacterCount < RUNTIME_MEMORY_MAX_CHARACTER_COUNT);

    World->Characters[World->CharacterCount] = Entity;
    World->CharacterCount += 1;

    RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Entity);
    assert(Character);

    RTRuntimeBroadcastEvent(
        Runtime,
        RUNTIME_EVENT_CHARACTER_SPAWN,
        World,
        Entity,
        kEntityNull,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y
    );

    RTRuntimeBroadcastEvent(
        Runtime,
        RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE,
        World,
        kEntityNull,
        Entity,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y
    );
}

Int32 _RTWorldGetCharacterIndex(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
) {
    Int32 CharacterIndex = -1;

    for (Int32 Index = 0; Index < World->CharacterCount; Index++) {
        if (Entity.EntityIndex == World->Characters[Index].EntityIndex) {
            CharacterIndex = Index;
            break;
        }
    }

    return CharacterIndex;
}

Void RTWorldDespawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    Int32 CharacterIndex = _RTWorldGetCharacterIndex(Runtime, World, Entity);
    assert(CharacterIndex >= 0);

    Int32 TailLength = World->CharacterCount - CharacterIndex - 1;
    if (TailLength > 0) {
        memmove(
            &World->Characters[CharacterIndex], 
            &World->Characters[CharacterIndex + 1], 
            TailLength * sizeof(RTEntity)
        );
    }

    World->CharacterCount -= 1;

    RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Entity);
    RTRuntimeBroadcastEvent(
        Runtime,
        RUNTIME_EVENT_CHARACTER_DESPAWN,
        World,
        kEntityNull,
        Character->ID,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y
    );
}

Void RTWorldSpawnMob(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob
) {
    assert(World->WorldIndex == Mob->ID.WorldIndex);

    Int32 X = RandomRange(&World->Seed, Mob->Spawn.AreaX, Mob->Spawn.AreaX + Mob->Spawn.AreaWidth);
    Int32 Y = RandomRange(&World->Seed, Mob->Spawn.AreaY, Mob->Spawn.AreaY + Mob->Spawn.AreaHeight);

    RTMovementInitialize(
        Runtime,
        &Mob->Movement,
        X,
        Y,
        Mob->Data->MoveSpeed,
        RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN
    );

    RTMobInit(Runtime, Mob);
    Mob->ActiveSkill = &Mob->Data->DefaultSkill;
    Mob->IsDead = false;
    Mob->IsSpawned = true;

    RTEventData EventData = { 0 };
    EventData.MobSpawnOrUpdate.Level = Mob->Data->Level;
    EventData.MobSpawnOrUpdate.CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    EventData.MobSpawnOrUpdate.MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    EventData.MobSpawnOrUpdate.MobSpeciesID = Mob->Spawn.MobSpeciesID;
    EventData.MobSpawnOrUpdate.PositionBeginX = Mob->Movement.PositionBegin.X;
    EventData.MobSpawnOrUpdate.PositionBeginY = Mob->Movement.PositionBegin.Y;
    EventData.MobSpawnOrUpdate.PositionEndX = Mob->Movement.PositionEnd.X;
    EventData.MobSpawnOrUpdate.PositionEndY = Mob->Movement.PositionEnd.Y;

    RTRuntimeBroadcastEventData(
        Runtime,
        RUNTIME_EVENT_MOB_SPAWN,
        World,
        kEntityNull,
        Mob->ID,
        Mob->Movement.PositionCurrent.X,
        Mob->Movement.PositionCurrent.Y,
        EventData
    );

    if (Mob->Spawn.SpawnTriggerID) {
        RTDungeonTriggerEvent(Runtime, World, Mob->Spawn.SpawnTriggerID);
    }
}

RTMobRef RTWorldGetMobByID(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
) {
    for (Int32 Index = 0; Index < World->MobTable.Count; Index += 1) {
        if (World->MobTable.Mobs[Index].ID.Serial == Entity.Serial) {
            return &World->MobTable.Mobs[Index];
        }
    }

    return NULL;
}

RTMobRef RTWorldGetMob(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    UInt32 MobIndex
) {
    for (Int32 Index = 0; Index < World->MobTable.Count; Index += 1) {
        if (World->MobTable.Mobs[Index].ID.EntityIndex == MobIndex) {
            return &World->MobTable.Mobs[Index];
        }
    }

    return NULL;
}

Void RTWorldDespawnMob(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobRef Mob
) {
    assert(World->WorldIndex == Mob->ID.WorldIndex);
    /*
    assert(Mob->State != RUNTIME_MOB_STATE_DEAD);
    assert(!Mob->IsDead);
    
    Mob->DespawnTimestamp = GetTickCount64();
    Mob->State = RUNTIME_MOB_STATE_DEAD;
    Mob->IsDead = true;
    */
    Mob->IsSpawned = false;
    Mob->IsDead = true;

    RTRuntimeBroadcastEvent(
        Runtime,
        RUNTIME_EVENT_MOB_DESPAWN,
        World,
        kEntityNull,
        Mob->ID,
        Mob->Movement.PositionCurrent.X,
        Mob->Movement.PositionCurrent.Y
    );

    // TODO: This should be evaluated inside the mob it self!
    if (Mob->DropOwner.Serial > 0) {
        RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Mob->DropOwner);
        if (Character) {
            Int32 Rate = RandomRange(&World->Seed, 0, 30000);
            if (Rate <= 10000) {
                UInt64 Amount = 10 * (100 + RandomRange(&World->Seed, 0, Mob->Data->Level)) / 100;
                RTItem Item;
                Item.ID = RUNTIME_ITEM_ID_CURRENCY;
                RTWorldSpawnItem(
                    Runtime,
                    World,
                    Mob->ID,
                    Item,
                    Amount,
                    Character->Movement.PositionCurrent.X,
                    Character->Movement.PositionCurrent.Y
                );
            }
            else if (Rate <= 20000) {
                struct _RTDropResult Result = { 0 };
                Bool Success = RTCalculateDrop(
                    Runtime,
                    World,
                    Mob,
                    Character,
                    &Result
                );

                if (Success) {
                    RTWorldSpawnItem(
                        Runtime,
                        World,
                        Mob->ID,
                        Result.ItemID,
                        Result.ItemOptions,
                        Character->Movement.PositionCurrent.X,
                        Character->Movement.PositionCurrent.Y
                    );
                }

                Success = RTCalculateQuestDrop(
                    Runtime,
                    World,
                    Mob,
                    Character,
                    &Result
                );

                if (Success) {
                    RTWorldSpawnItem(
                        Runtime,
                        World,
                        Mob->ID,
                        Result.ItemID,
                        Result.ItemOptions,
                        Character->Movement.PositionCurrent.X,
                        Character->Movement.PositionCurrent.Y
                    );
                }
            }
        }
    }

    if (Mob->Spawn.KillTriggerID) {
        RTDungeonTriggerEvent(Runtime, World, Mob->Spawn.KillTriggerID);
    }

    RTWorldIncrementQuestMobCounter(Runtime, World, Mob->Spawn.MobSpeciesID);
}

RTWorldItemRef RTWorldGetItem(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity,
    UInt32 EntityKey
) {
    for (Int32 Index = 0; Index < World->ItemCount; Index += 1) {
        if (World->Items[Index].ID.EntityIndex == Entity.EntityIndex &&
            World->Items[Index].EntityKey == EntityKey) {
            return &World->Items[Index];
        }
    }

    return NULL;
}

RTWorldItemRef RTWorldGetItemByEntity(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity Entity
) {
    assert(Entity.WorldIndex == World->WorldIndex);
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_ITEM);

    for (Int32 Index = 0; Index < World->ItemCount; Index += 1) {
        if (World->Items[Index].ID.EntityIndex == Entity.EntityIndex) {
            return &World->Items[Index];
        }
    }

    return NULL;
}

RTWorldItemRef RTWorldSpawnItem(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTEntity SourceID,
    RTItem ItemID,
    UInt64 ItemOptions,
    Int32 X,
    Int32 Y
) {
    if (World->ItemCount >= RUNTIME_MEMORY_MAX_ITEM_COUNT) {
        RTWorldDespawnItem(
            Runtime,
            World,
            &World->Items[0]
        );
    }

    RTWorldItemRef Item = &World->Items[World->ItemCount];
    memset(Item, 0, sizeof(struct _RTWorldItem));

    Item->ID = RTRuntimeCreateEntity(
        Runtime,
        World->WorldIndex,
        RUNTIME_ENTITY_TYPE_ITEM
    );
    Item->EntityKey = RandomRange(&World->Seed, 0, UINT16_MAX);
    Item->Index = World->ItemCount;
    Item->Item = ItemID;
    Item->ItemOptions = ItemOptions;
    Item->X = X;
    Item->Y = Y;
    Item->Timestamp = GetTickCount64();
    World->ItemCount += 1;

    RTEventData EventData = { 0 };
    EventData.ItemSpawn.Entity = Item->ID;
    EventData.ItemSpawn.ItemOptions = Item->ItemOptions;
    EventData.ItemSpawn.ItemID = Item->Item.ID;
    EventData.ItemSpawn.X = Item->X;
    EventData.ItemSpawn.Y = Item->Y;
    EventData.ItemSpawn.UniqueKey = Item->EntityKey;

    RTRuntimeBroadcastEventData(
        Runtime,
        RUNTIME_EVENT_ITEM_SPAWN,
        World,
        SourceID,
        Item->ID,
        Item->X,
        Item->Y,
        EventData
    );

    return Item;
}

Void RTWorldDespawnItem(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTWorldItemRef Item
) {
    assert(World->WorldIndex == Item->ID.WorldIndex);

    for (Int32 Index = 0; Index < World->ItemCount; Index++) {
        if (World->Items[Index].ID.EntityIndex == Item->ID.EntityIndex &&
            World->Items[Index].EntityKey == Item->EntityKey) {
            RTRuntimeBroadcastEvent(
                Runtime,
                RUNTIME_EVENT_ITEM_DESPAWN,
                World,
                kEntityNull,
                Item->ID,
                Item->X,
                Item->Y
            );

            Int32 TailLength = World->ItemCount - Index - 1;
            if (TailLength > 0) {
                memmove(&World->Items[Index], &World->Items[Index + 1], sizeof(struct _RTWorldItem) * TailLength);
            }

            World->ItemCount -= 1;
            break;
        }
    }
}

Bool RTWorldIsTileColliding(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    Int32 X,
    Int32 Y,
    UInt32 CollisionMask
) {
    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) {
        return true;
    }

    // TODO: Check if map orientation is correct!
    return (World->Tiles[X + Y * RUNTIME_WORLD_SIZE] & CollisionMask) > 0;
}

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
) {
    StartX <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    StartY <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    EndX <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    EndY <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;

    if (ResultX) *ResultX = StartX >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    if (ResultY) *ResultY = StartY >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;

    Int32 DeltaX = EndX - StartX;
    Int32 DeltaY = EndY - StartY;
    Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
    Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;
    Int32 AbsDelta[] = { AbsDeltaX, AbsDeltaY };
    if (AbsDeltaX < AbsDeltaY) {
        AbsDelta[0] = AbsDeltaY;
        AbsDelta[1] = AbsDeltaX;
    }

    Int32 Length = AbsDelta[0] + 1;
    Int32 D = (AbsDelta[1] << 1) - AbsDelta[0];
    Int32 SignX = (StartX > EndX) ? -1 : 1;
    Int32 SignY = (StartY > EndY) ? -1 : 1;
    Int32 StepD = (D < 0) ? (AbsDelta[1] << 1) : (AbsDelta[1] - AbsDelta[0]) << 1;
    Int32 StepX = (D < 0 && AbsDeltaX < AbsDeltaY) ? 0 : SignX;
    Int32 StepY = (D < 0 && AbsDeltaX >= AbsDeltaY) ? 0 : SignY;
    Int32 X = StartX;
    Int32 Y = StartY;

    for (Int32 Index = 0; Index < Length; Index++) {
        D += StepD;
        X += StepX;
        Y += StepY;

        Int32 TileX = X >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;
        Int32 TileY = Y >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;

        Bool Ignore = false;
        if (IgnoreMask) Ignore = RTWorldIsTileColliding(Runtime, World, TileX, TileY, IgnoreMask);

        if (!Ignore && RTWorldIsTileColliding(Runtime, World, TileX, TileY, CollisionMask)) {
            return false;
        }

        if (ResultX) *ResultX = TileX;
        if (ResultY) *ResultY = TileY;
    }

    return true;
}

Void RTWorldIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    UInt32 MobSpeciesID
) {
    if (World->Type != RUNTIME_WORLD_TYPE_DUNGEON && World->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return;

    for (Int32 Index = 0; Index < World->MissionMobCount; Index++) {
        if (World->MissionMobs[Index].MobID != MobSpeciesID) continue;

        World->MissionMobs[Index].Count = MIN(World->MissionMobs[Index].Count + 1, World->MissionMobs[Index].MaxCount);
    }

    RTDungeonStartNextPatternPart(Runtime, World);
}

Void RTWorldSetMobTable(
    RTRuntimeRef Runtime,
    RTWorldRef World,
    RTMobTableRef MobTable
) {
    for (Int32 Index = 0; Index < World->MobTable.Count; Index += 1) {
        RTMobRef Mob = &World->MobTable.Mobs[Index];
        if (!Mob->IsDead) {
            Mob->IsDead = true;

            RTRuntimeBroadcastEvent(
                Runtime,
                RUNTIME_EVENT_MOB_DESPAWN,
                World,
                kEntityNull,
                Mob->ID,
                Mob->Movement.PositionCurrent.X,
                Mob->Movement.PositionCurrent.Y
            );
        }
    }

    memcpy(&World->MobTable, MobTable, sizeof(struct _RTMobTable));

    for (Int32 Index = 0; Index < World->MobTable.Count; Index += 1) {
        World->MobTable.Mobs[Index].ID.WorldIndex = World->WorldIndex;
    }
}

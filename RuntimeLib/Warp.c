#include "Inventory.h"
#include "Runtime.h"
#include "Warp.h"

Bool RTRuntimeWarpCharacter(
    RTRuntimeRef Runtime,
    RTEntity Entity,
    Int32 NpcID,
    Int32 WarpIndex,
    Int32 SlotIndex
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Entity);
    assert(Character);

    if (!RTCharacterIsAlive(Runtime, Character)) return false;
    if (RTCharacterIsUnmovable(Runtime, Character)) return false;
    if ((Character->Movement.IsMoving) ||
        (Character->Movement.IsDeadReckoning)) {
        return false;
    }

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (!World) return false;

    if (RUNTIME_NPC_ID_RESERVED_BEGIN <= NpcID && NpcID <= RUNTIME_NPC_ID_RESERVED_END) {
        switch (NpcID) {
        case RUNTIME_NPC_ID_WAR_0: return false;
        case RUNTIME_NPC_ID_WAR_1: return false;
        case RUNTIME_NPC_ID_WAR_WARP: return false;
        case RUNTIME_NPC_ID_WAR_BATTLEFIELD: return false;
        case RUNTIME_NPC_ID_WAR_LOBBY: return false;
        case RUNTIME_NPC_ID_DEAD: return false;
        case RUNTIME_NPC_ID_RETURN: {
            // TODO: Check if character is in dungeon or war and return err

            RTItemSlotRef Slot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, SlotIndex);
            if (!Slot) return false;

            // TODO: Check return stone cooldown time!!!

            if (Slot->Item.Serial == RUNTIME_ITEM_SERIAL_WARP_CONSUMABLE) {
                if (Slot->ItemOptions > 0) {
                    Slot->ItemOptions -= 1;

                    if (Slot->ItemOptions < 1) {
                        RTInventoryClearSlot(Runtime, &Character->InventoryInfo, SlotIndex);
                    }
                }
                else {
                    return false;
                }
            }
            else if (Slot->Item.Serial == RUNTIME_ITEM_SERIAL_WARP_PERMANENT) {
                // TODO: Check if item is expired
            }
            else {
                return false;
            }

            RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[World->ReturnWarpIndex - 1];
            RTPositionRef Position = &WarpIndex->Target[0];
            RTWorldRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
            assert(NewWorld);

            RTWorldDespawnCharacter(Runtime, World, Entity);

            Character->Info.Position.X = Position->X;
            Character->Info.Position.Y = Position->Y;
            Character->Info.Position.WorldID = WarpIndex->WorldID;

            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                Position->X,
                Position->Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            RTWorldSpawnCharacter(Runtime, NewWorld, Entity);

            return true;
        }

        case RUNTIME_NPC_ID_NAVIGATION: return false;
        case RUNTIME_NPC_ID_UNKNOWN_1: return false;
        case RUNTIME_NPC_ID_UNKNOWN_2: return false;
        case RUNTIME_NPC_ID_QUEST_DUNGEON_WARP: return false;
        case RUNTIME_NPC_ID_QUEST_DUNGEON_EXIT: {
            if (World->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return false;

            RTDungeonDataRef QuestDungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);
            if (!QuestDungeonData) return false;

            assert(0 <= QuestDungeonData->FailWarpNpcID - 1 && QuestDungeonData->FailWarpNpcID - 1 < Runtime->WarpIndexCount);
            RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[QuestDungeonData->FailWarpNpcID - 1];
            RTPositionRef Position = &WarpIndex->Target[0];
            RTWorldRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
            assert(NewWorld);

            // TODO: Delete Dungeon Instance!
            RTWorldDespawnCharacter(Runtime, World, Entity);

            Character->Info.Position.X = Position->X;
            Character->Info.Position.Y = Position->Y;
            Character->Info.Position.WorldID = WarpIndex->WorldID;

            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                Position->X,
                Position->Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            RTWorldSpawnCharacter(Runtime, NewWorld, Entity);

            return true;
        }
        case RUNTIME_NPC_ID_UNKNOWN_3: return false;
        case RUNTIME_NPC_ID_PRISON: return false;
        case RUNTIME_NPC_ID_UNKNOWN_4: return false;
        case RUNTIME_NPC_ID_UNKNOWN_5: return false;
        case RUNTIME_NPC_ID_GM: return false;
        }
    }

    RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, Character->Info.Position.WorldID, NpcID);
    if (!Npc) return false;

    // TODO: Distance check if not working for quest dungeons from npcs...
    //if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) return false;

    RTWarpRef Warp = RTRuntimeGetWarpByWorldNpcID(Runtime, Character->Info.Position.WorldID, NpcID, WarpIndex);
    if (!Warp) return false;
    assert(Warp->Index == WarpIndex);

    if (Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level < Warp->Level) return false;
    if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < Warp->Fee) return false;

    if (Warp->Type == RUNTIME_WARP_TYPE_GATE) {
        assert(0 <= Warp->TargetID - 1 && Warp->TargetID - 1 < Runtime->WarpIndexCount);
        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[Warp->TargetID - 1];
        RTPositionRef Position = &WarpIndex->Target[0];
        RTWorldRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
        assert(NewWorld);

        RTWorldDespawnCharacter(Runtime, World, Entity);

        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Warp->Fee;
        Character->Info.Position.X = Position->X;
        Character->Info.Position.Y = Position->Y;
        Character->Info.Position.WorldID = WarpIndex->WorldID;

        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            Position->X,
            Position->Y,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacter(Runtime, NewWorld, Entity);

        return true;
    }
    else if (Warp->Type == RUNTIME_WARP_TYPE_QUEST_DUNGEON) {
        // TODO: Check quest dungeon flags of character if is already finished
        // TODO: Check if user is currently in quest dungeon and next quest dungeon index for success is matching

        if (!RTCharacterHasQuestDungeon(Runtime, Character, Warp->TargetID)) return false;

        RTDungeonDataRef QuestDungeonData = RTRuntimeGetDungeonDataByID(Runtime, Warp->TargetID);

        // TODO: Check and remove EntryItem

        if (Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level < QuestDungeonData->EntryConditionLevel) return false;

        // TODO: Check MaxPlayerCount & Party Size
        // TODO: Check EntryConditionClass

        assert(0 <= QuestDungeonData->EntryWarpID - 1 && QuestDungeonData->EntryWarpID - 1 < Runtime->WarpIndexCount);
        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[QuestDungeonData->EntryWarpID - 1];
        RTPositionRef Position = &WarpIndex->Target[0];
        RTWorldRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
        assert(NewWorld);

        RTWorldDespawnCharacter(Runtime, World, Entity);

        RTWorldRef DungeonWorld = RTRuntimeOpenDungeon(Runtime, Character, NewWorld, QuestDungeonData->DungeonID);
        if (!DungeonWorld) return false;

        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Warp->Fee;
        Character->Info.Position.X = Position->X;
        Character->Info.Position.Y = Position->Y;
        Character->Info.Position.WorldID = WarpIndex->WorldID;

        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            Position->X,
            Position->Y,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacter(Runtime, DungeonWorld, Entity);

        /*
        Int32 FailWarpNpcID;
        Int32 DeadWarpID;
        Int32 SuccessWarpNpcID;
        Int32 WarpNpcSetID;
        Int32 DungeonType;
        Int32 MissionTimeout;
        */
        Character->Info.Position.X = Position->X;
        Character->Info.Position.Y = Position->Y;
        Character->Info.Position.WorldID = QuestDungeonData->WorldID;
        return true;
    }
    else if (Warp->Type == RUNTIME_WARP_TYPE_DUNGEON) {
        // TODO: Check if user is currently in dungeon and next dungeon index for success is matching

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, Warp->TargetID);

        // TODO: Check and remove EntryItem

        if (Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level < DungeonData->EntryConditionLevel) return false;

        // TODO: Check MaxPlayerCount & Party Size
        // TODO: Check EntryConditionClass

        assert(0 <= DungeonData->EntryWarpID - 1 && DungeonData->EntryWarpID - 1 < Runtime->WarpIndexCount);
        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[DungeonData->EntryWarpID - 1];
        RTPositionRef Position = &WarpIndex->Target[0];
        RTWorldRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
        assert(NewWorld);

        RTWorldDespawnCharacter(Runtime, World, Entity);

        RTWorldRef DungeonWorld = RTRuntimeOpenDungeon(Runtime, Character, NewWorld, DungeonData->DungeonID);
        if (!DungeonWorld) return false;

        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Warp->Fee;
        Character->Info.Position.X = Position->X;
        Character->Info.Position.Y = Position->Y;
        Character->Info.Position.WorldID = WarpIndex->WorldID;

        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            Position->X,
            Position->Y,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacter(Runtime, DungeonWorld, Entity);

        /*
        Int32 FailWarpNpcID;
        Int32 DeadWarpID;
        Int32 SuccessWarpNpcID;
        Int32 WarpNpcSetID;
        Int32 DungeonType;
        Int32 MissionTimeout;
        */
        Character->Info.Position.X = Position->X;
        Character->Info.Position.Y = Position->Y;
        Character->Info.Position.WorldID = DungeonData->WorldID;
        return true;
    }

    return false;
}

#include "ClientProtocol.h"
#include "IPCProtocol.h"
#include "Notification.h"
#include "Server.h"

Void BroadcastUserList(
    ServerContextRef Server
) {
    IPC_DATA_WORLD_NFYUSERLIST* Notification = PacketInit(IPC_DATA_WORLD_NFYUSERLIST);
    Notification->Command = IPC_WORLD_NFYUSERLIST;
    Notification->PlayerCount = Server->ClientSocket->Connections.Count;
    Notification->MaxPlayerCount = Server->ClientSocket->MaxConnectionCount;
    SocketSendAll(Server->MasterSocket, Notification);
}

Void ServerBroadcastMobListToClient(
    ServerContextRef Server,
    ClientContextRef Client
) {
    RTRuntimeRef Runtime = Server->Runtime;
    RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Client->CharacterEntity);
    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

    Int32 ChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
    Int32 ChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;

    S2C_DATA_MOBS_SPAWN* Notification = PacketInit(S2C_DATA_MOBS_SPAWN);
    Notification->Command = S2C_MOBS_SPAWN;
    Notification->Count = 0;

    for (Int32 Index = 0; Index < World->MobTable.Count; Index += 1) {
        RTMobRef Mob = &World->MobTable.Mobs[Index];
        if (Mob->IsDead) continue;

        Int32 MobChunkX = Mob->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
        Int32 MobChunkY = Mob->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
        Int32 DeltaChunkX = abs(MobChunkX - ChunkX);
        Int32 DeltaChunkY = abs(MobChunkY - ChunkY);
        if (DeltaChunkX <= SERVER_CHARACTER_CHUNK_VIEW_RADIUS &&
            DeltaChunkY <= SERVER_CHARACTER_CHUNK_VIEW_RADIUS) {
            Notification->Count += 1;

            S2C_DATA_MOBS_SPAWN_INDEX* Spawn = PacketAppendStruct(S2C_DATA_MOBS_SPAWN_INDEX);
            Spawn->Entity = Mob->ID;
            Spawn->PositionBegin.X = Mob->Movement.PositionBegin.X;
            Spawn->PositionBegin.Y = Mob->Movement.PositionBegin.Y;
            Spawn->PositionEnd.X = Mob->Movement.PositionEnd.X;
            Spawn->PositionEnd.Y = Mob->Movement.PositionEnd.Y;
            Spawn->MobSpeciesID = Mob->Spawn.MobSpeciesID;
            Spawn->MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
            Spawn->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
            Spawn->Level = Mob->Data->Level;
        }
    }

    SocketSend(Server->ClientSocket, Client->Connection, Notification);
}

Void BroadcastToWorld(
    ServerContextRef Server,
    RTWorldRef World,
    RTEntity Entity,
    Int32 X,
    Int32 Y,
    PacketRef Packet
) {
    // NOTE: This is a fallback solution for now
    for (Int32 Index = 0; Index < World->CharacterCount; Index++) {
        RTEntity CharacterEntity = World->Characters[Index];
        if (Entity.Serial > 0 && Entity.Serial == CharacterEntity.Serial) continue;

        RTCharacterRef Character = RTRuntimeGetCharacter(Server->Runtime, CharacterEntity);
        RTMovementUpdateDeadReckoning(Server->Runtime, &Character->Movement);

        ClientContextRef Client = ServerGetClientByEntity(Server, CharacterEntity);
        if (Client) {
            SocketSend(Server->ClientSocket, Client->Connection, Packet);
        }
    }

    /* TODO: Add a correct chunking system into RTWorld
    
    Int32 ChunkX = (X >= 0) ? X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT : -1;
    Int32 ChunkY = (Y >= 0) ? Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT : -1;
    if (ChunkX < 0 || ChunkY < 0) return;

    for (Int32 Index = 0; Index < World->CharacterCount; Index++) {
        RTEntity CharacterEntity = World->Characters[Index];
        if (Entity.Serial > 0 && Entity.Serial == CharacterEntity.Serial) continue;

        RTCharacterRef Character = RTRuntimeGetCharacter(Server->Runtime, CharacterEntity);
        RTMovementUpdateDeadReckoning(Server->Runtime, &Character->Movement);

        Int32 CharacterChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
        Int32 CharacterChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
        Int32 MaxDistance = 2;
        Int32 DeltaX = ABS(ChunkX - CharacterChunkX);
        Int32 DeltaY = ABS(ChunkY - CharacterChunkY);

        if (DeltaX <= MaxDistance && DeltaY <= MaxDistance) {
            ClientContextRef Client = ServerGetClientByEntity(Server, CharacterEntity);
            if (Client) {
                SocketSend(Server->ClientSocket, Client->Connection, Packet);
            }
        }
    }
    */
}

Void ServerRuntimeOnEvent(
    RTRuntimeRef Runtime,
    RTEventRef Event,
    Void* UserData
) {
    ServerContextRef Server = (ServerContextRef)UserData;

    if (Event->Type == RUNTIME_EVENT_CHARACTER_SPAWN) {}

    if (Event->Type == RUNTIME_EVENT_CHARACTER_DESPAWN) {}

    if (Event->Type == RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE) {
        ClientContextRef Client = ServerGetClientByEntity(Server, Event->TargetID);
        if (Client) {
            ServerBroadcastMobListToClient(Server, Client);
        }
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_LEVEL_UP) {
        ClientContextRef Client = ServerGetClientByEntity(Server, Event->TargetID);
        if (!Client) return;

        RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Client->CharacterEntity);
        {
            S2C_DATA_NFY_CHARACTER_DATA* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_DATA);
            Notification->Command = S2C_NFY_CHARACTER_DATA;
            Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_LEVEL;
            Notification->Level = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level;
            SocketSend(Server->ClientSocket, Client->Connection, Notification);
        }

        S2C_DATA_NFY_CHARACTER_EVENT* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_EVENT);
        Notification->Command = S2C_NFY_CHARACTER_EVENT;
        Notification->Type = S2C_DATA_CHARACTER_EVENT_TYPE_LEVELUP;
        Notification->CharacterIndex = Client->CharacterIndex;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS) {
        ClientContextRef Client = ServerGetClientByEntity(Server, Event->TargetID);
        if (Client) {
            RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Client->CharacterEntity);

            Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
            Int32 SkillLevelMax = RTRuntimeDataCharacterRankUpConditionGet(
                &Runtime->Context,
                Character->Info.Skill.Rank,
                BattleStyleIndex
            );

            S2C_DATA_UPDATE_SKILL_STATUS* Notification = PacketInit(S2C_DATA_UPDATE_SKILL_STATUS);
            Notification->Command = S2C_UPDATE_SKILL_STATUS;
            Notification->SkillRank = Character->Info.Skill.Rank;
            Notification->SkillLevel = Character->Info.Skill.Level;
            Notification->SkillLevelMax = SkillLevelMax;
            Notification->SkillExp = Character->Info.Skill.Exp;
            Notification->SkillPoint = Character->Info.Skill.Point;
            SocketSend(Server->ClientSocket, Client->Connection, Notification);
        }

        return;
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_BATTLE_RANK_UP) {
        ClientContextRef Client = ServerGetClientByEntity(Server, Event->TargetID);
        if (!Client) return;

        RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Client->CharacterEntity);
        {
            S2C_DATA_BATTLE_RANK_UP* Notification = PacketInit(S2C_DATA_BATTLE_RANK_UP);
            Notification->Command = S2C_BATTLE_RANK_UP;
            Notification->Level = Character->Info.Style.BattleRank;
            SocketSend(Server->ClientSocket, Client->Connection, Notification);
        }

        S2C_DATA_NFY_CHARACTER_EVENT* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_EVENT);
        Notification->Command = S2C_NFY_CHARACTER_EVENT;
        Notification->Type = S2C_DATA_CHARACTER_EVENT_TYPE_RANKUP;
        Notification->CharacterIndex = Client->CharacterIndex;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_SPAWN || Event->Type == RUNTIME_EVENT_MOB_UPDATE) {
        S2C_DATA_MOBS_SPAWN* Notification = PacketInit(S2C_DATA_MOBS_SPAWN);
        Notification->Command = S2C_MOBS_SPAWN;
        Notification->Count = 1;

        S2C_DATA_MOBS_SPAWN_INDEX* Spawn = PacketAppendStruct(S2C_DATA_MOBS_SPAWN_INDEX);
        Spawn->Entity = Event->TargetID;
        Spawn->PositionBegin.X = Event->Data.MobSpawnOrUpdate.PositionBeginX;
        Spawn->PositionBegin.Y = Event->Data.MobSpawnOrUpdate.PositionBeginY;
        Spawn->PositionEnd.X = Event->Data.MobSpawnOrUpdate.PositionEndX;
        Spawn->PositionEnd.Y = Event->Data.MobSpawnOrUpdate.PositionEndY;
        Spawn->MobSpeciesID = Event->Data.MobSpawnOrUpdate.MobSpeciesID;
        Spawn->MaxHP = Event->Data.MobSpawnOrUpdate.MaxHP;
        Spawn->CurrentHP = Event->Data.MobSpawnOrUpdate.CurrentHP;
        Spawn->Level = Event->Data.MobSpawnOrUpdate.Level;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_DESPAWN) {
        S2C_DATA_MOBS_DESPAWN* Notification = PacketInit(S2C_DATA_MOBS_DESPAWN);
        Notification->Command = S2C_MOBS_DESPAWN;
        Notification->Entity = Event->TargetID;
        Notification->DespawnType = S2C_DATA_ENTITY_DESPAWN_TYPE_DEAD;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_MOVEMENT_BEGIN) {
        S2C_DATA_MOB_MOVEMENT_BEGIN* Notification = PacketInit(S2C_DATA_MOB_MOVEMENT_BEGIN);
        Notification->Command = S2C_MOB_MOVEMENT_BEGIN;
        Notification->Entity = Event->TargetID;
        Notification->TickCount = Event->Data.MobMovementBegin.TickCount;
        Notification->PositionBegin.X = Event->Data.MobMovementBegin.PositionBeginX;
        Notification->PositionBegin.Y = Event->Data.MobMovementBegin.PositionBeginY;
        Notification->PositionEnd.X = Event->Data.MobMovementBegin.PositionEndX;
        Notification->PositionEnd.Y = Event->Data.MobMovementBegin.PositionEndY;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_MOVEMENT_END) {
        S2C_DATA_MOB_MOVEMENT_END* Notification = PacketInit(S2C_DATA_MOB_MOVEMENT_END);
        Notification->Command = S2C_MOB_MOVEMENT_END;
        Notification->Entity = Event->TargetID;
        Notification->Position.X = Event->Data.MobMovementEnd.PositionCurrentX;
        Notification->Position.Y = Event->Data.MobMovementEnd.PositionCurrentY;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_CHASE_BEGIN) {
        S2C_DATA_MOB_CHASE_BEGIN* Notification = PacketInit(S2C_DATA_MOB_CHASE_BEGIN);
        Notification->Command = S2C_MOB_CHASE_BEGIN;
        Notification->Entity = Event->TargetID;
        Notification->TickCount = Event->Data.MobMovementBegin.TickCount;
        Notification->PositionBegin.X = Event->Data.MobMovementBegin.PositionBeginX;
        Notification->PositionBegin.Y = Event->Data.MobMovementBegin.PositionBeginY;
        Notification->PositionEnd.X = Event->Data.MobMovementBegin.PositionEndX;
        Notification->PositionEnd.Y = Event->Data.MobMovementBegin.PositionEndY;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_CHASE_END) {
        S2C_DATA_MOB_CHASE_END* Notification = PacketInit(S2C_DATA_MOB_CHASE_END);
        Notification->Command = S2C_MOB_CHASE_END;
        Notification->Entity = Event->TargetID;
        Notification->Position.X = Event->Data.MobMovementEnd.PositionCurrentX;
        Notification->Position.Y = Event->Data.MobMovementEnd.PositionCurrentY;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_ATTACK) {
        S2C_DATA_NFY_MOB_ATTACK_AOE* Notification = PacketInit(S2C_DATA_NFY_MOB_ATTACK_AOE);
        Notification->Command = S2C_NFY_MOB_ATTACK_AOE;
        Notification->Entity = Event->SourceID;
        Notification->IsDefaultSkill = Event->Data.MobAttack.IsDefaultSkill;
        Notification->MobHP = Event->Data.MobAttack.MobHP;
        Notification->TargetCount = Event->Data.MobAttack.ResultCount;

        for (Int32 Index = 0; Index < Event->Data.MobAttack.ResultCount; Index += 1) {
            S2C_DATA_MOB_ATTACK_TARGET* Target = PacketAppendStruct(S2C_DATA_MOB_ATTACK_TARGET);
            Target->CharacterIndex = RTRuntimeGetCharacterIndex(Runtime, Event->Data.MobAttack.Results[Index].Entity);
            Target->IsDead = Event->Data.MobAttack.Results[Index].IsDead;
            Target->Result = Event->Data.MobAttack.Results[Index].Result;
            Target->AppliedDamage = Event->Data.MobAttack.Results[Index].AppliedDamage;
            Target->TargetHP = Event->Data.MobAttack.Results[Index].TargetHP;

        }

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_ITEM_SPAWN) {
        S2C_DATA_NFY_SPAWN_ITEM* Notification = PacketInit(S2C_DATA_NFY_SPAWN_ITEM);
        Notification->Command = S2C_NFY_SPAWN_ITEM;
        Notification->Count = 1;

        UInt32 SourceIndex = Event->SourceID.Serial;
        if (Event->SourceID.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
            RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Event->SourceID);
            SourceIndex = Character->CharacterIndex;
        }

        S2C_DATA_NFY_SPAWN_ITEM_INDEX* Spawn = PacketAppendStruct(S2C_DATA_NFY_SPAWN_ITEM_INDEX);
        Spawn->Entity = Event->Data.ItemSpawn.Entity;
        Spawn->ItemOptions = Event->Data.ItemSpawn.ItemOptions;
        Spawn->SourceIndex = SourceIndex;
        Spawn->ItemID = Event->Data.ItemSpawn.ItemID;
        Spawn->X = Event->Data.ItemSpawn.X;
        Spawn->Y = Event->Data.ItemSpawn.Y;
        Spawn->UniqueKey = Event->Data.ItemSpawn.UniqueKey;
        Spawn->Type = 1;
        Spawn->Unknown4 = 52;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_ITEM_DESPAWN) {
        RTWorldItemRef Item = RTRuntimeGetItem(Runtime, Event->TargetID);
        assert(Item);

        S2C_DATA_ITEM_DESPAWN* Notification = PacketInit(S2C_DATA_ITEM_DESPAWN);
        Notification->Command = S2C_ITEM_DESPAWN;
        Notification->Entity = Item->ID;
        Notification->DespawnType = S2C_DATA_ENTITY_DESPAWN_TYPE_DISAPPEAR;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_QUEST_DUNGEON_PATTERN_PART_COMPLETED) {
        S2C_DATA_QUEST_DUNGEON_PATTERN_PART_COMPLETED* Notification = PacketInit(S2C_DATA_QUEST_DUNGEON_PATTERN_PART_COMPLETED);
        Notification->Command = S2C_QUEST_DUNGEON_PATTERN_PART_COMPLETED;
        Notification->PatternPartIndex = Event->World->PatternPartIndex;

        return BroadcastToWorld(
            Server,
            Event->World,
            kEntityNull,
            Event->X,
            Event->Y,
            Notification
        );
    }
}

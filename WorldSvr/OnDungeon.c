#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ENTER_DUNGEON_GATE) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

    S2C_DATA_ENTER_DUNGEON_GATE* Response = PacketInit(S2C_DATA_ENTER_DUNGEON_GATE);
    Response->Command = S2C_ENTER_DUNGEON_GATE;
    Response->Result = 0;
    Response->DungeonID = Packet->DungeonID;
    Response->Unknown1 = Packet->Unknown1;
    Response->NpcID = Packet->NpcID;
    Response->Unknown3 = Packet->Unknown3;
    Response->Unknown4 = Packet->Unknown4;
    Response->WorldID = Packet->WorldID;
    Response->Unknown5 = Packet->Unknown5;
    Response->DungeonBoostLevel = Packet->DungeonBoostLevel;

    RTWorldRef World = RTRuntimeGetWorldByID(Runtime, Packet->WorldID);
    if (!World) goto error;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, Packet->DungeonID);
    if (DungeonData) {
        Response->Result = 1;
    }

    /* TODO: Check on how to differentiate between quest dg and normal dg
    if (RTCharacterHasQuestDungeon(Runtime, Character, Packet->DungeonID) && DungeonData) {

        // TODO: Verify NpcID, WorldID
        Response->Result = 1;
    }
    */

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_START) {
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    S2C_DATA_QUEST_DUNGEON_START* Response = PacketInit(S2C_DATA_QUEST_DUNGEON_START);
    Response->Command = S2C_QUEST_DUNGEON_START;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_SPAWN) {
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);
    if (!DungeonData) goto error;

    RTDungeonStart(Runtime, World);
    RTWorldUpdate(Runtime, World);

    S2C_DATA_QUEST_DUNGEON_SPAWN* Response = PacketInit(S2C_DATA_QUEST_DUNGEON_SPAWN);
    Response->Command = S2C_QUEST_DUNGEON_SPAWN;

    // TODO: Character is joining the dungeon which doesn't mean that it is a start command,
    //       because a party member could have had already started it!

    Response->DungeonTimeout1 = DungeonData->MissionTimeout * 1000;
    Response->DungeonTimeout2 = DungeonData->MissionTimeout * 1000;
    SocketSend(Socket, Connection, Response);

    return;
    
error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_END) {
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    Bool Success = RTDungeonEnd(Runtime, World);

    if (Success) {
        S2C_DATA_NFY_DUNGEON_COMPLETE_INFO* Notification = PacketInit(S2C_DATA_NFY_DUNGEON_COMPLETE_INFO);
        Notification->Command = S2C_NFY_DUNGEON_COMPLETE_INFO;
        BroadcastToWorld(Server, World, kEntityNull, -1, -1, Notification);
    }

    S2C_DATA_QUEST_DUNGEON_END* Response = PacketInit(S2C_DATA_QUEST_DUNGEON_END);
    Response->Command = S2C_QUEST_DUNGEON_END;
    Response->Result = Success ? 1 : 0;
    SocketSend(Socket, Connection, Response);

    S2C_DATA_NFY_QUEST_DUNGEON_END* Notification = PacketInit(S2C_DATA_NFY_QUEST_DUNGEON_END);
    Notification->Command = S2C_NFY_QUEST_DUNGEON_END;
    Notification->CharacterIndex = Client->CharacterIndex;
    Notification->Result = Success ? 1 : 0;
    Notification->Unknown1 = 28;
    return BroadcastToWorld(Server, World, kEntityNull, -1, -1, Notification);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ATTACK_BOSS_MOB) {
    if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    assert(World);

    RTMobRef Mob = RTWorldGetMobByID(Runtime, World, Packet->Entity);
    if (!Mob) goto error;

    // TODO: Find out what this field means!
    if (Mob->Data->Boss == 2) {
        RTWorldDespawnMob(Runtime, World, Mob);
    }
    else {
        goto error;
    }

    // TODO: Implementation missing!
    // NOTE: It can be that when an npc is progressed but the dungeon is reopened it calls to repeat the event trigger of the npc and despawn it...

    S2C_DATA_ATTACK_BOSS_MOB* Response = PacketInit(S2C_DATA_ATTACK_BOSS_MOB);
    Response->Command = S2C_ATTACK_BOSS_MOB;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
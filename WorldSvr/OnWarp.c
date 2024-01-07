#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(WARP) {	
    if (!Character) goto error;

    if (!RTRuntimeWarpCharacter(Runtime, Client->CharacterEntity, Packet->NpcID, Packet->WarpIndex, Packet->SlotIndex)) goto error;

    Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
    Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    S2C_DATA_WARP* Response = PacketInit(S2C_DATA_WARP);
    Response->Command = S2C_WARP;
    Response->AccumulatedExp = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Exp;
    Response->AccumulatedOxp = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_OVERLORD].Exp;
    Response->Currency = Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
    Response->Position.X = Character->Info.Position.X;
    Response->Position.Y = Character->Info.Position.Y;
    Response->WorldID = Character->Info.Position.WorldID;
    return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
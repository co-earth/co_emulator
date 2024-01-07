#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ADD_CHARACTER_STATS) {
	if (!Character) goto error;

    if (Character->Info.Stat[RUNTIME_CHARACTER_STAT_STR] != Packet->Stats[RUNTIME_CHARACTER_STAT_STR] ||
        Character->Info.Stat[RUNTIME_CHARACTER_STAT_DEX] != Packet->Stats[RUNTIME_CHARACTER_STAT_DEX] ||
        Character->Info.Stat[RUNTIME_CHARACTER_STAT_INT] != Packet->Stats[RUNTIME_CHARACTER_STAT_INT]) {
        goto error;
    }

    Bool Success = RTCharacterAddStats(Runtime, Character, Packet->StatsAddition);
    
    S2C_DATA_ADD_CHARACTER_STATS* Response = PacketInit(S2C_DATA_ADD_CHARACTER_STATS);
    Response->Command = S2C_ADD_CHARACTER_STATS;
    Response->Result = Success ? 0 : 1;
    Response->Stats[RUNTIME_CHARACTER_STAT_STR] = Character->Info.Stat[RUNTIME_CHARACTER_STAT_STR];
    Response->Stats[RUNTIME_CHARACTER_STAT_DEX] = Character->Info.Stat[RUNTIME_CHARACTER_STAT_DEX];
    Response->Stats[RUNTIME_CHARACTER_STAT_INT] = Character->Info.Stat[RUNTIME_CHARACTER_STAT_INT];
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

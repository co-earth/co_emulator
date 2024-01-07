#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_DUNGEON_REWARD_LIST) {
	if (!Character) goto error;
	
	// TODO: Add reward list data

	S2C_DATA_GET_DUNGEON_REWARD_LIST* Response = PacketInit(S2C_DATA_GET_DUNGEON_REWARD_LIST);
	Response->Command = S2C_GET_DUNGEON_REWARD_LIST;
	Response->DungeonID = Packet->DungeonID;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

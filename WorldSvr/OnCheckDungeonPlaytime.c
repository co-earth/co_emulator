#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHECK_DUNGEON_PLAYTIME) {
	if (!Character) goto error;
	
	// TODO: Add xml data



	S2C_DATA_CHECK_DUNGEON_PLAYTIME* Response = PacketInit(S2C_DATA_CHECK_DUNGEON_PLAYTIME);
	Response->Command = S2C_CHECK_DUNGEON_PLAYTIME;
	Response->DungeonID = Packet->DungeonID;
	Response->MaxInstanceCount = 10;
	Response->InstanceCount = 0;
	Response->Unknown1[0] = 0x01;
	Response->RemainingPlayTimeInSeconds = 21600;
	Response->MaxEntryCount = 99;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

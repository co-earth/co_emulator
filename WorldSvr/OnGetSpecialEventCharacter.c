#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SPECIAL_EVENT_CHARACTER) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	S2C_DATA_GET_SPECIAL_EVENT_CHARACTER* Response = PacketInit(S2C_DATA_GET_SPECIAL_EVENT_CHARACTER);
	Response->Command = S2C_GET_SPECIAL_EVENT_CHARACTER;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
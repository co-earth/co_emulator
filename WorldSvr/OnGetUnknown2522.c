#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_UNKNOWN_2522) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	S2C_DATA_GET_UNKNOWN_2522* Response = PacketInit(S2C_DATA_GET_UNKNOWN_2522);
	Response->Command = S2C_GET_UNKNOWN_2522;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
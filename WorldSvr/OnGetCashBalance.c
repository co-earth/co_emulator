#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CASH_BALANCE) {
    if (!Character) goto error;

    S2C_DATA_GET_CASH_BALANCE* Response = PacketInit(S2C_DATA_GET_CASH_BALANCE);
    Response->Command = S2C_GET_CASH_BALANCE;
    Response->Amount = 999;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

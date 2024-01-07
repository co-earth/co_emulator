#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_PREMIUM_SERVICE) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;

	IPC_DATA_WORLD_REQPREMIUMSERVICE* Request = PacketInit(IPC_DATA_WORLD_REQPREMIUMSERVICE);
	Request->Command = IPC_WORLD_REQPREMIUMSERVICE;
	Request->ConnectionID = Connection->ID;
	Request->AccountID = Client->Account.AccountID;
	return SocketSendAll(Server->MasterSocket, Request);

error:
	return SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(OnWorldGetPremiumService, IPC_WORLD_ACKPREMIUMSERVICE, IPC_DATA_WORLD_ACKPREMIUMSERVICE) {
	if (!ClientConnection || !Client) goto error;

    S2C_DATA_GET_PREMIUM_SERVICE* Response = PacketInit(S2C_DATA_GET_PREMIUM_SERVICE);
    Response->Command = S2C_GET_PREMIUM_SERVICE;
    Response->ServiceType = Packet->ServiceType;
    Response->ExpirationDate = Packet->ExpiredAt;
    return SocketSend(Server->ClientSocket, ClientConnection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
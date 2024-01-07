#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldVerifyResult, IPC_WORLD_VERIFYRESULT, IPC_DATA_WORLD_VERIFYRESULT) {
	IPC_DATA_AUTH_VERIFYRESULT* Response = PacketInit(IPC_DATA_AUTH_VERIFYRESULT);
	Response->Command = IPC_AUTH_VERIFYRESULT;
	Response->ConnectionID = Packet->ConnectionID;
	Response->ServerID = Packet->ServerID;
	Response->WorldID = Packet->WorldID;
	Response->Status = Packet->Status;
	return SocketSendAll(Server->AuthSocket, Response);

error:
	Response->Status = 0;
	return SocketSendAll(Server->AuthSocket, Response);
}
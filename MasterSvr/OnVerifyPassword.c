#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldVerifyPassword, IPC_WORLD_REQVERIFYPASSWORD, IPC_DATA_WORLD_REQVERIFYPASSWORD) {
    IPC_DATA_AUTH_REQVERIFYPASSWORD* Request = PacketInit(IPC_DATA_AUTH_REQVERIFYPASSWORD);
	Request->Command = IPC_AUTH_REQVERIFYPASSWORD;
	Request->ConnectionID = Packet->ConnectionID;
	Request->AccountID = Packet->AccountID;
	memcpy(Request->Credentials, Packet->Credentials, MAX_PASSWORD_LENGTH);
	return SocketSendAll(Server->AuthSocket, Request);
}

IPC_PROCEDURE_BINDING(OnAuthVerifyPassword, IPC_AUTH_ACKVERIFYPASSWORD, IPC_DATA_AUTH_ACKVERIFYPASSWORD) {
	IPC_DATA_WORLD_ACKVERIFYPASSWORD* Response = PacketInit(IPC_DATA_WORLD_ACKVERIFYPASSWORD);
	Response->Command = IPC_WORLD_ACKVERIFYPASSWORD;
	Response->ConnectionID = Packet->ConnectionID;
	Response->Success = Packet->Success;
	return SocketSendAll(Server->WorldSocket, Response);
}
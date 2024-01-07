#include "IPCProtocol.h"
#include "IPCProcs.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(OnMasterConnect, IPC_AUTH_REQCONNECT, IPC_DATA_AUTH_REQCONNECT) {
	IPC_DATA_AUTH_ACKCONNECT* Response = PacketInit(IPC_DATA_AUTH_ACKCONNECT);
	Response->Command = IPC_AUTH_ACKCONNECT;
	Response->ConnectionID = 0;
	Response->Success = ServerGetMaster(Server, Packet->ServerID) == NULL;
	Master->ServerID = Packet->ServerID;
	SocketSend(Socket, Connection, Response);

	if (!Response->Success) {
		SocketDisconnect(Socket, Connection);
	}
}
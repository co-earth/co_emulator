#include "Server.h"
#include "IPCProcs.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldConnect, IPC_WORLD_REQCONNECT, IPC_DATA_WORLD_REQCONNECT) {
	if (ServerGetWorld(Server, Packet->WorldID) != NULL) {
		IPC_DATA_WORLD_ACKCONNECT* Response = PacketInit(IPC_DATA_WORLD_ACKCONNECT);
		Response->Command = IPC_WORLD_ACKCONNECT;
		Response->ServerID = Server->Config->MasterSvr.ServerID;
		Response->Success = false;
		SocketSend(Socket, Connection, Response);
		return SocketDisconnect(Socket, Connection);
	}

	Client->World.WorldID = Packet->WorldID;
	memcpy(Client->World.WorldHost, Packet->WorldHost, sizeof(Client->World.WorldHost));
	Client->World.WorldPort = Packet->WorldPort;
	Client->World.WorldType = Packet->WorldType;
	Client->World.PlayerCount = 0;
	Client->World.MaxPlayerCount = Packet->MaxPlayerCount;

	Client->Flags |= CLIENT_FLAGS_WORLD_INITIALIZED;

	IPC_DATA_WORLD_ACKCONNECT* Response = PacketInit(IPC_DATA_WORLD_ACKCONNECT);
	Response->Command = IPC_WORLD_ACKCONNECT;
	Response->ServerID = Server->Config->MasterSvr.ServerID;
	Response->Success = true;
	SocketSend(Socket, Connection, Response);

	Server->WorldListUpdateTimestamp = Server->Timestamp;
	BroadcastWorldList(Server);
}
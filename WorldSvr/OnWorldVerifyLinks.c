#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(OnWorldVerifyLinks, IPC_WORLD_VERIFYLINKS, IPC_DATA_WORLD_VERIFYLINKS) {
	IPC_DATA_WORLD_VERIFYRESULT* Response = PacketInit(IPC_DATA_WORLD_VERIFYRESULT);
	Response->Command = IPC_WORLD_VERIFYRESULT;
	Response->ConnectionID = Packet->ConnectionID;
	Response->ServerID = Packet->ServerID;
	Response->WorldID = Packet->WorldID;
	Response->AccountID = Packet->Account.AccountID;
	Response->Status = 0;

	if (Master->ServerID != Packet->ServerID ||
		Server->Config->WorldSvr.WorldID != Packet->WorldID) {
		return SocketSend(Socket, Connection, Response);
	}

	Client = ServerGetClientByAuthKey(Server, Packet->AuthKey, Packet->EntityID);
	if (!Client || Client->Flags & CLIENT_FLAGS_VERIFIED) {
		return SocketSend(Socket, Connection, Response);
	}

	if (memcmp(Connection->AddressIP, Packet->SessionIP, MAX_ADDRESSIP_LENGTH) != 0) {
		return SocketSend(Socket, Connection, Response);
	}

	// TODO: Check IP whitelist for WorldType == WORLD_TYPE_RESTRICTED
	// TODO: Check War status for WorldType == WORLD_TYPE_WAR_...
	// TODO: Store premium service info in client context

	memcpy(&Client->Account, &Packet->Account, sizeof(GAME_DATA_ACCOUNT));
	Client->Flags |= CLIENT_FLAGS_VERIFIED;

	Response->Status = 1;
	return SocketSend(Socket, Connection, Response);
}
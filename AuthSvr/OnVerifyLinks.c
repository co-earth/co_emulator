#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcs.h"

CLIENT_PROCEDURE_BINDING(OnVerifyLinks, C2S_VERIFYLINKS, C2S_DATA_VERIFYLINKS) {
    if (!(Client->Flags & CLIENT_FLAGS_AUTHENTICATED)) goto error;

    if (Client->Flags & CLIENT_FLAGS_VERIFIED) goto error;

    LogMessageFormat(LOG_LEVEL_INFO, "Client Magic Key: %d", Packet->ClientMagicKey);

    if (Server->Config->Auth.ClientMagicKey != Packet->ClientMagicKey) goto error;

    MasterContextWorldRef World = ServerGetWorld(Server, Packet->ServerID, Packet->WorldID);
    if (!World) goto error;

    Bool IsWorldFull = (World->PlayerCount >= World->MaxPlayerCount);
    if (IsWorldFull) goto error;

    assert(Client->AccountID > 0);

    Client->Flags |= CLIENT_FLAGS_VERIFIED;

    // TODO: Add account premium service info
    // TODO: Add client ip address

    IPC_DATA_AUTH_VERIFYLINKS* Request = PacketInit(IPC_DATA_AUTH_VERIFYLINKS);
    Request->Command = IPC_AUTH_VERIFYLINKS;
    Request->ConnectionID = Connection->ID;
    Request->ServerID = Packet->ServerID;
    Request->WorldID = Packet->WorldID;
    Request->AccountID = Client->AccountID;
    Request->AuthKey = Packet->AuthKey;
    Request->EntityID = Packet->EntityID;
    memcpy(Request->SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
    SocketSendAll(Server->MasterSocket, Request);

    return;
    
error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(OnMasterVerifyLinks, IPC_AUTH_VERIFYLINKS, IPC_DATA_AUTH_VERIFYLINKS) {

}

IPC_PROCEDURE_BINDING(OnMasterVerifyResult, IPC_AUTH_VERIFYRESULT, IPC_DATA_AUTH_VERIFYRESULT) {
	SocketConnectionRef ClientConnection = SocketGetConnection(Server->ClientSocket, Packet->ConnectionID);
	if (!ClientConnection) return;

    // TODO: Check already logged in here?

	S2C_DATA_VERIFYLINKS* Response = PacketInit(S2C_DATA_VERIFYLINKS);
	Response->Command = S2C_VERIFYLINKS;
	Response->WorldID = Packet->WorldID;
	Response->ServerID = Packet->ServerID;
    Response->Status = Packet->Status;
	SocketSend(Server->ClientSocket, ClientConnection, Response);
	SocketDisconnect(Server->ClientSocket, ClientConnection);
}
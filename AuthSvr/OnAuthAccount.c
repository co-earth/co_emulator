#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "Enumerations.h"
#include "Server.h"

Void StartDisconnectTimer(
    ServerContextRef Server,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection,
    UInt64 Timeout
) {
    S2C_DATA_DCTIMER* Response = PacketInit(S2C_DATA_DCTIMER);
    Response->Command = S2C_DCTIMER;
    Response->Timeout = Timeout;
    Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
    Client->DisconnectTimestamp = Server->Timestamp + Timeout;
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(OnAuthAccount, C2S_AUTHACCOUNT, C2S_DATA_AUTHACCOUNT) {
    if (!(Client->Flags & CLIENT_FLAGS_PUBLICKEY_INITIALIZED)) {
        return SocketDisconnect(Socket, Connection);
    }

    UInt32 ServerStatus = SERVER_STATUS_ONLINE;
    if (Server->Config->Auth.Maintenance) {
        ServerStatus = SERVER_STATUS_MAINTENANCE;
    }
    
    if (ServerStatus == SERVER_STATUS_ONLINE) {
        Client->Flags |= CLIENT_FLAGS_AUTHORIZED;
    }

    S2C_DATA_AUTHACCOUNT* Response = PacketInit(S2C_DATA_AUTHACCOUNT);
    Response->Command = S2C_AUTHACCOUNT;
    Response->ServerStatus = ServerStatus;
    SocketSend(Socket, Connection, Response);

    if (ServerStatus == SERVER_STATUS_ONLINE) {
        if (Server->Config->Auth.AutoDisconnect) {
            StartDisconnectTimer(
                Server,
                Socket,
                Client,
                Connection,
                Server->Config->Auth.AutoDisconnectDelay
            );
        }
    } else {
        SocketDisconnect(Socket, Connection);
    }
}
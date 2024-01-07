#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(OnCheckVersion, C2S_CHECKVERSION, C2S_DATA_CHECKVERSION) {
    if (!(Client->Flags & CLIENT_FLAGS_CONNECTED)) {
        return SocketDisconnect(Socket, Connection);
    }

    S2C_DATA_CHECKVERSION* Response = PacketInit(S2C_DATA_CHECKVERSION);
    Response->Command = S2C_CHECKVERSION;

    LogMessageFormat(LOG_LEVEL_INFO, "Client Version: %d", Packet->ClientVersion);

    if (Server->Config->Auth.CheckVersion) {
        Response->ClientVersion = Server->Config->Auth.ClientVersion;
        Response->ServerMagicKey = Server->Config->Auth.ServerMagicKey;
        
        if (Response->ClientVersion != Server->Config->Auth.ClientVersion) {
            return SocketDisconnect(Socket, Connection);
        }
    }
    else {
        Response->ClientVersion = Packet->ClientVersion;
        Response->ServerMagicKey = Server->Config->Auth.ServerMagicKey;
    }

    Client->ClientVersion = Packet->ClientVersion;
    Client->Flags |= CLIENT_FLAGS_VERSION_CHECKED;
    SocketSend(Socket, Connection, Response);
}
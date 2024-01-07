#include "AuthSocket.h"
#include "WorldSocket.h"
#include "MasterDB.h"
#include "Server.h"
#include "Notification.h"

Void ServerRun(
	ServerContextRef Server,
	CString ConfigFilePath
) {
    LogMessage(LOG_LEVEL_INFO, "Loading configuration...");
    
    if (!LoadConfig(Server->Config, ConfigFilePath)) {
        FatalError("Loading configuration failed!");
    }

    Char Directory[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, Directory);

    SocketInitPlainServer(
        Server->AuthSocket,
        1,
        &AuthSocketOnConnect,
        &AuthSocketOnDisconnect,
        NULL,
        &AuthSocketOnReceived,
        Server
    );

    SocketInitPlainServer(
        Server->WorldSocket,
        1,
        &WorldSocketOnConnect,
        &WorldSocketOnDisconnect,
        NULL,
        &WorldSocketOnReceived,
        Server
    );

    LogMessageFormat(
        LOG_LEVEL_INFO, 
        "Connecting to master database...\n\n\thost = %s\n\tusername = %s\n\tpassword = ****\n\tdatabase = %s\n\tport = %d\n",
        Server->Config->MasterDB.Host,
        Server->Config->MasterDB.Username,
        Server->Config->MasterDB.Database,
        Server->Config->MasterDB.Port
    );
    
    Server->Database = DatabaseConnect(
        Server->Config->MasterDB.Host,
        Server->Config->MasterDB.Username,
        Server->Config->MasterDB.Password,
        Server->Config->MasterDB.Database,
        Server->Config->MasterDB.Port
    );
    
    if (!Server->Database) {
        FatalError("Database connection failed");
    }

    MasterDBPrepareStatements(Server->Database);

    SocketListen(
        Server->WorldSocket,
        Server->Config->MasterSvr.Port
    );

    srand(GetTimestamp());

    while (true) {
        Server->Timestamp = GetTimestamp();

        if (!(Server->Flags & SERVER_FLAGS_AUTH_CONNECTED) &&
            !(Server->AuthSocket->Flags & SOCKET_FLAGS_CONNECTING)) {
            SocketConnect(
                Server->AuthSocket,
                Server->Config->AuthSvr.Host,
                Server->Config->AuthSvr.Port,
                0
            );
        }

        SocketUpdate(Server->AuthSocket);
        SocketUpdate(Server->WorldSocket);

        if (Server->WorldListBroadcastTimestamp < Server->WorldListUpdateTimestamp &&
            (Server->Flags & SERVER_FLAGS_AUTH_CONNECTED)) {
            Server->WorldListBroadcastTimestamp = Server->Timestamp;
            Server->WorldListUpdateTimestamp = Server->WorldListBroadcastTimestamp;
            BroadcastWorldList(Server);
        }
    }
}

ClientContextRef ServerGetWorld(
    ServerContextRef Server,
    Int32 WorldID
) {
    for (Int32 Index = 0; Index < Server->WorldSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Server->WorldSocket->Connections,
            Index
        );
        ClientContextRef Client = (ClientContextRef)Connection->Userdata;

        if ((Client->Flags & CLIENT_FLAGS_WORLD_INITIALIZED) &&
            Client->World.WorldID == WorldID) {
            return Client;
        }
    }

    return NULL;
}

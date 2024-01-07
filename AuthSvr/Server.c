#include "AuthDB.h"
#include "ClientSocket.h"
#include "MasterSocket.h"
#include "Notification.h"
#include "Server.h"

Void ServerRun(
	ServerContextRef Server,
	CString ConfigFilePath
) {
    LogMessage(LOG_LEVEL_INFO, "Loading configuration...");
    
    if (!LoadConfig(Server->Config, ConfigFilePath)) {
        FatalError("Loading configuration failed!");
    }
    
    SocketInitEncryptedServer(
        Server->ClientSocket,
        Server->Config->Auth.MaxConnectionCount,
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect,
        &ClientSocketOnSend,
        &ClientSocketOnReceived,
        Server
    );

    SocketInitPlainServer(
        Server->MasterSocket,
        MAX_SERVER_COUNT,
        &MasterSocketOnConnect,
        &MasterSocketOnDisconnect,
        NULL,
        &MasterSocketOnReceived,
        Server
    );

    LogMessageFormat(
        LOG_LEVEL_INFO, 
        "Connecting to auth database...\n\n\thost = %s\n\tusername = %s\n\tpassword = ****\n\tdatabase = %s\n\tport = %d\n",
        Server->Config->AuthDB.Host,
        Server->Config->AuthDB.Username,
        Server->Config->AuthDB.Database,
        Server->Config->AuthDB.Port
    );
    
    Server->Database = DatabaseConnect(
        Server->Config->AuthDB.Host,
        Server->Config->AuthDB.Username,
        Server->Config->AuthDB.Password,
        Server->Config->AuthDB.Database,
        Server->Config->AuthDB.Port
    );
    
    if (!Server->Database) {
        FatalError("Database connection failed");
    }

    AuthDBPrepareStatements(Server->Database);

    SocketListen(
        Server->ClientSocket, 
        Server->Config->Auth.Port
    );

    SocketListen(
        Server->MasterSocket,
        Server->Config->MasterSvr.Port
    );

    srand(GetTimestamp());

    while (true) {
        Server->Timestamp = GetTimestamp();

        SocketUpdate(Server->ClientSocket);
        SocketUpdate(Server->MasterSocket);

        for (Int32 Index = 0; Index < Server->ClientSocket->Connections.Count; Index++) {
            SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
                &Server->ClientSocket->Connections, 
                Index
            );
            ClientContextRef Client = (ClientContextRef)Connection->Userdata;
            if (Client->Flags & CLIENT_FLAGS_CHECK_DISCONNECT_TIMER) {
                if (Client->DisconnectTimestamp < Server->Timestamp) {
                    Client->Flags &= ~CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
                    SocketDisconnect(Server->ClientSocket, Connection);
                    continue;
                }
            }
        }

        if (Server->WorldListBroadcastTimestamp < Server->WorldListUpdateTimestamp) {
            Server->WorldListBroadcastTimestamp = Server->Timestamp;
            Server->WorldListUpdateTimestamp = Server->WorldListBroadcastTimestamp;
            BroadcastWorldList(Server);
        }
    }
}

MasterContextRef ServerGetMaster(
    ServerContextRef Server,
    Int32 ServerID
) {
    for (Int32 Index = 0; Index < Server->MasterSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Server->MasterSocket->Connections,
            Index
        );
        MasterContextRef Master = (MasterContextRef)Connection->Userdata;

        if (Master->ServerID == ServerID) return Master;
    }

    return NULL;
}

MasterContextWorldRef ServerGetWorld(
    ServerContextRef Server,
    Int32 ServerID,
    Int32 WorldID
) {
    MasterContextRef Master = ServerGetMaster(Server, ServerID);
    if (!Master) return NULL;

    for (Int32 WorldIndex = 0; WorldIndex < Master->Worlds.Count; WorldIndex++) {
        MasterContextWorldRef World = (MasterContextWorldRef)ArrayGetElementAtIndex(
            &Master->Worlds,
            WorldIndex
        );

        if (World->WorldID == WorldID) return World;
    }

    return NULL;
}

#include "ClientProtocol.h"
#include "Notification.h"

S2C_DATA_SERVERLIST* BuildWorldListNotification(
    ServerContextRef Server
) {
    S2C_DATA_SERVERLIST* Notification = PacketInit(S2C_DATA_SERVERLIST);
    Notification->Command = S2C_SERVERLIST;
    Notification->ServerCount = 0;

    for (Int32 Index = 0; Index < Server->MasterSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Server->MasterSocket->Connections,
            Index
        );
        MasterContextRef Master = (MasterContextRef)Connection->Userdata;

        S2C_DATA_SERVER* Server = PacketAppendStruct(S2C_DATA_SERVER);
        Server->ServerID = Master->ServerID;
        Server->WorldCount = 0;

        for (Int32 WorldIndex = 0; WorldIndex < Master->Worlds.Count; WorldIndex++) {
            MasterContextWorldRef MasterWorld = (MasterContextWorldRef)ArrayGetElementAtIndex(
                &Master->Worlds,
                WorldIndex
            );

            S2C_DATA_WORLD* World = PacketAppendStruct(S2C_DATA_WORLD);
            World->ServerID = Server->ServerID;
            World->WorldID = MasterWorld->WorldID;
            World->PlayerCount = MasterWorld->PlayerCount;
            World->MaxPlayerCount = MasterWorld->MaxPlayerCount;
            memcpy(World->WorldHost, MasterWorld->WorldHost, sizeof(MasterWorld->WorldHost));
            World->WorldPort = MasterWorld->WorldPort;
            World->WorldType = MasterWorld->WorldType;

            Server->WorldCount += 1;
        }

        Server->WorldCount += 1;
        S2C_DATA_WORLD* ZeroWorld = PacketAppendStruct(S2C_DATA_WORLD);

        Notification->ServerCount += 1;
    }

    return Notification;
}

S2C_DATA_UNKNOWN124* BuildMessageUnknown124(
    ServerContextRef Server
) {
    S2C_DATA_UNKNOWN124* Response = PacketInit(S2C_DATA_UNKNOWN124);
    Response->Command = S2C_UNKNOWN124;
    Response->Unknown1 = 0;
    Response->Unknown2[0] = 100;
    Response->Unknown2[1] = 200;
    Response->Unknown2[2] = 300;
    Response->Unknown2[3] = 400;
    Response->Unknown3 = 1;
    Response->Unknown4[0] = 500;
    Response->Unknown4[1] = 600;
    Response->Unknown4[2] = 700;
    Response->Unknown4[3] = 800;
    return Response;
}

Void BroadcastWorldList(
    ServerContextRef Server
) {
    S2C_DATA_SERVERLIST* Notification = BuildWorldListNotification(Server);

    for (Int32 Index = 0; Index < Server->ClientSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Server->ClientSocket->Connections,
            Index
        );
        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        
        Bool Authorized = (
            (Client->Flags & CLIENT_FLAGS_AUTHENTICATED) &&
            !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED)
        );
        if (Authorized) {
            SocketSend(Server->ClientSocket, Connection, Notification);
        }
    }

    S2C_DATA_UNKNOWN124* Response = BuildMessageUnknown124(Server);

    for (Int32 Index = 0; Index < Server->ClientSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Server->ClientSocket->Connections,
            Index
        );
        ClientContextRef Client = (ClientContextRef)Connection->Userdata;

        Bool Authorized = (
            (Client->Flags & CLIENT_FLAGS_AUTHENTICATED) &&
            !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED)
            );
        if (Authorized) {
            SocketSend(Server->ClientSocket, Connection, Response);
        }
    }
}

Void BroadcastWorldListToConnection(
    ServerContextRef Server,
    SocketConnectionRef Connection
) {
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    Bool Authorized = (
        (Client->Flags & CLIENT_FLAGS_AUTHENTICATED) &&
        !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED)
    );
    if (Authorized) {
        S2C_DATA_SERVERLIST* Notification = BuildWorldListNotification(Server);
        SocketSend(Server->ClientSocket, Connection, Notification);

        S2C_DATA_UNKNOWN124* Response = BuildMessageUnknown124(Server);
        SocketSend(Server->ClientSocket, Connection, Notification);
    }
}
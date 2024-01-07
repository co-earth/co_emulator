#include "ClientSocket.h"
#include "ClientProcs.h"
#include "Server.h"

Void ClientSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)malloc(sizeof(struct _ClientContext));
    if (!Client) FatalError("Memory allocation failed!");

    Client->Connection = Connection;
    Client->AccountID = -1;
    Client->Flags = 0;
    Client->DisconnectTimestamp = 0;
    Client->RSA = NULL;

    Connection->Userdata = Client;
}

Void ClientSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;

    if (Client->RSA) {
        RSA_free(Client->RSA);
    }

    free(Client);
    Connection->Userdata = NULL;
}

Void ClientSocketOnSend(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;

    struct { S2C_DATA_SIGNATURE; } *Header = Packet;

    switch (Header->Command) {
#define S2C_COMMAND(__NAME__, __VALUE__)                         \
    case __NAME__: {                                             \
        LogMessageFormat(LOG_LEVEL_INFO, "SEND: %s", #__NAME__); \
        return;                                                  \
    }
#include "ClientCommands.h"

    default: {                                             \
        LogMessageFormat(LOG_LEVEL_ERROR, "SEND UNKNOWN: %d", Header->Command); \
        return;                                                  \
    }
    }
}

Void ClientSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    
    struct { C2S_DATA_SIGNATURE; } *Header = Packet;

    switch (Header->Command) {
#define CLIENT_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__)                 \
    case __COMMAND__: {                                                       \
        LogMessageFormat(LOG_LEVEL_INFO, "RECV: %s", #__COMMAND__);           \
        __NAME__(Server, Socket, Client, Connection, (__PROTOCOL__ *)Packet); \
        return;                                                               \
    }
#include "ClientProcDefinition.h"

    default: {
        LogMessageFormat(
            LOG_LEVEL_WARNING, 
            "Received unknown packet with command: %d", 
            Header->Command
        );
        return;
    }
    }
}

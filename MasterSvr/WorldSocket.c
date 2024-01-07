#include "Server.h"
#include "IPCProcs.h"
#include "Notification.h"

Void WorldSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)malloc(sizeof(struct _ClientContext));
    if (!Client) FatalError("Memory allocation failed!");

    memset(Client, 0, sizeof(struct _ClientContext));
    Client->Connection = Connection;

    Connection->Userdata = Client;
}

Void WorldSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    assert(Client);

    free(Client);

    Server->WorldListUpdateTimestamp = Server->Timestamp;
}

Void WorldSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;

    struct { IPC_DATA_SIGNATURE; } *Header = Packet;

    switch (Header->Command) {
#define IPC_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__)                    \
    case __COMMAND__: {                                                       \
        LogMessageFormat(LOG_LEVEL_INFO, "RECV: %s", #__COMMAND__);           \
        __NAME__(Server, Socket, Client, Connection, (__PROTOCOL__ *)Packet); \
        return;                                                               \
    }
#include "IPCProcDefinition.h"

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

#include "AuthSocket.h"
#include "IPCProtocol.h"
#include "IPCProcs.h"

Void AuthSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)malloc(sizeof(struct _ClientContext));
    if (!Client) FatalError("Memory allocation failed!");

    memset(Client, 0, sizeof(struct _ClientContext));
    Client->Connection = Connection;

    Connection->Userdata = Client;

    IPC_DATA_AUTH_REQCONNECT* Request = PacketInit(IPC_DATA_AUTH_REQCONNECT);
    Request->Command = IPC_AUTH_REQCONNECT;
    Request->ServerID = Server->Config->MasterSvr.ServerID;
    SocketSend(Socket, Connection, Request);

    Server->Flags |= SERVER_FLAGS_AUTH_CONNECTED;
}

Void AuthSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    assert(Client);

    free(Client);

    Server->Flags &= ~SERVER_FLAGS_AUTH_CONNECTED;
}

Void AuthSocketOnReceived(
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

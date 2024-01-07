#include "Server.h"
#include "IPCProcs.h"

Void MasterSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    MasterContextRef Master = (MasterContextRef)malloc(sizeof(struct _MasterContext));
    if (!Master) FatalError("Memory allocation failed!");

    Master->ServerID = 0;
    ArrayInitializeEmpty(&Master->Worlds, sizeof(struct _MasterContextWorld), 0);

    Connection->Userdata = Master;
}

Void MasterSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    MasterContextRef Master = (MasterContextRef)Connection->Userdata;
    assert(Master);

    ArrayDeinitialize(&Master->Worlds);
    free(Master);
}

Void MasterSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    MasterContextRef Master = (MasterContextRef)Connection->Userdata;

    struct { IPC_DATA_SIGNATURE; } *Header = Packet;

    switch (Header->Command) {
#define IPC_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__)                    \
    case __COMMAND__: {                                                       \
        LogMessageFormat(LOG_LEVEL_INFO, "RECV: %s", #__COMMAND__);           \
        __NAME__(Server, Socket, Master, Connection, (__PROTOCOL__ *)Packet); \
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

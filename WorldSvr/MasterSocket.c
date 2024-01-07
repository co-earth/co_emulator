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

    Connection->Userdata = Master;

    IPC_DATA_WORLD_REQCONNECT* Request = PacketInit(IPC_DATA_WORLD_REQCONNECT);
    Request->Command = IPC_WORLD_REQCONNECT;
    Request->WorldID = Server->Config->WorldSvr.WorldID;
    memcpy(Request->WorldHost, Server->Config->WorldSvr.Host, sizeof(Request->WorldHost));
    Request->WorldPort = Server->Config->WorldSvr.Port;
    Request->WorldType = Server->Config->WorldSvr.WorldType;
    Request->MaxPlayerCount = Server->Config->WorldSvr.MaxConnectionCount;
    SocketSend(Socket, Connection, Request);
}

Void MasterSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    MasterContextRef Master = (MasterContextRef)Connection->Userdata;
    assert(Master);

    free(Master);

    Server->Flags &= ~SERVER_FLAGS_MASTER_CONNECTED;
}

Void MasterSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    MasterContextRef Master = (MasterContextRef)Connection->Userdata;
    RTRuntimeRef Runtime = Server->Runtime;
    SocketConnectionRef ClientConnection = NULL;
    ClientContextRef Client = NULL;
    RTCharacterRef Character = NULL;
    
    struct { IPC_DATA_SIGNATURE; } *Header = Packet;

    ClientConnection = SocketGetConnection(Server->ClientSocket, Header->ConnectionID);
    if (ClientConnection) {
        Client = (ClientContextRef)ClientConnection->Userdata;
    }

    if (Client && Client->CharacterEntity.Serial > 0) {
        Character = RTRuntimeGetCharacter(Runtime, Client->CharacterEntity);
    }

    switch (Header->Command) {
#define IPC_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__)                      \
    case __COMMAND__: {                                                         \
        LogMessageFormat(LOG_LEVEL_INFO, "RECV: %s", #__COMMAND__);             \
        __NAME__(                                                               \
            Server,                                                             \
            Socket,                                                             \
            Master,                                                             \
            Connection,                                                         \
            ClientConnection,                                                   \
            Client,                                                             \
            Runtime,                                                            \
            Character,                                                          \
            (__PROTOCOL__ *)Packet                                              \
        );                                                                      \
        return;                                                                 \
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

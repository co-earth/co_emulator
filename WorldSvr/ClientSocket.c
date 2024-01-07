#include "ClientCommands.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "Server.h"

Void ClientSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)malloc(sizeof(struct _ClientContext));
    if (!Client) FatalError("Memory allocation failed!");

    memset(Client, 0, sizeof(struct _ClientContext));
    Client->Connection = Connection;
    Client->Account.AccountID = -1;

    Connection->Userdata = Client;
}

Void ClientSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    assert(Client);

    if (Client->CharacterEntity.Serial > 0) {
        RTCharacterRef Character = RTRuntimeGetCharacter(Server->Runtime, Client->CharacterEntity);
        RTWorldRef World = RTRuntimeGetWorldByCharacter(Server->Runtime, Character);

        // TODO: Delete character dungeon instance and respawn to global world

        RTWorldDespawnCharacter(
            Server->Runtime,
            World,
            Client->CharacterEntity
        );

        // TODO: Delete Entity from Runtime!
    }

    free(Client);
    
    Connection->Userdata = NULL;

    LogMessage(LOG_LEVEL_INFO, "Client disconnected...");
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
    case S2C_ ## __NAME__: {                                     \
        LogMessageFormat(LOG_LEVEL_INFO, "SEND: %s", #__NAME__); \
        return;                                                  \
    }
#include "ClientCommands.h"

    default:
        return;
    }
}

Void ClientSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
) {
    ServerContextRef Server = (ServerContextRef)Socket->Userdata;
    ClientContextRef Client = (ClientContextRef)Connection->Userdata;
    RTRuntimeRef Runtime = Server->Runtime;
    RTCharacterRef Character = NULL;

    if (Client->CharacterEntity.Serial > 0) {
        Character = RTRuntimeGetCharacter(Runtime, Client->CharacterEntity);
    }
    
    struct { C2S_DATA_SIGNATURE; } *Header = Packet;

    switch (Header->Command) {
#define C2S_COMMAND(__NAME__, __VALUE__)                         \
    case C2S_ ## __NAME__: {                                     \
        LogMessageFormat(LOG_LEVEL_INFO, "RECV: %s", #__NAME__); \
        PROC_ ## __NAME__(                                       \
            Server,                                              \
            Socket,                                              \
            Client,                                              \
            Connection,                                          \
            Runtime,                                             \
            Character,                                           \
            (C2S_DATA_ ## __NAME__ *)Packet                      \
        );                                                       \
        return;                                                  \
    }
#include "ClientCommands.h"

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

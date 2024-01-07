#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void ClientSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void ClientSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void ClientSocketOnSend(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
);

Void ClientSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
);

EXTERN_C_END
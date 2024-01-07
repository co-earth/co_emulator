#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void WorldSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void WorldSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void WorldSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
);

EXTERN_C_END
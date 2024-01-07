#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void AuthSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void AuthSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void AuthSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
);

EXTERN_C_END
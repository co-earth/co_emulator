#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void MasterSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void MasterSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void MasterSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
);

EXTERN_C_END
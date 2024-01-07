#pragma once

#include "Base.h"
#include "Server.h"

EXTERN_C_BEGIN

Void BroadcastWorldList(
    ServerContextRef Server
);

Void BroadcastWorldListToConnection(
    ServerContextRef Server,
    SocketConnectionRef Connection
);

EXTERN_C_END
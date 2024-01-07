#pragma once

#include "Base.h"
#include "Server.h"

EXTERN_C_BEGIN

Void BroadcastUserList(
    ServerContextRef Server
);

Void BroadcastToWorld(
    ServerContextRef Server,
    RTWorldRef World,
    RTEntity Entity,
    Int32 X,
    Int32 Y,
    PacketRef Packet
);

EXTERN_C_END
#pragma once

#include "Base.h"
#include "Context.h"
#include "ClientProtocol.h"

EXTERN_C_BEGIN

#ifndef CLIENT_PROCEDURE_BINDING(__NAME__)
#define CLIENT_PROCEDURE_BINDING(__NAME__) \
Void PROC_ ## __NAME__(                    \
    ServerContextRef Server,               \
    SocketRef Socket,                      \
    ClientContextRef Client,               \
    SocketConnectionRef Connection,        \
    RTRuntimeRef Runtime,                  \
    RTCharacterRef Character,              \
    C2S_DATA_ ## __NAME__* Packet          \
)
#endif

#define C2S_COMMAND(__NAME__, __VALUE__) \
CLIENT_PROCEDURE_BINDING(__NAME__);
#include "ClientCommands.h"

EXTERN_C_END
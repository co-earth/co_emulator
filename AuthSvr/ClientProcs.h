#pragma once

#include "Base.h"
#include "Context.h"
#include "ClientProtocol.h"

EXTERN_C_BEGIN

#ifndef CLIENT_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__)
#define CLIENT_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__) \
Void __NAME__(                                                        \
    ServerContextRef Server,                                          \
    SocketRef Socket,                                                 \
    ClientContextRef Client,                                          \
    SocketConnectionRef Connection,                                   \
    __PROTOCOL__* Packet                                              \
)
#endif

#define CLIENT_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
CLIENT_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__);
#include "ClientProcDefinition.h"

EXTERN_C_END
#pragma once

#include "Base.h"
#include "Context.h"
#include "IPCProtocol.h"

EXTERN_C_BEGIN

#ifndef IPC_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__)
#define IPC_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__) \
Void __NAME__(                                                     \
    ServerContextRef Server,                                       \
    SocketRef Socket,                                              \
    MasterContextRef Master,                                       \
    SocketConnectionRef Connection,                                \
    __PROTOCOL__* Packet                                           \
)
#endif

#define IPC_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
IPC_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__);
#include "IPCProcDefinition.h"

EXTERN_C_END
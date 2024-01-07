#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void ServerRun(
    ServerContextRef Server,
    CString ConfigFilePath
);

Void ServerSyncDB(
    ServerContextRef Server,
    Bool Force
);

Void ServerRuntimeOnEvent(
    RTRuntimeRef Runtime,
    RTEventRef Event,
    Void* UserData
);

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Server,
    UInt32 AuthKey,
    UInt16 EntityID
);

ClientContextRef ServerGetClientByEntity(
    ServerContextRef Server,
    RTEntity Entity
);

EXTERN_C_END
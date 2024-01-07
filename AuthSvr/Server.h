#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void ServerRun(
    ServerContextRef Server,
    CString ConfigFilePath
);

MasterContextRef ServerGetMaster(
    ServerContextRef Server,
    Int32 ServerID
);

MasterContextWorldRef ServerGetWorld(
    ServerContextRef Server,
    Int32 ServerID,
    Int32 WorldID
);

EXTERN_C_END
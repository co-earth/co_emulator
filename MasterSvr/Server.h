#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void ServerRun(
    ServerContextRef Server,
    CString ConfigFilePath
);

ClientContextRef ServerGetWorld(
    ServerContextRef Server,
    Int32 WorldID
);

EXTERN_C_END
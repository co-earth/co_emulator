#pragma once

#include "Base.h"

EXTERN_C_BEGIN

enum {
    LOG_LEVEL_INFO       = 1,
    LOG_LEVEL_WARNING    = 2,
    LOG_LEVEL_ERROR      = 3,
};

typedef Void (*DiagnosticHandler)(
    Int32 Level,
    CString Message,
    Void* Context
);

Void DiagnosticSetDefaultHandler(
    DiagnosticHandler Handler,
    Void* Context
);

Void LogMessage(
    Int32 Level,
    CString Message
);

Void LogMessageFormat(
    Int32 Level,
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

ATTRIBUTE_NORETURN Void FatalError(
    CString Message
);

Void FatalErrorFormat(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

EXTERN_C_END

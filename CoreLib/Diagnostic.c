#include "Diagnostic.h"

Void _DefaultDiagnosticHandler(
    Int32 Level,
    CString Message,
    Void* Context
) {
    switch (Level) {
        case LOG_LEVEL_INFO:     return fprintf(stdout, "\033[92m[INFO] %s\033[0m\n", Message);
        case LOG_LEVEL_WARNING:  return fprintf(stdout, "\033[33m[WARNING] %s\033[0m\n", Message);
        case LOG_LEVEL_ERROR:    return fprintf(stderr, "\033[31m[ERROR] %s\033[0m\n", Message);
        default:                 return fprintf(stdout, "\033[37m%s\033[0m\n", Message);
    }
}

static struct {
    DiagnosticHandler Handler;
    Void* Context;
    Char FormatBuffer[0xFFFF];
} kDiagnosticEngine = { &_DefaultDiagnosticHandler, NULL, 0 };

Void DiagnosticSetDefaultHandler(
    DiagnosticHandler Handler,
    Void* Context
) {
    if (Handler) {
        kDiagnosticEngine.Handler = Handler;
        kDiagnosticEngine.Context = NULL;
    } else {
        kDiagnosticEngine.Handler = &_DefaultDiagnosticHandler;
        kDiagnosticEngine.Context = Context;
    }
}

Void LogMessage(
    Int32 Level,
    CString Message
) {
    kDiagnosticEngine.Handler(Level, Message, kDiagnosticEngine.Context);
}

Void LogMessageFormat(
    Int32 Level,
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(&kDiagnosticEngine.FormatBuffer[0], Format, ArgumentPointer);
    va_end(ArgumentPointer);

    LogMessage(Level, kDiagnosticEngine.FormatBuffer);
}

Void FatalError(
    CString Message
) {
    kDiagnosticEngine.Handler(LOG_LEVEL_ERROR, Message, kDiagnosticEngine.Context);
    exit(EXIT_FAILURE);
}

Void FatalErrorFormat(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(&kDiagnosticEngine.FormatBuffer[0], Format, ArgumentPointer);
    va_end(ArgumentPointer);

    FatalError(kDiagnosticEngine.FormatBuffer);
}

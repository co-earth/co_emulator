#include "Context.h"
#include "Server.h"

Int32 main(Int32 argc, CString* argv) {
    if (!EncryptionLoadLibrary()) {
        FatalError(LOG_LEVEL_ERROR, "Error loading library...\n");
    }

    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "WorldSvr.ini");

    ServerContextRef Server = malloc(sizeof(struct _ServerContext));
    if (!Server) FatalError("Memory allocation failed!");

    Server->Config = &Server->Memory.ConfigMemory;
    Server->ClientSocket = &Server->Memory.ClientSocketMemory;
    Server->MasterSocket = &Server->Memory.MasterSocketMemory;
    Server->Runtime = NULL;
    Server->RuntimeData = &Server->Memory.RuntimeDataMemory;
    Server->RuntimeContext = &Server->Memory.RuntimeContextMemory;
    Server->Flags = 0;
    Server->Timestamp = GetTimestamp();
    Server->UserListBroadcastTimestamp = 0;
    Server->UserListUpdateTimestamp = 0;

    ServerRun(Server, ConfigFilePath);
    
    free(Server);
    return EXIT_SUCCESS;
}

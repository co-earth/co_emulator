#include "Context.h"
#include "Server.h"

Int32 main(Int32 argc, CString* argv) {
    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString ConfigFilePath = PathCombineNoAlloc(WorkingDirectory, "AuthSvr.ini");

    ServerContextRef Server = malloc(sizeof(struct _ServerContext));
    if (!Server) FatalError("Memory allocation failed!");

    Server->Config = &Server->Memory.ConfigMemory;
    Server->ClientSocket = &Server->Memory.ClientSocketMemory;
    Server->MasterSocket = &Server->Memory.MasterSocketMemory;
    Server->Database = NULL;
    Server->Flags = 0;
    Server->Timestamp = GetTimestamp();
    Server->WorldListBroadcastTimestamp = 0;
    Server->WorldListUpdateTimestamp = 0;
    
    ServerRun(Server, ConfigFilePath);
    
    free(Server);
    return 0;
}

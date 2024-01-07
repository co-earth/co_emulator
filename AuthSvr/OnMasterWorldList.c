#include "IPCProtocol.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(OnMasterWorldList, IPC_AUTH_NFYWORLDLIST, IPC_DATA_AUTH_NFYWORLDLIST) {
	ArrayRemoveAllElements(&Master->Worlds, true);

	IPC_DATA_AUTH_WORLD* Worlds = (IPC_DATA_AUTH_WORLD*)Packet->Data;

	for (Int32 Index = 0; Index < Packet->WorldCount; Index++) {
		IPC_DATA_AUTH_WORLD* World = (IPC_DATA_AUTH_WORLD*)&Worlds[Index];
		MasterContextWorldRef MasterWorld = ArrayAppendUninitializedElement(&Master->Worlds);
		MasterWorld->WorldID = World->WorldID;
		MasterWorld->PlayerCount = World->PlayerCount;
		MasterWorld->MaxPlayerCount = World->MaxPlayerCount;
		memcpy(MasterWorld->WorldHost, World->WorldHost, sizeof(World->WorldHost));
		MasterWorld->WorldPort = World->WorldPort;
		MasterWorld->WorldType = World->WorldType;
	}

	Server->WorldListUpdateTimestamp = Server->Timestamp;
}

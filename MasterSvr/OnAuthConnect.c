#include "Server.h"
#include "IPCProcs.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnAuthConnect, IPC_AUTH_ACKCONNECT, IPC_DATA_AUTH_ACKCONNECT) {
	Server->WorldListUpdateTimestamp = Server->Timestamp;
}
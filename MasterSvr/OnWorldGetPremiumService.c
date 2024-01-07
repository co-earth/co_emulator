#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldGetPremiumService, IPC_WORLD_REQPREMIUMSERVICE, IPC_DATA_WORLD_REQPREMIUMSERVICE) {
	IPC_DATA_WORLD_ACKPREMIUMSERVICE* Response = PacketInit(IPC_DATA_WORLD_ACKPREMIUMSERVICE);
	Response->Command = IPC_WORLD_ACKPREMIUMSERVICE;
	Response->ConnectionID = Packet->ConnectionID;

	MASTERDB_DATA_SERVICE Service = { 0 };
	Service.AccountID = Packet->AccountID;
	if (MasterDBSelectCurrentActiveServiceByAccount(Server->Database, &Service)) {
		Response->HasService = true;
		Response->ServiceType = Service.ServiceType;
		Response->StartedAt = Service.StartedAt;
		Response->ExpiredAt = Service.ExpiredAt;
	}

	SocketSend(Socket, Connection, Response);
}
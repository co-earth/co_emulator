#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_ENVIRONMENT) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;

	S2C_DATA_GET_SERVER_ENVIRONMENT* Response = PacketInit(S2C_DATA_GET_SERVER_ENVIRONMENT);
	Response->Command = S2C_GET_SERVER_ENVIRONMENT;
	Response->MaxLevel = Server->Config->Environment.MaxLevel;
	Response->DummyEnabled = Server->Config->Environment.DummyEnabled;
	Response->CashshopEnabled = Server->Config->Environment.CashshopEnabled;
	Response->NetCafePointEnabled = Server->Config->Environment.NetCafePointEnabled;
	Response->MinChatLevel = Server->Config->Environment.MinChatLevel;
	Response->MinShoutLevel = Server->Config->Environment.MinShoutLevel;
	Response->MinShoutSkillRank = Server->Config->Environment.MinShoutSkillRank;
	Response->MaxInventoryCurrency = Server->Config->Environment.MaxInventoryCurrency;
	Response->MaxWarehouseCurrency = Server->Config->Environment.MaxWarehouseCurrency;
	Response->MaxAuctionCurrency = Server->Config->Environment.MaxAuctionCurrency;
	Response->MaxHonorPoint = Server->Config->Environment.MaxHonorPoint;
	Response->MinHonorPoint = Server->Config->Environment.MinHonorPoint;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

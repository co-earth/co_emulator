#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(OnUnknown5383, C2S_UNKNOWN5383, C2S_DATA_UNKNOWN5383) {
	S2C_DATA_UNKNOWN5383* Response = PacketInit(S2C_DATA_UNKNOWN5383);
	Response->Command = S2C_UNKNOWN5383;
	SocketSend(Socket, Connection, Response);
}
#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(OnUnknown3383, C2S_UNKNOWN3383, C2S_DATA_UNKNOWN3383) {
	S2C_DATA_UNKNOWN3383* Response = PacketInit(S2C_DATA_UNKNOWN3383);
	Response->Command = S2C_UNKNOWN3383;
	SocketSend(Socket, Connection, Response);
}
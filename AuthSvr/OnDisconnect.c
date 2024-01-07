#include "ClientProtocol.h"
#include "ClientProcs.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(OnDisconnect, C2S_DISCONNECT, C2S_DATA_DISCONNECT) {
    SocketDisconnect(Socket, Connection);
}
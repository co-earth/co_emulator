#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHECK_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;
	
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	Bool IsVerifiedSession = time(NULL) < Client->Account.SessionTimeout;

	S2C_DATA_CHECK_SUBPASSWORD* Response = PacketInit(S2C_DATA_CHECK_SUBPASSWORD);
	Response->Command = S2C_CHECK_SUBPASSWORD;
	Response->IsVerificationRequired = !IsVerifiedSession && IsSubpasswordSet;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

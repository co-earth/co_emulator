#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

Void OnVerifyDeleteCharacterSubpassword(
	ServerContextRef Server,
	SocketRef Socket,
	ClientContextRef Client,
	SocketConnectionRef Connection,
	C2S_DATA_VERIFY_DELETE_SUBPASSWORD* Packet
) {
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	if (!IsSubpasswordSet || strlen(Packet->Password) < MIN_SUBPASSWORD_LENGTH ||
		memcmp(Client->Account.CharacterPassword, Packet->Password, MAX_SUBPASSWORD_LENGTH) != 0) {
		Client->SubpasswordFailureCount += 1;
		if (Client->SubpasswordFailureCount >= Server->Config->WorldSvr.MaxSubpasswordFailureCount) {
			// TODO: Ban account based on configuration due to max failure count reach!
		}

		S2C_DATA_VERIFY_DELETE_SUBPASSWORD* Response = PacketInit(S2C_DATA_VERIFY_DELETE_SUBPASSWORD);
		Response->Command = S2C_VERIFY_DELETE_SUBPASSWORD;
		Response->Success = 0;
		Response->FailureCount = Client->SubpasswordFailureCount;
		Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
		return SocketSend(Socket, Connection, Response);
	}

	Client->SubpasswordFailureCount = 0;
	Client->Flags |= CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;
	Client->Account.SessionTimeout = max(Client->Account.SessionTimeout, time(NULL) + 60);

	S2C_DATA_VERIFY_DELETE_SUBPASSWORD* Response = PacketInit(S2C_DATA_VERIFY_DELETE_SUBPASSWORD);
	Response->Command = S2C_VERIFY_DELETE_SUBPASSWORD;
	Response->Success = 1;
	Response->FailureCount = Client->SubpasswordFailureCount;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(VERIFY_DELETE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	if (Packet->Type == CHARACTER_SUBPASSWORD_TYPE_CHARACTER) {
		return OnVerifyDeleteCharacterSubpassword(
			Server,
			Socket,
			Client,
			Connection,
			Packet
		);
	}

	S2C_DATA_VERIFY_DELETE_SUBPASSWORD* Response = PacketInit(S2C_DATA_VERIFY_DELETE_SUBPASSWORD);
	Response->Command = S2C_VERIFY_DELETE_SUBPASSWORD;
	Response->Success = 0;
	Response->FailureCount =0;
	Response->Type = Packet->Type;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

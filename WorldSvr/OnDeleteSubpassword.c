#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

Void OnDeleteCharacterSubpassword(
	ServerContextRef Server,
	SocketRef Socket,
	ClientContextRef Client,
	SocketConnectionRef Connection,
	C2S_DATA_DELETE_SUBPASSWORD* Packet
) {
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	Bool IsDeletionVerified = Client->Flags & CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;
	Bool IsSessionExpired = Client->Account.SessionTimeout <= time(NULL);
	if (!IsSubpasswordSet || !IsDeletionVerified || IsSessionExpired) {
		S2C_DATA_DELETE_SUBPASSWORD* Response = PacketInit(S2C_DATA_DELETE_SUBPASSWORD);
		Response->Command = S2C_DELETE_SUBPASSWORD;
		Response->Success = 0;
		Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
		return SocketSend(Socket, Connection, Response);
	}

	memset(Client->Account.CharacterPassword, 0, MAX_SUBPASSWORD_LENGTH);
	Client->Account.CharacterQuestion = 0;
	memset(Client->Account.CharacterAnswer, 0, MAX_SUBPASSWORD_ANSWER_LENGTH);

	IPC_DATA_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA* Request = PacketInit(IPC_DATA_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA);
	Request->Command = IPC_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA;
	Request->ConnectionID = Connection->ID;
	Request->AccountID = Client->Account.AccountID;
	memcpy(Request->CharacterPassword, Client->Account.CharacterPassword, MAX_SUBPASSWORD_LENGTH);
	Request->CharacterQuestion = Client->Account.CharacterQuestion;
	memcpy(Request->CharacterAnswer, Client->Account.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);
	SocketSendAll(Server->MasterSocket, Request);

	S2C_DATA_DELETE_SUBPASSWORD* Response = PacketInit(S2C_DATA_DELETE_SUBPASSWORD);
	Response->Command = S2C_DELETE_SUBPASSWORD;
	Response->Success = 1;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(DELETE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	if (Packet->Type == CHARACTER_SUBPASSWORD_TYPE_CHARACTER) {
		return OnDeleteCharacterSubpassword(
			Server,
			Socket,
			Client,
			Connection,
			Packet
		);
	}

	S2C_DATA_DELETE_SUBPASSWORD* Response = PacketInit(S2C_DATA_DELETE_SUBPASSWORD);
	Response->Command = S2C_DELETE_SUBPASSWORD;
	Response->Success = 0;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

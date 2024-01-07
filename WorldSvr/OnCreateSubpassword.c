#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

Void OnCreateCharacterSubpassword(
	ServerContextRef Server,
	SocketRef Socket,
	ClientContextRef Client,
	SocketConnectionRef Connection,
	C2S_DATA_CREATE_SUBPASSWORD* Packet
) {
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	if (IsSubpasswordSet ||
		strlen(Packet->Password) < MIN_SUBPASSWORD_LENGTH ||
		strlen(Packet->Answer) < MIN_SUBPASSWORD_ANSWER_LENGTH) {
		goto error;
	}

	memcpy(Client->Account.CharacterPassword, Packet->Password, MAX_SUBPASSWORD_LENGTH);
	Client->Account.CharacterQuestion = Packet->Question;
	memcpy(Client->Account.CharacterAnswer, Packet->Answer, MAX_SUBPASSWORD_ANSWER_LENGTH);

	IPC_DATA_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA* Request = PacketInit(IPC_DATA_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA);
	Request->Command = IPC_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA;
	Request->ConnectionID = Connection->ID;
	Request->AccountID = Client->Account.AccountID;
	memcpy(Request->CharacterPassword, Client->Account.CharacterPassword, MAX_SUBPASSWORD_LENGTH);
	Request->CharacterQuestion = Client->Account.CharacterQuestion;
	memcpy(Request->CharacterAnswer, Client->Account.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);
	SocketSendAll(Server->MasterSocket, Request);

	S2C_DATA_CREATE_SUBPASSWORD* Response = PacketInit(S2C_DATA_CREATE_SUBPASSWORD);
	Response->Command = S2C_CREATE_SUBPASSWORD;
	Response->Success = 1;
	Response->Mode = Packet->Mode;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CREATE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	if (Packet->Type == CHARACTER_SUBPASSWORD_TYPE_CHARACTER) {
		return OnCreateCharacterSubpassword(
			Server,
			Socket,
			Client,
			Connection,
			Packet
		);
	}

	S2C_DATA_CREATE_SUBPASSWORD* Response = PacketInit(S2C_DATA_CREATE_SUBPASSWORD);
	Response->Command = S2C_CREATE_SUBPASSWORD;
	Response->Success = 0;
	Response->Mode = Packet->Mode;
	Response->Type = Packet->Type;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

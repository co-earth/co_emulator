#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(VERIFY_CREDENTIALS) {
	Int32 Length = strlen(Packet->Credentials);
	if (Length < 1 || Length >= MAX_CREDENTIALS_LENGTH) {
		goto error;
	}

	if (Packet->CredentialsType == C2S_DATA_VERIFY_CREDENTIALS_TYPE_PASSWORD) {
		IPC_DATA_WORLD_REQVERIFYPASSWORD* Request = PacketInit(IPC_DATA_WORLD_REQVERIFYPASSWORD);
		Request->Command = IPC_WORLD_REQVERIFYPASSWORD;
		Request->ConnectionID = Connection->ID;
		Request->AccountID = Client->Account.AccountID;
		memcpy(Request->Credentials, Packet->Credentials, MAX_CREDENTIALS_LENGTH);
		return SocketSendAll(Server->MasterSocket, Request);
	}

error: 
	{
		S2C_DATA_VERIFY_CREDENTIALS* Response = PacketInit(S2C_DATA_VERIFY_CREDENTIALS);
		Response->Success = false;
		return SocketSend(Socket, Connection, Response);
	}
}

IPC_PROCEDURE_BINDING(OnWorldVerifyPassword, IPC_WORLD_ACKVERIFYPASSWORD, IPC_DATA_WORLD_ACKVERIFYPASSWORD) {
	if (!ClientConnection || !Client) goto error;

	if (Packet->Success) {
		Client->PasswordVerificationTimestamp = GetTimestamp();
	}

	S2C_DATA_VERIFY_CREDENTIALS* Response = PacketInit(S2C_DATA_VERIFY_CREDENTIALS);
    Response->Command = S2C_VERIFY_CREDENTIALS;
	Response->Success = Packet->Success;
    return SocketSend(Server->ClientSocket, ClientConnection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(VERIFY_CREDENTIALS_SUBPASSWORD) {

	/*
	Void OnVerifyCharacterSubpassword(
		ServerContextRef Server,
		SocketRef Socket,
		ClientContextRef Client,
		SocketConnectionRef Connection,
		C2S_DATA_VERIFY_SUBPASSWORD * Packet
	) {
		Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
		if (!IsSubpasswordSet || strlen(Packet->Password) < MIN_SUBPASSWORD_LENGTH ||
			memcmp(Client->Account.CharacterPassword, Packet->Password, MAX_SUBPASSWORD_LENGTH) != 0) {
			Client->SubpasswordFailureCount += 1;
			if (Client->SubpasswordFailureCount >= Server->Config->WorldSvr.MaxSubpasswordFailureCount) {
				// TODO: Ban account based on configuration due to max failure count reach!
			}

			S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketInit(S2C_DATA_VERIFY_SUBPASSWORD);
			Response->Command = S2C_VERIFY_SUBPASSWORD;
			Response->Success = 0;
			Response->FailureCount = Client->SubpasswordFailureCount;
			Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
			return SocketSend(Socket, Connection, Response);
		}

		Client->SubpasswordFailureCount = 0;
		memcpy(Client->Account.SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
		Client->Account.SessionTimeout = time(NULL) + (time_t)Packet->ExpirationInHours * 60 * 60;

		IPC_DATA_WORLD_UPDATE_ACCOUNT_SESSION_DATA* Request = PacketInit(IPC_DATA_WORLD_UPDATE_ACCOUNT_SESSION_DATA);
		Request->Command = IPC_WORLD_UPDATE_ACCOUNT_SESSION_DATA;
		Request->ConnectionID = Connection->ID;
		Request->AccountID = Client->Account.AccountID;
		memcpy(Request->SessionIP, Client->Account.SessionIP, MAX_ADDRESSIP_LENGTH);
		Request->SessionTimeout = Client->Account.SessionTimeout;
		SocketSendAll(Server->MasterSocket, Request);

		S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketInit(S2C_DATA_VERIFY_SUBPASSWORD);
		Response->Command = S2C_VERIFY_SUBPASSWORD;
		Response->Success = 1;
		Response->FailureCount = Client->SubpasswordFailureCount;
		Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
		return SocketSend(Socket, Connection, Response);
	}
	*/

	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	if (!IsSubpasswordSet || strlen(Packet->Password) < MIN_SUBPASSWORD_LENGTH ||
		memcmp(Client->Account.CharacterPassword, Packet->Password, MAX_SUBPASSWORD_LENGTH) != 0) {
		Client->SubpasswordFailureCount += 1;
		if (Client->SubpasswordFailureCount >= Server->Config->WorldSvr.MaxSubpasswordFailureCount) {
			// TODO: Ban account based on configuration due to max failure count reach!
		}

		S2C_DATA_VERIFY_CREDENTIALS_SUBPASSWORD* Response = PacketInit(S2C_DATA_VERIFY_CREDENTIALS_SUBPASSWORD);
		Response->Command = S2C_VERIFY_CREDENTIALS_SUBPASSWORD;
		Response->Success = 0;
		return SocketSend(Socket, Connection, Response);
	}

	return OnVerifyCharacterSubpassword(
		Server,
		Socket,
		Client,
		Connection,
		Packet
	);

	S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketInit(S2C_DATA_VERIFY_SUBPASSWORD);
	Response->Command = S2C_VERIFY_SUBPASSWORD;
	Response->Success = 0;
	Response->FailureCount = 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
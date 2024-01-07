#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldUpdateAccountSubpasswordData, IPC_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA, IPC_DATA_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA) {
	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Server->Database, &Account)) return;

	memcpy(Account.CharacterPassword, Packet->CharacterPassword, MAX_SUBPASSWORD_LENGTH);
	Account.CharacterQuestion = Packet->CharacterQuestion;
	memcpy(Account.CharacterAnswer, Packet->CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);

	MasterDBUpdateAccountCharacterPassword(Server->Database, &Account);
}
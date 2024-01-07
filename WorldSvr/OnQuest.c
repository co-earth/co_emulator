#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(QUEST_BEGIN) {
	S2C_DATA_QUEST_BEGIN* Response = PacketInit(S2C_DATA_QUEST_BEGIN);
	Response->Command = S2C_QUEST_BEGIN;
	Response->Result = 0;

	if (!Character) goto error;

	if (RTCharacterQuestBegin(Runtime, Character, Packet->QuestID, Packet->SlotID)) {
		Response->Result = 1;
	}

	return SocketSend(Socket, Connection, Response);

error:
	Response->Result = 0;
	return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(QUEST_CLEAR) {
	S2C_DATA_QUEST_CLEAR* Response = PacketInit(S2C_DATA_QUEST_CLEAR);
	Response->Command = S2C_QUEST_CLEAR;
	Response->Result = 0;

	if (!Character) goto error;

	UInt64 RewardExp;
	UInt32 RewardSkillExp;
	if (RTCharacterQuestClear(Runtime, Character, Packet->QuestID, Packet->SlotID, Packet->RewardItemIndex, Packet->InventorySlotIndex, &RewardExp, &RewardSkillExp)) {
		Response->Result = 1;
		Response->ResultType = S2C_DATA_QUEST_CLEAR_RESULT_TYPE_SUCCESS;
		Response->RewardExp = RewardExp;
	}

	SocketSend(Socket, Connection, Response);
	/*
	RTQuestDataRef QuestData = RTRuntimeGetQuestByIndex(Runtime, Packet->QuestID);
	
	S2C_DATA_NFY_EVENT_PASS_PROGRESS* Notification1 = PacketInit(S2C_DATA_NFY_EVENT_PASS_PROGRESS);
	Notification1->Command = S2C_NFY_EVENT_PASS_PROGRESS;
	Notification1->GroupID = 3;
	Notification1->MissionID = 12;
	Notification1->Progress = 2;
	SocketSend(Socket, Connection, Notification1);

	S2C_DATA_NFY_EVENT_PASS_RESUME* Notification2 = PacketInit(S2C_DATA_NFY_EVENT_PASS_RESUME);
	Notification2->Command = S2C_NFY_EVENT_PASS_RESUME;
	Notification2->Unknown1 = 0;
	Notification2->Unknown2 = 100;
	Notification2->Unknown3 = 0;
	SocketSend(Socket, Connection, Notification2);
	*/
	return;

error:
	Response->Result = 0;
	return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(QUEST_CANCEL) {
	S2C_DATA_QUEST_CANCEL* Response = PacketInit(S2C_DATA_QUEST_CANCEL);
	Response->Command = S2C_QUEST_CANCEL;

	if (!RTCharacterQuestCancel(Runtime, Character, Packet->QuestID, Packet->SlotID)) goto error;

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_ACTION) {
	if (!Character) goto error;

	Bool Success = true;
	for (Int32 Index = 0; Index < Packet->ActionCount; Index++) {
		Success &= RTCharacterQuestAction(
			Runtime,
			Character,
			Packet->QuestID,
			Packet->NpcSetID,
			Packet->SlotIndex,
			Packet->Actions[Index].ActionIndex,
			Packet->Actions[Index].SlotIndex
		);
	}

	if (!Success) goto error;

	// TODO: Do not access the data directly!
	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[Packet->SlotIndex];

	S2C_DATA_QUEST_ACTION* Response = PacketInit(S2C_DATA_QUEST_ACTION);
	Response->Command = S2C_QUEST_ACTION;
	Response->QuestID = Packet->QuestID;
	Response->NpcFlags = 0;

	for (Int32 Index = 0; Index < QuestSlot->NpcActionIndex; Index++) {
		Response->NpcFlags |= 1 << Index;
	}
    
	// TODO: Set correct npc set id
	Response->NpcSetID = Packet->NpcSetID;

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPDATE_QUEST_LIST) {
    // TODO: Update character quest list data
}
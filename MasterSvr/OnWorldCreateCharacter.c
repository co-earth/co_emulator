#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldCreateCharacter, IPC_WORLD_REQCREATECHARACTER, IPC_DATA_WORLD_REQCREATECHARACTER) {
	IPC_DATA_WORLD_ACKCREATECHARACTER* Response = PacketInit(IPC_DATA_WORLD_ACKCREATECHARACTER);
	Response->Command = IPC_WORLD_ACKCREATECHARACTER;
	Response->ConnectionID = Packet->ConnectionID;

	Int32 NameExists = 0;
	if (!MasterDBSelectCharacterNameCount(Server->Database, Packet->Name, &NameExists)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		return SocketSend(Socket, Connection, Response);
	}

	if (NameExists) {
		Response->Status = CREATE_CHARACTER_STATUS_NAME_OCCUPIED;
		return SocketSend(Socket, Connection, Response);
	}

	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Server->Database, &Account)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		return SocketSend(Socket, Connection, Response);
	}

	MASTERDB_DATA_CHARACTER* Character = (MASTERDB_DATA_CHARACTER*)malloc(sizeof(MASTERDB_DATA_CHARACTER));
	assert(Character);
	memset(Character, 0, sizeof(MASTERDB_DATA_CHARACTER));

	Character->AccountID = Account.AccountID;
	memcpy(Character->Name, Packet->Name, Packet->NameLength);
	Character->Index = Packet->SlotIndex;
	Character->CharacterData = Packet->CharacterData;
    Character->EquipmentData = Packet->CharacterEquipment;
	Character->InventoryData = Packet->CharacterInventory;
    Character->SkillSlotData = Packet->CharacterSkillSlots;
    Character->QuickSlotData = Packet->CharacterQuickSlots;

	if (!MasterDBInsertCharacter(Server->Database, Character)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		free(Character);
		return SocketSend(Socket, Connection, Response);
	}

	Character->CharacterID = DatabaseGetLastInsertID(Server->Database);
	if (!MasterDBSelectCharacterByID(Server->Database, Character)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		free(Character);
		return SocketSend(Socket, Connection, Response);
	}

	Response->Status = CREATE_CHARACTER_STATUS_SUCCESS;
	Response->SlotIndex = Packet->SlotIndex;
	Response->Character.ID = Character->CharacterID;
	Response->Character.CreationDate = Character->CreatedAt;
	Response->Character.Style = Character->CharacterData.Style.RawValue;
	Response->Character.Level = Character->CharacterData.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level;
	Response->Character.OverlordLevel = Character->CharacterData.Level[RUNTIME_CHARACTER_LEVEL_OVERLORD].Level;
	Response->Character.SkillRank = Character->CharacterData.Skill.Rank;
	memcpy(Response->Character.Name, Character->Name, MAX_CHARACTER_NAME_LENGTH);
	Response->Character.HonorPoint = Character->CharacterData.Honor.Point;
	Response->Character.Alz = Character->CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
	Response->Character.MapID = Character->CharacterData.Position.WorldID;
	Response->Character.PositionX = Character->CharacterData.Position.X;
	Response->Character.PositionY = Character->CharacterData.Position.Y;
	Response->Character.EquipmentCount = Character->EquipmentData.Count;
	memcpy(Response->Character.Equipment, Character->EquipmentData.Slots, sizeof(struct _RTItemSlot) * Character->EquipmentData.Count);
	
	free(Character);
	return SocketSend(Socket, Connection, Response);
}
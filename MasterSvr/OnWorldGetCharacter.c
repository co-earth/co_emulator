#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldGetCharacter, IPC_WORLD_REQGETCHARACTER, IPC_DATA_WORLD_REQGETCHARACTER) {
	IPC_DATA_WORLD_ACKGETCHARACTER* Response = PacketInit(IPC_DATA_WORLD_ACKGETCHARACTER);
	Response->Command = IPC_WORLD_ACKGETCHARACTER;
	Response->ConnectionID = Packet->ConnectionID;

	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Server->Database, &Account)) goto error;

	MASTERDB_DATA_CHARACTER Character = { 0 };
	Character.CharacterID = Packet->CharacterID;
	if (!MasterDBSelectCharacterByID(Server->Database, &Character)) goto error;

	if (Character.AccountID != Account.AccountID) goto error;

	Response->Success = true;
    Response->CharacterIndex = Packet->CharacterIndex;
    Response->Character.ID = Character.CharacterID;
    Response->Character.Index = Character.Index;
    Response->Character.CreationDate = Character.CreatedAt;

    memcpy(Response->Character.Name, Character.Name, MAX_CHARACTER_NAME_LENGTH);
    memcpy(&Response->Character.CharacterData, &Character.CharacterData, sizeof(struct _RTCharacterInfo));
    memcpy(&Response->Character.EquipmentData, &Character.EquipmentData, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&Response->Character.InventoryData, &Character.InventoryData, sizeof(struct _RTCharacterInventoryInfo));
    memcpy(&Response->Character.SkillSlotData, &Character.SkillSlotData, sizeof(struct _RTCharacterSkillSlotInfo));
    memcpy(&Response->Character.QuickSlotData, &Character.QuickSlotData, sizeof(struct _RTCharacterQuickSlotInfo));
    memcpy(&Response->Character.QuestSlotData, &Character.QuestSlotData, sizeof(struct _RTCharacterQuestSlotInfo));
    memcpy(&Response->Character.QuestFlagData, &Character.QuestFlagData, sizeof(struct _RTCharacterQuestFlagInfo));
    memcpy(&Response->Character.AchievementData, &Character.AchievementData, sizeof(GAME_DATA_CHARACTER_ACHIEVEMENT));
    memcpy(&Response->Character.EssenceAbilityData, &Character.EssenceAbilityData, sizeof(struct _RTCharacterEssenceAbilityInfo));
    memcpy(&Response->Character.BlendedAbilityData, &Character.BlendedAbilityData, sizeof(GAME_DATA_CHARACTER_BLENDEDABILITY));
    memcpy(&Response->Character.HonorMedalData, &Character.HonorMedalData, sizeof(GAME_DATA_CHARACTER_HONORMEDAL));
    memcpy(&Response->Character.OverlordData, &Character.OverlordData, sizeof(GAME_DATA_CHARACTER_OVERLORD));
    memcpy(&Response->Character.TransformData, &Character.TransformData, sizeof(GAME_DATA_CHARACTER_TRANSFORM));
    memcpy(&Response->Character.TranscendenceData, &Character.TranscendenceData, sizeof(GAME_DATA_CHARACTER_TRANSCENDENCE));
    memcpy(&Response->Character.MercenaryData, &Character.MercenaryData, sizeof(GAME_DATA_CHARACTER_MERCENARY));
    memcpy(&Response->Character.CraftData, &Character.CraftData, sizeof(GAME_DATA_CHARACTER_CRAFT));

	return SocketSend(Socket, Connection, Response);

error:
	Response->Success = false;

	return SocketSend(Socket, Connection, Response);
}
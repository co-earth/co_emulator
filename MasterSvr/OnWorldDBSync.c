#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"
#include "Notification.h"

IPC_PROCEDURE_BINDING(OnWorldDBSync, IPC_WORLD_REQDBSYNC, IPC_DATA_WORLD_REQDBSYNC) {
	IPC_DATA_WORLD_ACKDBSYNC* Response = PacketInit(IPC_DATA_WORLD_ACKDBSYNC);
	Response->Command = IPC_WORLD_ACKDBSYNC;
	Response->ConnectionID = Packet->ConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->CharacterID = Packet->CharacterID;
	Response->DBSyncMaskFailure = 0;

	Int32 DataOffset = 0;

	if (Packet->DBSyncMask & RUNTIME_CHARACTER_SYNC_INFO) {
		RTCharacterInfoRef CharacterInfo = (RTCharacterInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterInfo);

		Bool Success = MasterDBUpdateCharacterInfo(
			Server->Database,
			Packet->CharacterID,
			CharacterInfo
		);

		if (!Success) {
			Response->DBSyncMaskFailure |= RUNTIME_CHARACTER_SYNC_INFO;
		}
	}

    if (Packet->DBSyncMask & RUNTIME_CHARACTER_SYNC_EQUIPMENT) {
		RTCharacterEquipmentInfoRef EquipmentInfo = (RTCharacterEquipmentInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterEquipmentInfo);

		Bool Success = MasterDBUpdateCharacterEquipmentData(
			Server->Database,
			Packet->CharacterID,
			EquipmentInfo
		);

		if (!Success) {
			Response->DBSyncMaskFailure |= RUNTIME_CHARACTER_SYNC_EQUIPMENT;
		}
    }

    if (Packet->DBSyncMask & RUNTIME_CHARACTER_SYNC_INVENTORY) {
		RTCharacterInventoryInfoRef InventoryInfo = (RTCharacterInventoryInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterInventoryInfo);

		Bool Success = MasterDBUpdateCharacterInventoryData(
			Server->Database,
			Packet->CharacterID,
			InventoryInfo
		);

		if (!Success) {
			Response->DBSyncMaskFailure |= RUNTIME_CHARACTER_SYNC_INVENTORY;
		}
    }

    if (Packet->DBSyncMask & RUNTIME_CHARACTER_SYNC_SKILLSLOT) {
		RTCharacterSkillSlotInfoRef SkillSlotInfo = (RTCharacterSkillSlotInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterSkillSlotInfo);

		Bool Success = MasterDBUpdateCharacterSkillSlotData(
			Server->Database,
			Packet->CharacterID,
			SkillSlotInfo
		);

		if (!Success) {
			Response->DBSyncMaskFailure |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
		}
    }

    if (Packet->DBSyncMask & RUNTIME_CHARACTER_SYNC_QUICKSLOT) {
		RTCharacterQuickSlotInfoRef QuickSlotInfo = (RTCharacterQuickSlotInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterQuickSlotInfo);

		Bool Success = MasterDBUpdateCharacterQuickSlotData(
			Server->Database,
			Packet->CharacterID,
			QuickSlotInfo
		);

		if (!Success) {
			Response->DBSyncMaskFailure |= RUNTIME_CHARACTER_SYNC_QUICKSLOT;
		}
    }

    if (Packet->DBSyncMask & RUNTIME_CHARACTER_SYNC_QUESTSLOT) {
		RTCharacterQuestSlotInfoRef QuestSlotInfo = (RTCharacterQuestSlotInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterQuestSlotInfo);

		Bool Success = MasterDBUpdateCharacterQuestSlotData(
			Server->Database,
			Packet->CharacterID,
			QuestSlotInfo
		);

		if (!Success) {
			Response->DBSyncMaskFailure |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
		}
    }

    if (Packet->DBSyncMask & RUNTIME_CHARACTER_SYNC_QUESTFLAG) {
		RTCharacterQuestFlagInfoRef QuestFlagInfo = (RTCharacterQuestFlagInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterQuestFlagInfo);

		Bool Success = MasterDBUpdateCharacterQuestFlagData(
			Server->Database,
			Packet->CharacterID,
			QuestFlagInfo
		);

		if (!Success) {
			Response->DBSyncMaskFailure |= RUNTIME_CHARACTER_SYNC_QUESTFLAG;
		}
    }
}
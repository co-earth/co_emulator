#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"
#include "Util.h"

#define RUNTIME_CREATE_DEBUG_CHARACTER 1

CLIENT_PROCEDURE_BINDING(CREATE_CHARACTER) {
	S2C_DATA_CREATE_CHARACTER* Response = PacketInit(S2C_DATA_CREATE_CHARACTER);
	
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) {
		return SocketDisconnect(Socket, Connection);
	}

	// TODO: CharacterSlotFlags is not set correctly?!
	if (!(Client->Account.CharacterSlotFlags & (1 << Packet->SlotIndex)) ||
		Client->Characters[Packet->SlotIndex].ID > 0) {
		Response->Status = CREATE_CHARACTER_STATUS_NOT_ALLOWED;
		return SocketSend(Socket, Connection, Response);
	}

	if (Packet->NameLength < MIN_CHARACTER_NAME_LENGTH ||
		Packet->NameLength > MAX_CHARACTER_NAME_LENGTH) {
		Response->Status = CREATE_CHARACTER_STATUS_NAME_VALIDATION_FAILED;
		return SocketSend(Socket, Connection, Response);
	}

	UInt32 RawStyle = SwapUInt32(Packet->Style);
	union _RTCharacterStyle Style = *((union _RTCharacterStyle*)&RawStyle);
	UInt32 BattleStyleIndex = Style.BattleStyle | (Style.ExtendedBattleStyle << 3);

	if (Style.Padding1 > 0 ||
		Style.Padding2 > 0 ||
		Style.Padding3 > 0 ||
		Style.Padding4 > 0 ||
		Style.Unknown1 < 1 ||
		Style.AuraCode > 0 ||
		Style.ShowHelmed < 1 ||
		Style.HairStyle >= MAX_CHARACTER_NORMAL_HAIR_COUNT ||
		BattleStyleIndex < RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN ||
		BattleStyleIndex > RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX ||
		Style.Face >= MAX_CHARACTER_NORMAL_FACE_COUNT ||
		Style.HairColor >= MAX_CHARACTER_NORMAL_HAIR_COLOR_COUNT ||
		Style.BattleRank > 1) {
		Response->Status = CREATE_CHARACTER_STATUS_NOT_ALLOWED;
		return SocketSend(Socket, Connection, Response);
	}

#if (RUNTIME_CREATE_DEBUG_CHARACTER)
	Style.BattleRank = 10;
#endif

	IPC_DATA_WORLD_REQCREATECHARACTER* Request = PacketInit(IPC_DATA_WORLD_REQCREATECHARACTER);
	Request->Command = IPC_WORLD_REQCREATECHARACTER;
	Request->ConnectionID = Connection->ID;
	Request->AccountID = Client->Account.AccountID;
	Request->SlotIndex = Packet->SlotIndex;
	Request->NameLength = Packet->NameLength;
	memcpy(Request->Name, Packet->Name, Packet->NameLength);

	struct _RuntimeDataCharacterTemplate* CharacterTemplate = &Server->RuntimeData->CharacterTemplate[BattleStyleIndex - 1];
	if (CharacterTemplate->BattleStyleIndex != BattleStyleIndex) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		return SocketSend(Socket, Connection, Response);
	}


	struct _RTCharacterInfo {
		union _RTCharacterStyle Style;
		struct _RTCharacterLevel Level[RUNTIME_CHARACTER_LEVEL_COUNT];
		struct _RTCharacterHonor Honor;
		struct _RTCharacterAbility Ability;
		struct _RTCharacterSkill Skill;
		struct _RTCharacterResource Resource;
		UInt16 Stat[RUNTIME_CHARACTER_STAT_COUNT];
		UInt64 Currency[RUNTIME_CHARACTER_CURRENCY_COUNT];
		struct _RTCharacterPosition Position;
	};
	
	Request->CharacterData.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level = 1;
	Request->CharacterData.Skill.Rank = 1;
	Request->CharacterData.Skill.Level = 0;
	Request->CharacterData.Style = Style;

	RTDataCharacterInitRef CharacterInit = RTRuntimeDataCharacterInitGet(&Server->Runtime->Context, BattleStyleIndex);

	Request->CharacterData.Position.WorldID = CharacterInit->WorldID;
	Request->CharacterData.Position.X = CharacterInit->X;
	Request->CharacterData.Position.Y = CharacterInit->Y;

	if (CharacterInit->Suit > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->Suit;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->Glove > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->Glove;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->Boot > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->Boot;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->RightHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->RightHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT;

		Request->CharacterEquipment.Count += 1;
	}

	if (CharacterInit->LeftHand > 0) {
		RTItemSlotRef ItemSlot = &Request->CharacterEquipment.Slots[Request->CharacterEquipment.Count];
		ItemSlot->Item.ID = CharacterInit->LeftHand;
		ItemSlot->SlotIndex = RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT;

		Request->CharacterEquipment.Count += 1;
	}

	RTDataCharacterInitStatRef CharacterInitStat = RTRuntimeDataCharacterInitStatGet(&Server->Runtime->Context, BattleStyleIndex);
	Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_STR] = CharacterInitStat->Str;
	Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_DEX] = CharacterInitStat->Dex;
	Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_INT] = CharacterInitStat->Int;

	memcpy(&Request->CharacterInventory, &CharacterTemplate->Inventory, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Request->CharacterSkillSlots, &CharacterTemplate->SkillSlots, sizeof(struct _RTCharacterSkillSlotInfo));
	memcpy(&Request->CharacterQuickSlots, &CharacterTemplate->QuickSlots, sizeof(struct _RTCharacterQuickSlotInfo));

#if (RUNTIME_CREATE_DEBUG_CHARACTER)
	Request->CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] = 999999999;
	Request->CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_GEM] = 999999;
	Request->CharacterData.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level = 200;
	Request->CharacterData.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Exp = RTRuntimeGetExpByLevel(Runtime, 200);
	Request->CharacterData.Stat[RUNTIME_CHARACTER_STAT_PNT] = 200 * 5;

	RTDataHonorLevelFormulaRef HonorLevelFormula = RTRuntimeDataHonorLevelFormulaGet(Runtime->Context, 20);
	Request->CharacterData.Honor.Rank = HonorLevelFormula->Rank;
	Request->CharacterData.Honor.Exp = HonorLevelFormula->MaxPoint;
#endif

	return SocketSendAll(Server->MasterSocket, Request);
}

IPC_PROCEDURE_BINDING(OnWorldCreateCharacter, IPC_WORLD_ACKCREATECHARACTER, IPC_DATA_WORLD_ACKCREATECHARACTER) {
	if (!ClientConnection || !Client) goto error;

	if (Packet->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		Client->Characters[Packet->SlotIndex] = Packet->Character;
	}
	
	S2C_DATA_CREATE_CHARACTER* Response = PacketInit(S2C_DATA_CREATE_CHARACTER);
	Response->Command = S2C_CREATE_CHARACTER;
	Response->CharacterIndex = Packet->Character.ID * MAX_CHARACTER_COUNT + Packet->SlotIndex;
	Response->Status = Packet->Status;
	return SocketSend(Server->ClientSocket, ClientConnection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
#include "Character.h"
#include "Item.h"
#include "Inventory.h"
#include "Runtime.h"

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemStub) {
	return false;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemPotion) {
	/* TODO: Send notification event in character!
	S2C_DATA_CHARACTER_UPDATE* Notification = PacketInit(S2C_DATA_CHARACTER_UPDATE);
	Notification->Command = S2C_CHARACTER_UPDATE;

	switch (ItemData->Options[0]) {
	case RUNTIME_ITEM_POTION_TYPE_HP:
		RTCharacterAddHP(Runtime, Character, ItemData->Options[4]);
		Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_HPPOTION;
		Notification->HP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		break;

	case RUNTIME_ITEM_POTION_TYPE_MP:
		RTCharacterAddMP(Runtime, Character, ItemData->Options[4]);
		Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_MPPOTION;
		Notification->MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		break;

	case RUNTIME_ITEM_POTION_TYPE_SP:
		break;

	default:
		goto error;
	}
	*/

	switch (ItemData->Options[0]) {
		case RUNTIME_ITEM_SUBTYPE_POTION_HP:
			RTCharacterAddHP(Runtime, Character, ItemData->Options[4]);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_MP:
			RTCharacterAddMP(Runtime, Character, ItemData->Options[4]);
			break;

		case RUNTIME_ITEM_SUBTYPE_POTION_SP:
			RTCharacterAddSP(Runtime, Character, ItemData->Options[4]);
			break;

		default:
			return false;
	}

	if (ItemData->MaxStackSize > 0) {
		ItemSlot->ItemOptions = MAX(0, ItemSlot->ItemOptions - 1);
		if (ItemSlot->ItemOptions < 1) {
			RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

			Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
			Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
		}
	}
	else {
		// TODO: Do not delete inexhaustable items!!!
		RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
	}

	return true;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSkillBook) {
    struct {
        UInt8 SkillSlotIndex;
    } *Data = Payload;

    RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, ItemData->Options[0]);
    if (!SkillData) return false;

    // TODO: Check if character is allowed to learn the skill by battle style and skill ranks...

    RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Data->SkillSlotIndex);
    if (SkillSlot) return false;

    // TODO: Check if ItemOptions begins at 0 for Level 1

    Int32 SkillLevel = MAX(1, ItemSlot->ItemOptions);
    SkillSlot = RTCharacterAddSkillSlot(Runtime, Character, SkillData->SkillID, SkillLevel, Data->SkillSlotIndex);

    RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

    Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
    Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    return true;
}

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemImmediateReward) {
	switch (ItemData->Options[0]) {
	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_ALZ:
		RTCharacterAddCurrency(Runtime, Character, RUNTIME_CHARACTER_CURRENCY_ALZ, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_EXP:
		RTCharacterAddExp(Runtime, Character, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_SKILLEXP:
		RTCharacterAddSkillExp(Runtime, Character, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_PETEXP:
		UNIMPLEMENTED;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_GUILDEXP:
		UNIMPLEMENTED;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_HONOR:
		RTCharacterAddHonorPoint(Runtime, Character, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_AXP:
		RTCharacterAddAbilityExp(Runtime, Character, ItemSlot->ItemOptions);
		break;

	case RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_WAREXP:
		UNIMPLEMENTED;

	default:
		return false;
	}

	RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    return true;
}
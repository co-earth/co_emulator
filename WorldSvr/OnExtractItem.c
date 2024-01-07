#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(EXTRACT_ITEM) {
	if (!Character) {
		return SocketDisconnect(Socket, Connection);
	}

	S2C_DATA_EXTRACT_ITEM* Response = PacketInit(S2C_DATA_EXTRACT_ITEM);
	Response->Command = S2C_EXTRACT_ITEM;
	Response->Result = 1;
	Response->ItemCount = 0;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) goto error;

	Int32 Seed = GetTickCount64();

	for (Int32 DestroyItemIndex = 0; DestroyItemIndex < Runtime->Context->DestroyItemCount; DestroyItemIndex += 1) {
		RTDataDestroyItemRef ItemPattern = &Runtime->Context->DestroyItemList[DestroyItemIndex];
		if (ItemPattern->ItemIndex != ItemSlot->Item.ID) continue;
		if (ItemPattern->MinEnchantLevel > ItemSlot->Item.UpgradeLevel) continue;
		if (ItemPattern->MaxEnchantLevel < ItemSlot->Item.UpgradeLevel) continue;
		if (ItemPattern->Slot != -1) continue; // TODO: Add support for slot count checking
		if (ItemPattern->Epic != -1) {
			// TODO: This is only for debugging as ItemOptions can be > 0 with no epic!
			if (ItemPattern->Epic == 0 && ItemSlot->ItemOptions != 0) continue;
			if (ItemPattern->Epic > 0) continue; // TODO: Add support for epic level checking
		}

		if (ItemPattern->EpicOption != -1) continue; // TODO: Add support for epic option checking
		if (ItemPattern->AccountBind > 0 && !ItemSlot->Item.IsAccountBinding) continue;
		if (ItemPattern->CharacterBind > 0 && !ItemSlot->Item.IsCharacterBinding) continue;
		
		// TODO: Add support for alert

		RTDataDestroyItemPoolRef ItemPool = RTRuntimeDataDestroyItemPoolGet(&Runtime->Context, ItemPattern->PoolID);
		if (!ItemPool) continue;

		for (Int32 ItemResultIndex = 0; ItemResultIndex < ItemPool->DestroyItemPoolResultCount; ItemResultIndex += 1) {
			RTDataDestroyItemPoolResultRef ItemResult = &ItemPool->DestroyItemPoolResultList[ItemResultIndex];
			Int32 ItemCount = RandomRange(&Seed, ItemResult->MinCount, ItemResult->MaxCount);
			for (Int32 Index = 0; Index < ItemCount; Index += 1) {
				struct _RTItemSlot Slot = { 0 };
				Slot.Item.ID = ItemResult->ItemKind;
				Slot.SlotIndex = Character->TemporaryInventoryInfo.Count;

				if (RTInventorySetSlot(Runtime, &Character->TemporaryInventoryInfo, &Slot)) {
					S2C_EXTRACT_ITEM_SLOT_INDEX *ResponseItemSlot = PacketAppendStruct(S2C_EXTRACT_ITEM_SLOT_INDEX);
					ResponseItemSlot->ItemID = Slot.Item.Serial;
					ResponseItemSlot->ItemOption = Slot.ItemOptions;
					ResponseItemSlot->InventorySlotIndex = RUNTIME_INVENTORY_TOTAL_SIZE + Slot.SlotIndex;
					Response->ItemCount += 1;
				}
			}
		}
	}

	return SocketSend(Socket, Connection, Response);

error:
	Response->Command = S2C_EXTRACT_ITEM;
	Response->Result = 0;
	Response->ItemCount = 0;
	return SocketSend(Socket, Connection, Response);
}

#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ADD_FORCE_SLOT_OPTION) {
	if (!Character) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->ItemSlotIndex);
	if (!ItemSlot) goto error;

	// TODO: Implementation is missing

	S2C_DATA_ADD_FORCE_SLOT_OPTION* Response = PacketInit(S2C_DATA_ADD_FORCE_SLOT_OPTION);
	Response->Command = S2C_ADD_FORCE_SLOT_OPTION;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPGRADE_ITEM_LEVEL) {
	if (!Character) goto error;

    RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;

	// TODO: Check if item type is upgradable!
	// TODO: Check which upgrade code type is required!

	Int32 CoreCount = RTUpgradeLevelNormalGetCoreCount(Runtime, ItemSlot);
	assert(CoreCount <= RUNTIME_UPGRADE_MAX_CORE_COUNT);
	Int32 CoreItemSlotCount = 0;
	struct _RTItemSlot CoreItemSlots[RUNTIME_UPGRADE_MAX_CORE_COUNT] = { 0 };
	RTInventoryFindItems(
		Runtime,
		&Character->InventoryInfo,
		33556422,
		CoreCount,
		&CoreItemSlotCount,
		&CoreItemSlots
	);

	UInt8 Result = RUNTIME_UPGRADE_RESULT_ERROR;
	UInt64 ItemID = 0;
	UInt64 ItemOption = 0;

	if (CoreItemSlotCount >= CoreCount) {
		// TODO: Take snapshot of inventory and revert changes on error!
		Bool ConsumedCoreItems = true;
		for (Int32 Index = 0; Index < CoreItemSlotCount; Index++) {
			ConsumedCoreItems &= RTInventoryClearSlot(
				Runtime,
				&Character->InventoryInfo,
				CoreItemSlots[Index].SlotIndex
			);
		}

		if (ConsumedCoreItems) {
			Int32 Seed = GetTickCount64();
			Int32 UpgradePoint = Client->UpgradePoint;

			Result = RTUpgradeLevelNormal(
				Runtime,
				ItemSlot,
				&UpgradePoint,
				&Seed
			);
			ItemID = ItemSlot->Item.Serial;
			ItemOption = ItemSlot->ItemOptions;

			Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
			Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

			Client->UpgradePoint = UpgradePoint;
		}
	}

	Int32 Seed = GetTickCount64();
	S2C_DATA_UPDATE_UPGRAGE_POINTS* Notification = PacketInit(S2C_DATA_UPDATE_UPGRAGE_POINTS);
	Notification->Command = 2506;
	Notification->UpgradePoint = Client->UpgradePoint;
	Notification->Timestamp = GetTimestamp() + (Timestamp)1000 * 60 * 60;
	SocketSend(Socket, Connection, Notification);

	S2C_DATA_UPGRADE_ITEM_LEVEL* Response = PacketInit(S2C_DATA_UPGRADE_ITEM_LEVEL);
	Response->Command = S2C_UPGRADE_ITEM_LEVEL;
	Response->Result = Result;
	Response->ItemID = ItemID;
	Response->ItemOption = ItemOption;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

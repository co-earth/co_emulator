#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ADD_PASSIVE_ABILITY) {
	if (!Character) goto error;

	S2C_DATA_ADD_PASSIVE_ABILITY* Response = PacketInit(S2C_DATA_ADD_PASSIVE_ABILITY);
	Response->Command = S2C_ADD_PASSIVE_ABILITY;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->InventorySlotIndex);
	if (ItemSlot) {
		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (ItemData && ItemData->ItemType == RUNTIME_ITEM_TYPE_ABILITY_RUNE) {
			RTDataPassiveAbilityCostRef AbilityCost = RTRuntimeDataPassiveAbilityCostGet(
                Runtime->Context, 
                ItemSlot->Item.ID
            );
			assert(AbilityCost);

			Response->Result = 1;
		}
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPGRADE_PASSIVE_ABILITY) {
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(REMOVE_PASSIVE_ABILITY) {
	return SocketDisconnect(Socket, Connection);
}
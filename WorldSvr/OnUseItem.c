#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(USE_ITEM) {
	if (!Character) goto error;

	S2C_DATA_USE_ITEM* Response = PacketInit(S2C_DATA_USE_ITEM);
	Response->Command = S2C_USE_ITEM;
	Response->Result = 1;
	
	Bool Success = false;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->InventorySlotIndex);
	if (ItemSlot) {
		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (!ItemData) goto error;

		#define RUNTIME_ITEM_PROCEDURE(__NAME__, __TYPE__)		\
		if (ItemData->ItemType == __TYPE__) {					\
			Success = __NAME__(									\
				Runtime,										\
				Character,										\
				ItemSlot,										\
				ItemData,										\
				Packet->Data									\
			);													\
		}
		#include <RuntimeLib/ItemProcDefinition.h>
	}

	if (Success) {
		Response->Result = 0;
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

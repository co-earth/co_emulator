#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

static struct _RTCharacterEquipmentInfo kEquipmentInfoBackup;
static struct _RTCharacterInventoryInfo kInventoryInfoBackup;

CLIENT_PROCEDURE_BINDING(MOVE_INVENTORY_ITEM) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
    }

    // TODO: Check if this is causing an issue when the client is not initialized!
    memcpy(&kEquipmentInfoBackup, &Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&kInventoryInfoBackup, &Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));

    S2C_DATA_MOVE_INVENTORY_ITEM* Response = PacketInit(S2C_DATA_MOVE_INVENTORY_ITEM);
    Response->Command = S2C_MOVE_INVENTORY_ITEM;
    Response->Result = 0;

    if (Packet->Source.StorageType == STORAGE_TYPE_INVENTORY &&
        Packet->Destination.StorageType == STORAGE_TYPE_EQUIPMENT) {
        RTItemSlotRef Slot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->Source.Index);
        if (!Slot) goto error;

        struct _RTItemSlot SlotCopy = *Slot;
        if (!RTInventoryClearSlot(Runtime, &Character->InventoryInfo, Packet->Source.Index)) goto error;

        SlotCopy.SlotIndex = Packet->Destination.Index;
        if (!RTEquipmentSetSlot(Runtime, &Character->EquipmentInfo, &SlotCopy)) goto error;

        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_EQUIPMENT;
        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
        Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

        Response->Result = 1;
    }
    else if (Packet->Source.StorageType == STORAGE_TYPE_EQUIPMENT &&
        Packet->Destination.StorageType == STORAGE_TYPE_INVENTORY) {
        RTItemSlotRef Slot = RTEquipmentGetSlot(Runtime, &Character->EquipmentInfo, Packet->Source.Index);
        if (!Slot) goto error;

        struct _RTItemSlot SlotCopy = *Slot;
        if (!RTEquipmentClearSlot(Runtime, &Character->EquipmentInfo, Packet->Source.Index)) goto error;

        SlotCopy.SlotIndex = Packet->Destination.Index;
        if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, &SlotCopy)) goto error;

        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_EQUIPMENT;
        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
        Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

        Response->Result = 1;
    }
    else if (Packet->Source.StorageType == STORAGE_TYPE_INVENTORY &&
        Packet->Destination.StorageType == STORAGE_TYPE_INVENTORY) {
        if (!RTInventoryMoveSlot(Runtime, &Character->InventoryInfo, &Character->InventoryInfo, Packet->Source.Index, Packet->Destination.Index)) goto error;

        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
        Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

        Response->Result = 1;
    }
    else {
        goto error;
    }

    return SocketSend(Socket, Connection, Response);

error:
    memcpy(&Character->EquipmentInfo, &kEquipmentInfoBackup, sizeof(struct _RTCharacterEquipmentInfo));
    memcpy(&Character->InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));

    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SORT_INVENTORY) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
    }

    S2C_DATA_SORT_INVENTORY* Response = PacketInit(S2C_DATA_SORT_INVENTORY);
    Response->Command = S2C_SORT_INVENTORY;
    Response->Success = 0;
    return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(MOVE_INVENTORY_ITEM_LIST) {
    if (!Character) {
        return SocketDisconnect(Socket, Connection);
    }

    S2C_DATA_SORT_INVENTORY* Response = PacketInit(S2C_DATA_SORT_INVENTORY);
    Response->Command = S2C_SORT_INVENTORY;
    Response->Success = 0;

    Int32 TailLength = Packet->ItemCount * sizeof(CSC_DATA_ITEM_SLOT_INDEX) * 2;
    Int32 PacketLength = sizeof(C2S_DATA_MOVE_INVENTORY_ITEM_LIST) + TailLength;
    if (Packet->Signature.Length != PacketLength) goto error;

    CSC_DATA_ITEM_SLOT_INDEX* Source = (CSC_DATA_ITEM_SLOT_INDEX*)(&Packet->Data[0]);
    CSC_DATA_ITEM_SLOT_INDEX* Destination = (CSC_DATA_ITEM_SLOT_INDEX*)(&Packet->Data[0] + Packet->ItemCount * sizeof(CSC_DATA_ITEM_SLOT_INDEX));

    for (Int32 Index = 0; Index < Packet->ItemCount; Index += 1) {
        if (Source[Index].StorageType != STORAGE_TYPE_INVENTORY) goto error;
        if (Destination[Index].StorageType != STORAGE_TYPE_INVENTORY) goto error;

        Int32 SourceIndex = Source[Index].Index;
        RTCharacterInventoryInfoRef SourceInventory = &Character->InventoryInfo;
        if (SourceIndex >= RUNTIME_INVENTORY_TOTAL_SIZE) {
            SourceIndex -= RUNTIME_INVENTORY_TOTAL_SIZE;
            SourceInventory = &Character->TemporaryInventoryInfo;
            if (SourceIndex >= RUNTIME_INVENTORY_TOTAL_SIZE) {
                goto error;
            }
        }

        Bool Success = RTInventoryMoveSlot(
            Runtime,
            SourceInventory,
            &Character->InventoryInfo,
            SourceIndex,
            Destination[Index].Index
        );

        if (!Success) goto error;
    }

    Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
    Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    Response->Success = 1;
    return SocketSend(Socket, Connection, Response);

error:
    Response->Success = 0;
    return SocketSend(Socket, Connection, Response);
}

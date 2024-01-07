#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

CLIENT_PROCEDURE_BINDING(LOOT_INVENTORY_ITEM) {
    if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (!World) goto error;

    RTWorldItemRef Item = RTWorldGetItem(Runtime, World, Packet->Entity, Packet->UniqueKey);
    if (!Item) goto error;

    S2C_DATA_LOOT_INVENTORY_ITEM* Response = PacketInit(S2C_DATA_LOOT_INVENTORY_ITEM);
    Response->Command = S2C_LOOT_INVENTORY_ITEM;

    // TODO: Check character distance to item (S2C_DATA_LOOT_RESULT_OUTOFRANGE_ERROR)
    // TODO: Check item ownership (S2C_DATA_LOOT_RESULT_OWNERSHIP_ERROR)

    struct _RTItemSlot Slot = { 0 };
    Slot.Item = Item->Item;
    Slot.ItemOptions = Item->ItemOptions;
    Slot.SlotIndex = Packet->InventorySlotIndex;
    if (RTInventorySetSlot(Runtime, &Character->InventoryInfo, &Slot)) {
        Response->Result = S2C_DATA_LOOT_RESULT_SUCCESS;
        Response->ItemID = Slot.Item.Serial;
        Response->ItemOptions = Slot.ItemOptions;
        Response->InventorySlotIndex = Packet->InventorySlotIndex;
    } else {
        Response->Result = S2C_DATA_LOOT_RESULT_SLOTINUSE_ERROR;
    }

    Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
    Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    RTWorldDespawnItem(Runtime, World, Item);

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(LOOT_CURRENCY_ITEM) {
    if (!Character) goto error;

    Int32 PacketLength = sizeof(C2S_DATA_LOOT_CURRENCY_ITEM) + sizeof(C2S_DATA_LOOT_CURRENCY_ITEM_INDEX) * Packet->Count;
    if (Packet->Signature.Length != PacketLength) goto error;

    RTWorldRef World = RTRuntimeGetWorldByID(Runtime, Character->Info.Position.WorldID);
    if (!World) goto error;

    S2C_DATA_LOOT_CURRENCY_ITEM* Response = PacketInit(S2C_DATA_LOOT_CURRENCY_ITEM);
    Response->Command = S2C_LOOT_CURRENCY_ITEM;
    Response->Result = S2C_DATA_LOOT_RESULT_SUCCESS;

    for (Int32 Index = 0; Index < Packet->Count; Index++) {
        C2S_DATA_LOOT_CURRENCY_ITEM_INDEX* ItemIndex = &Packet->Data[Index];
        RTWorldItemRef Item = RTWorldGetItem(Runtime, World, ItemIndex->Entity, ItemIndex->UniqueKey);
        if (!Item) continue; // NOTE: Is it fine to just continue here?

        // TODO: Check character distance to item?
        
        if (Item->Item.ID != RUNTIME_ITEM_ID_CURRENCY) continue;

        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] += Item->ItemOptions;

        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
        Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

        Response->Count += 1;

        RTWorldDespawnItem(Runtime, World, Item);
    }

    Response->Currency = Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
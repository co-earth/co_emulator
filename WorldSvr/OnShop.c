#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(BUY_ITEM) {
    if (!Character) goto error;

    // TODO: Add packet bounds checking for dynamically appended data!
    if (Packet->InventoryIndexCount < 1 || Packet->InventoryIndexCount > SERVER_CHARACTER_MAX_BUY_ITEM_COUNT) goto error;

    RTShopDataRef Shop = RTRuntimeGetShopByWorldNpcID(
        Runtime,
        Character->Info.Position.WorldID,
        Packet->NpcID
    );
    if (!Shop) goto error;

    // TODO: Check NPC distance to character
    // TODO: Check and subtract item prices

    Int32 ItemIndex = -1;
    for (Int32 Index = 0; Index < Shop->ItemCount; Index++) {
        RTShopItemDataRef Item = &Shop->Items[Index];
        if (Item->SlotID == Packet->SlotIndex &&
            Item->ItemID == Packet->ItemID &&
            Item->ItemOption == Packet->ItemOptions) {
            ItemIndex = Index;
            break;
        }
    }
    if (ItemIndex < 0) goto error;

    RTShopItemDataRef Item = &Shop->Items[ItemIndex];
    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Item->ItemID);
    Bool Success = true;
    struct _RTItemSlot ItemSlot = { 0 };
    for (Int32 Index = 0; Index < Packet->InventoryIndexCount; Index++) {
        ItemSlot.SlotIndex = Packet->InventoryIndex[Index];
        ItemSlot.Item.ID = Item->ItemID;
        ItemSlot.ItemOptions = Item->ItemOption;

        // TODO: Only for debugging remove me!
        if (Item->ItemID == 454) {
            Int32 Seed = GetTickCount64();
            RTItemDataRef Item = &Runtime->ItemData[Random(&Seed) % Runtime->ItemDataCount];
            ItemSlot.Item.ID = Random(&Seed);

            while (true) {
                ItemSlot.Item.ID = Random(&Seed);
                if (RTRuntimeGetItemDataByIndex(Runtime, ItemSlot.Item.ID)) {
                    break;
                }
            }

            ItemSlot.Item.Extension = Random(&Seed);
            //ItemSlot.Unknown1 = (UInt64)Random(&Seed) << 32 | Random(&Seed);
            ItemSlot.ItemOptions = (UInt64)Random(&Seed) << 32 | Random(&Seed);
        }
        /*
        if (Item->ItemID == 454) {
            Int32 Seed = GetTickCount64();
            RTDataPassiveAbilityCostRef PassiveAbilityCost = &Runtime->Context->PassiveAbilityCostList[Random(&Seed) % Runtime->Context->PassiveAbilityCostCount];
            ItemSlot.Item.ID = PassiveAbilityCost->Index;
            ItemSlot.Item.Extension = 0;
            ItemSlot.ItemOptions = 1;
        }
        */
        if (Item->ItemID >= 33556376) {
            RTItem ItemID;
            ItemID.Serial = Item->ItemID;
            ItemID.UpgradeLevel = 15;
            ItemID.ExtremeLevel = 7;
            ItemID.DivineLevel = 15;
            Item->ItemID = ItemID.Serial;
            Item->ItemOption = 0;
        }

        Success &= RTInventorySetSlot(
            Runtime,
            &Character->InventoryInfo,
            &ItemSlot
        );

        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
        Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

        if (!Success) 
            break;
    }

    if (!Success) goto error;

    S2C_DATA_BUY_ITEM* Response = PacketInit(S2C_DATA_BUY_ITEM);
    Response->Command = S2C_BUY_ITEM;
    Response->ItemID = ItemSlot.Item.Serial;
    Response->ItemOptions = ItemSlot.ItemOptions;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SELL_ITEM) {
    if (!Character) goto error;

    // TODO: Add packet bounds checking for dynamically appended data!
    if (Packet->InventoryIndexCount < 1 || Packet->InventoryIndexCount > SERVER_CHARACTER_MAX_SELL_ITEM_COUNT) goto error;

    RTShopDataRef Shop = RTRuntimeGetShopByWorldNpcID(
        Runtime,
        Character->Info.Position.WorldID,
        Packet->NpcID
    );
    if (!Shop) goto error;

    // TODO: Check NPC distance to character

    for (Int32 Index = 0; Index < Packet->InventoryIndexCount; Index++) {
        Int32 ItemSlotIndex = Packet->InventoryIndex[Index];
        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, ItemSlotIndex);
        if (!ItemSlot) goto error;

        RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
        if (!ItemData) goto error;

        Int32 RecoverySlotIndex = Character->RecoveryInfo.Count % RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT;
        Character->RecoveryInfo.Prices[RecoverySlotIndex] = ItemData->SellPrice;
        Character->RecoveryInfo.Slots[RecoverySlotIndex] = *ItemSlot;
        Character->RecoveryInfo.Count = MIN(Character->RecoveryInfo.Count + 1, RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT);

        if (!RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlotIndex)) goto error;

        // TODO: Calculate sell price based on upgrade level ...
        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] += ItemData->SellPrice;
        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
        Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
        Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH;
    }

    S2C_DATA_SELL_ITEM *Response = PacketInit(S2C_DATA_SELL_ITEM);
    Response->Command = S2C_SELL_ITEM;
    Response->Currency = Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SHOP_LIST) {
    if (!Character) goto error;

    S2C_DATA_GET_SHOP_LIST* Response = PacketInit(S2C_DATA_GET_SHOP_LIST);
    Response->Command = S2C_GET_SHOP_LIST;
    Response->Count = Runtime->ShopDataCount;
    
    for (Int32 Index = 0; Index < Runtime->ShopDataCount; Index++) {
        RTShopDataRef ShopData = &Runtime->ShopData[Index];

        S2C_DATA_GET_SHOP_LIST_INDEX* ShopIndex = PacketAppendStruct(S2C_DATA_GET_SHOP_LIST_INDEX);
        ShopIndex->WorldID = Character->Info.Position.WorldID;
        ShopIndex->ShopID = ShopData->NpcID;
        ShopIndex->ShopIndex = (UInt16)ShopData->Index;
    }

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_SHOP_DATA) {
    if (!Character) goto error;

    RTShopDataRef ShopData = RTRuntimeGetShopByIndex(Runtime, Packet->ShopIndex);
    if (!ShopData) goto error;

    S2C_DATA_GET_SHOP_DATA* Response = PacketInit(S2C_DATA_GET_SHOP_DATA);
    Response->Command = S2C_GET_SHOP_DATA;
    Response->ShopIndex = Packet->ShopIndex;
    Response->Count = ShopData->ItemCount;

    for (Int32 Index = 0; Index < ShopData->ItemCount; Index++) {
        RTShopItemDataRef ItemData = &ShopData->Items[Index];
         
        S2C_DATA_GET_SHOP_DATA_INDEX* ItemIndex = PacketAppendStruct(S2C_DATA_GET_SHOP_DATA_INDEX);
        ItemIndex->ShopSlotIndex = ItemData->SlotID;
        ItemIndex->ItemID = ItemData->ItemID;
        ItemIndex->ItemOptions = ItemData->ItemOption;
        ItemIndex->MinHonorRank = (Int8)ItemData->MinHonorRank;
        ItemIndex->AlzPrice = ItemData->Price;
        ItemIndex->DpPrice = (UInt16)ItemData->DpPrice;
        ItemIndex->WexpPrice = ItemData->WexpPrice;
    }

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_ITEM_RECOVERY_LIST) {
    if (!Character) goto error;

    S2C_DATA_GET_ITEM_RECOVERY_LIST* Response = PacketInit(S2C_DATA_GET_ITEM_RECOVERY_LIST);
    Response->Command = S2C_GET_ITEM_RECOVERY_LIST;
    Response->Count = Character->RecoveryInfo.Count;

    for (Int32 Index = 0; Index < RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT; Index += 1) {
        RTItemSlotRef ItemSlot = &Character->RecoveryInfo.Slots[Index];
        if (!ItemSlot->Item.Serial) continue;

        UInt64 RecoveryPrice = Character->RecoveryInfo.Prices[Index];

        S2C_ITEM_RECOVERY_LIST_SLOT* ResponseData = PacketAppendStruct(S2C_ITEM_RECOVERY_LIST_SLOT);
        ResponseData->ItemID = ItemSlot->Item.ID;
        ResponseData->ItemSerial = ItemSlot->Item.Serial;
        ResponseData->ItemOption = ItemSlot->ItemOptions;
        ResponseData->RecoveryPrice = RecoveryPrice;
        ResponseData->RecoverySlotIndex = Index;
        ResponseData->ExpirationTimestamp = 0;
    }

    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(RECOVER_ITEM) {
    if (!Character) goto error;

    if (Packet->RecoverySlotIndex >= RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT) goto error;

    RTItemSlotRef RecoverySlot = &Character->RecoveryInfo.Slots[Packet->RecoverySlotIndex];
    if (!RecoverySlot->Item.Serial) goto error;

    UInt64 RecoveryPrice = Character->RecoveryInfo.Prices[Packet->RecoverySlotIndex];
    if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < RecoveryPrice) goto error;

    RecoverySlot->SlotIndex = Packet->InventorySlotIndex;

    if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, RecoverySlot)) goto error;

    memset(RecoverySlot, 0, sizeof(struct _RTItemSlot));
    Character->RecoveryInfo.Count -= 1;

    Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= RecoveryPrice;
    Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
    Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
    Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH;

    S2C_DATA_RECOVER_ITEM* Response = PacketInit(S2C_DATA_RECOVER_ITEM);
    Response->Command = S2C_RECOVER_ITEM;
    Response->Result = 1; // TODO: Check Result types and remove disconnects
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

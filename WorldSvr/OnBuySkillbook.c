#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(BUY_SKILLBOOK) {
    if (!Character) goto error;

    RTTrainerDataRef Trainer = RTRuntimeGetTrainerByWorldNpcID(
        Runtime,
        Character->Info.Position.WorldID,
        Packet->NpcID
    );
    if (!Trainer) goto error;

    // TODO: Check NPC distance to character

    RTTrainerSkillDataRef Skill = NULL;
    for (Int32 Index = 0; Index < Trainer->SkillCount; Index++) {
        if (Trainer->Skills[Index].SlotID == Packet->SlotID && Trainer->Skills[Index].ID == Packet->SkillID) {
            Skill = &Trainer->Skills[Index];
        }
    }
    if (!Skill) goto error;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Skill->SkillBookID);
    assert(ItemData);

    if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < ItemData->BuyPrice) goto error;
    
    struct _RTItemSlot ItemSlot = { 0 };
    ItemSlot.SlotIndex = Packet->InventorySlotIndex;
    ItemSlot.Item.ID = Skill->SkillBookID;
    ItemSlot.ItemOptions = Skill->Level;

    Bool Success = RTInventorySetSlot(
        Runtime,
        &Character->InventoryInfo,
        &ItemSlot
    );

    if (Success) {
        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= ItemData->BuyPrice;
    }

    S2C_DATA_BUY_SKILLBOOK* Response = PacketInit(S2C_DATA_BUY_SKILLBOOK);
    Response->Command = S2C_BUY_SKILLBOOK;
    Response->ItemID = Skill->SkillBookID;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
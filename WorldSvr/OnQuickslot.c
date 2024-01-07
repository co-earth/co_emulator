#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SET_QUICKSLOT) {
    if (!Character) goto error;

    Bool Success = false;

    if (Packet->SkillSlotIndex < UINT16_MAX) {
        Success |= RTCharacterAddQuickSlot(Runtime, Character, Packet->SkillSlotIndex, Packet->QuickSlotIndex);
    }
    else {
        Success |= RTCharacterRemoveQuickSlot(Runtime, Character, Packet->QuickSlotIndex);
    }
    
    S2C_DATA_SET_QUICKSLOT* Response = PacketInit(S2C_DATA_SET_QUICKSLOT);
    Response->Command = S2C_SET_QUICKSLOT;
    Response->Success = Success ? 1 : 0;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SWAP_QUICKSLOT) {
    if (!Character) goto error;

    Bool Success = RTCharacterSwapQuickSlot(Runtime, Character, Packet->SourceSlotIndex, Packet->TargetSlotIndex);

    S2C_DATA_SET_QUICKSLOT* Response = PacketInit(S2C_DATA_SET_QUICKSLOT);
    Response->Command = S2C_SET_QUICKSLOT;
    Response->Success = Success ? 1 : 0;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
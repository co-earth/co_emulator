#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CHARACTERS) {
    if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;

    // TODO: Check if is entering premium channel and check service expiration!

    IPC_DATA_WORLD_REQGETCHARACTERS* Request = PacketInit(IPC_DATA_WORLD_REQGETCHARACTERS);
    Request->Command = IPC_WORLD_REQGETCHARACTERS;
    Request->ConnectionID = Connection->ID;
    Request->AccountID = Client->Account.AccountID;
    memcpy(Request->SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
    return SocketSendAll(Server->MasterSocket, Request);

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(OnWorldGetCharacters, IPC_WORLD_ACKGETCHARACTERS, IPC_DATA_WORLD_ACKGETCHARACTERS) {
    if (!ClientConnection || !Client) goto error;

    memcpy(Client->Characters, Packet->Characters, sizeof(Packet->Characters));
    Client->Flags |= CLIENT_FLAGS_CHARACTER_INDEX_LOADED;

    S2C_DATA_GET_CHARACTERS* Response = PacketInit(S2C_DATA_GET_CHARACTERS);
    Response->Command = S2C_GET_CHARACTERS;
    Response->IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
    Response->Unknown2 = 1;
    Response->CharacterSlotID = Client->Account.CharacterSlotID;
    Response->CharacterSlotOrder = Client->Account.CharacterSlotOrder;

    for (Int32 Index = 0; Index < MAX_CHARACTER_COUNT; Index++) {
        GAME_DATA_CHARACTER_INDEX* Character = &Packet->Characters[Index];
        if (Character->ID < 1) continue;

        Character->ID = Character->ID * MAX_CHARACTER_COUNT + Index;
        Character->Style = SwapUInt32(Character->Style);
    }

    memcpy(Response->Characters, Packet->Characters, sizeof(Packet->Characters));
    return SocketSend(Server->ClientSocket, ClientConnection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

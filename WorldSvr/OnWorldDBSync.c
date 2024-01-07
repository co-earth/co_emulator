#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

Void ServerSyncDB(
    ServerContextRef Server,
    Bool Force
) {
    Timestamp Timestamp = GetTickCount64();

    for (Int32 Index = 0; Index < Server->ClientSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Server->ClientSocket->Connections,
            Index
        );
        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client->CharacterEntity.Serial < 1) continue;

        RTCharacterRef Character = RTRuntimeGetCharacter(Server->Runtime, Client->CharacterEntity);
        assert(Character);

        Bool PerformSync = Force || (
            Character->SyncMask &&
            (
                ((Character->SyncPriority & RUNTIME_CHARACTER_SYNC_PRIORITY_LOW) && (Timestamp - Character->SyncTimestamp) >= SERVER_DBSYNC_TIMEOUT_PRIORITY_LOW) ||
                ((Character->SyncPriority & RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH) && (Timestamp - Character->SyncTimestamp) >= SERVER_DBSYNC_TIMEOUT_PRIORITY_HIGH) ||
                (Character->SyncPriority & RUNTIME_CHARACTER_SYNC_PRIORITY_INSTANT)
            )
        );

        if (PerformSync) {
            // We add up index here to potentially balance sync calls to multiple frames per client
            Character->SyncTimestamp = Timestamp + Index;

            IPC_DATA_WORLD_REQDBSYNC* Request = PacketInit(IPC_DATA_WORLD_REQDBSYNC);
            Request->Command = IPC_WORLD_REQDBSYNC;
            Request->ConnectionID = Connection->ID;
            Request->AccountID = Client->Account.AccountID;
            Request->CharacterID = Client->CharacterDatabaseID;
            Request->DBSyncMask = Character->SyncMask;
            Request->DBSyncPriority = Character->SyncPriority;

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_INFO) {
                RTMovementUpdateDeadReckoning(Server->Runtime, &Character->Movement);
                Character->Info.Position.X = Character->Movement.PositionCurrent.X;
                Character->Info.Position.Y = Character->Movement.PositionCurrent.Y;

                PacketAppendMemoryCopy(&Character->Info, sizeof(struct _RTCharacterInfo));
            }

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_EQUIPMENT) {
                PacketAppendMemoryCopy(&Character->EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
            }

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_INVENTORY) {
                PacketAppendMemoryCopy(&Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
            }

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_SKILLSLOT) {
                PacketAppendMemoryCopy(&Character->SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
            }

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_QUICKSLOT) {
                PacketAppendMemoryCopy(&Character->QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
            }

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_QUESTSLOT) {
                PacketAppendMemoryCopy(&Character->QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
            }

            if (Character->SyncMask & RUNTIME_CHARACTER_SYNC_QUESTFLAG) {
                PacketAppendMemoryCopy(&Character->QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
            }

            SocketSendAll(Server->MasterSocket, Request);

            Character->SyncMask = 0;
            Character->SyncPriority = 0;
        }
    }
}

IPC_PROCEDURE_BINDING(OnWorldDBSync, IPC_WORLD_ACKDBSYNC, IPC_DATA_WORLD_ACKDBSYNC) {
    if (!Client || !Character) return;

    if (Packet->DBSyncMaskFailure) {
        Character->SyncMask |= Packet->DBSyncMaskFailure;
        Character->SyncPriority |= Packet->DBSyncPriority;
    }
}
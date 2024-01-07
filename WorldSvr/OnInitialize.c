#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(INITIALIZE) {
    if (Character) goto error;
	
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;
   
	IPC_DATA_WORLD_REQGETCHARACTER* Request = PacketInit(IPC_DATA_WORLD_REQGETCHARACTER);
	Request->Command = IPC_WORLD_REQGETCHARACTER;
	Request->ConnectionID = Connection->ID;
	Request->AccountID = Client->Account.AccountID;
    Request->CharacterID = (Packet->CharacterIndex - Packet->CharacterIndex % MAX_CHARACTER_COUNT) / MAX_CHARACTER_COUNT;
    Request->CharacterIndex = Packet->CharacterIndex;
	return SocketSendAll(Server->MasterSocket, Request);

error:
	return SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(OnWorldGetCharacter, IPC_WORLD_ACKGETCHARACTER, IPC_DATA_WORLD_ACKGETCHARACTER) {
    if (!ClientConnection || !Client) goto error;
    if (!Packet->Success) goto error;

    if (Packet->Character.CharacterData.Position.WorldID == 67) {
        Packet->Character.CharacterData.Position.WorldID = 2;
        Packet->Character.CharacterData.Position.X = 50;
        Packet->Character.CharacterData.Position.Y = 50;
    }

    /*
    RTWorldRef TargetWorld = RTRuntimeGetWorldByID(Runtime, Packet->Character.CharacterData.Position.WorldID);
    if (TargetWorld->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
        RTDungeonDataRef QuestDungeonData = RTRuntimeGetDungeonDataByID(Runtime, TargetWorld->DungeonID);
        assert(QuestDungeonData);

        assert(0 <= QuestDungeonData->FailWarpNpcID - 1 && QuestDungeonData->FailWarpNpcID - 1 < Runtime->WarpIndexCount);
        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[QuestDungeonData->FailWarpNpcID - 1];
        RTPositionRef Position = &WarpIndex->Target[0];
        RTWorldRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
        assert(NewWorld);

        Packet->Character.CharacterData.Position.X = Position->X;
        Packet->Character.CharacterData.Position.Y = Position->Y;
        Packet->Character.CharacterData.Position.WorldID = WarpIndex->WorldID;
    }
    */

    S2C_DATA_INITIALIZE* Response = PacketInitExtended(S2C_DATA_INITIALIZE);
    Response->Command = S2C_INITIALIZE;

    // TODO: Populate correct data!!!

    /* Server Info */
    Response->Server.ServerID = Master->ServerID;
    Response->Server.WorldID = Server->Config->WorldSvr.WorldID;
    Response->Server.PlayerCount = Server->ClientSocket->Connections.Count;
    Response->Server.MaxPlayerCount = Server->Config->WorldSvr.MaxConnectionCount;
    memcpy(Response->Server.Address.Host, Server->Config->WorldSvr.Host, strlen(Server->Config->WorldSvr.Host));
    Response->Server.Address.Port = Server->Config->WorldSvr.Port;
    Response->Server.WorldType = Server->Config->WorldSvr.WorldType;

    /* Character Info */
    Response->MapID = Packet->Character.CharacterData.Position.WorldID;
    Response->Position.X = Packet->Character.CharacterData.Position.X;
    Response->Position.Y = Packet->Character.CharacterData.Position.Y;
    Response->Exp = Packet->Character.CharacterData.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Exp;
    Response->Alz = Packet->Character.CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
    Response->Wexp = Packet->Character.CharacterData.Honor.Exp;
    Response->Level = Packet->Character.CharacterData.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level;
    Response->STR = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_STR];
    Response->DEX = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_DEX];
    Response->INT = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_INT];
    Response->PNT = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_PNT];
    Response->SkillRank = Packet->Character.CharacterData.Skill.Rank;
    Response->SkillLevel = Packet->Character.CharacterData.Skill.Level;
    Response->SkillExp = Packet->Character.CharacterData.Skill.Exp;
    Response->SkillPoint = Packet->Character.CharacterData.Skill.Point;
    Response->HonorPoint = Packet->Character.CharacterData.Honor.Point;

    /* TODO: Populate Server host data */
    CString DummyHost = "127.0.0.1";
    memcpy(Response->ChatServerAddress.Host, DummyHost, strlen(DummyHost));
    Response->ChatServerAddress.Port = 0;
    memcpy(Response->AuctionServerAddress.Host, DummyHost, strlen(DummyHost));
    Response->AuctionServerAddress.Port = 0;
    memcpy(Response->UnknownServerAddress.Host, DummyHost, strlen(DummyHost));
    Response->UnknownServerAddress.Port = 0;

    Response->NationMask = 0;
    Response->WarpMask = 0;

    Response->MapsMask = 0;
    for (Int32 Index = 0; Index < Runtime->Context->MapCodeCount; Index++) {
        if (Runtime->Context->MapCodeList[Index].EntryLevel <= Packet->Character.CharacterData.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level) {
            Response->MapsMask |= 1 << Index;
        }
    }

    Response->CharacterStyle = SwapUInt32(Packet->Character.CharacterData.Style.RawValue);
    Response->CharacterStyleFlags = 0;
    Response->AP = Packet->Character.CharacterData.Ability.Point;
    Response->Axp = Packet->Character.CharacterData.Ability.Exp;
    Response->AllAchievementScore = 0;
    Response->NormalAchievementScore = 0;
    Response->QuestAchievementScore = 0;
    Response->DungeonAchievementScore = 0;
    Response->ItemsAchievementScore = 0;
    Response->PvpAchievementScore = 0;
    Response->WarAchievementScore = 0;
    Response->HuntingAchievementScore = 0;
    Response->CraftAchievementScore = 0;
    Response->CommunityAchievementScore = 0;
    Response->SharedAchievementScore = 0;
    Response->SpecialAchievementScore = 0;
    Response->DisplayTitle = 0;
    Response->EventTitle = 0;
    Response->GuildTitle = 0;
    Response->WarTitle = 0;
    Response->CraftEnergy = 0;
    Response->SortingOrderMask = 0;
    Response->BasicCraftExp = 0;
    Response->GoldMeritExp = 0;
    Response->GoldMeritPoint = 0;
    Response->PlatinumMeritCount = 0;
    Response->PlatinumMeritExp = 0;
    Response->PlatinumMeritPoint[0] = 0;
    Response->PlatinumMeritPoint[1] = 0;
    Response->CharacterCreationDate = Packet->Character.CreationDate;
    Response->ForceGem = Packet->Character.CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_GEM];
    Response->OverlordPoint = 0;
    // Response->SpecialGiftboxPoint = 0;
    Response->TranscendencePoint = 0;
    Response->NameLength = strlen(Packet->Character.Name) + 1;
    CString Name = PacketAppendMemory(strlen(Packet->Character.Name));
    memcpy(Name, Packet->Character.Name, strlen(Packet->Character.Name));

    Response->EquipmentSlotCount = Packet->Character.EquipmentData.Count;
    if (Packet->Character.EquipmentData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.EquipmentData.Slots,
            sizeof(struct _RTItemSlot) * Packet->Character.EquipmentData.Count
        );
    }

    //memset(Packet->Character.InventoryData.Inventory, 0, sizeof(struct _RTItemSlot) * Packet->Character.InventoryData.Count);
    //Packet->Character.InventoryData.Count = 0;

    Response->InventorySlotCount = Packet->Character.InventoryData.Count;
    if (Packet->Character.InventoryData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.InventoryData.Slots,
            sizeof(struct _RTItemSlot) * Packet->Character.InventoryData.Count
        );
    }

    Response->SkillSlotCount = Packet->Character.SkillSlotData.Count;
    if (Packet->Character.SkillSlotData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.SkillSlotData.Skills,
            sizeof(struct _RTSkillSlot) * Packet->Character.SkillSlotData.Count
        );
    }

    Response->QuickSlotCount = Packet->Character.QuickSlotData.Count;
    if (Packet->Character.QuickSlotData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.QuickSlotData.QuickSlots,
            sizeof(struct _RTQuickSlot) * Packet->Character.QuickSlotData.Count
        );
    }

    Response->EssenceAbilityCount = 0;
    Response->BlendedAbilityCount = 0;

    Response->QuestSlotCount = Packet->Character.QuestSlotData.Count;
    memcpy(&Response->QuestFlagInfo, &Packet->Character.QuestFlagData, sizeof(struct _RTCharacterQuestFlagInfo));

    for (Int32 Index = 0; Index < Packet->Character.QuestSlotData.Count; Index++) {
        RTQuestSlotRef QuestSlot = &Packet->Character.QuestSlotData.QuestSlot[Index];
        RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
        assert(Quest);

        UInt16 NpcActionFlags = 0;
        for (Int32 Index = 0; Index < QuestSlot->NpcActionIndex; Index++) {
            NpcActionFlags |= 1 << Index;
        }

        S2C_DATA_INITIALIZE_QUEST_INDEX* QuestResponse = PacketAppendStruct(S2C_DATA_INITIALIZE_QUEST_INDEX);
        QuestResponse->QuestIndex = QuestSlot->QuestIndex;
        QuestResponse->NpcActionFlags = NpcActionFlags;
        QuestResponse->Unknown1 = 1;
        QuestResponse->Unknown2 = 1;
        QuestResponse->QuestSlotIndex = Index;
        
        Int32 CounterCount = Quest->MissionMobCount + Quest->MissionItemCount;
        for (Int32 CounterIndex = 0; CounterIndex < CounterCount; CounterIndex++) {
            PacketAppendValue(UInt8, QuestSlot->Counter[CounterIndex]);
        }
    }

    Response->BlessingBeadCount = 0;
    Response->MercenaryCount = 0;
    Response->EquipmentAppearanceCount = 0;
    Response->InventoryAppearanceCount = 0;
    Response->AchievementCount = 0;
    Response->TitleCount = 0;
    Response->ExtendedTitleCount = 0;
    Response->CraftCount = 0;
    Response->OverlordMasteryCount = 0;
    Response->CollectionCount = 0;
    Response->TransformCount = 0;
    Response->TranscendenceCount = 0;
    // HonorMedalCount
    // ForceWingCount
    // 

    //Response->SpecialGiftboxCount = 5;
    for (Int32 Index = 0; Index < 0; Index++) {
        RTGiftBoxInfoRef GiftBox = PacketAppendStruct(struct _RTGiftBoxInfo);
        GiftBox->Index = Index;
        GiftBox->ReceivedCount = 0;
        GiftBox->ElapsedTime = 0;
        GiftBox->CooldownTime = 1;
        GiftBox->Active = 0;
    }

    assert(ClientConnection->ID < RUNTIME_MEMORY_MAX_CHARACTER_COUNT);

    Character = RTRuntimeCreateCharacter(Server->Runtime);

    RTCharacterInitialize(
        Runtime, 
        Character,
        &Packet->Character.CharacterData,
        &Packet->Character.EquipmentData,
        &Packet->Character.InventoryData,
        &Packet->Character.SkillSlotData,
        &Packet->Character.QuickSlotData,
        &Packet->Character.QuestSlotData,
        &Packet->Character.QuestFlagData
    );

    Character->CharacterIndex = Packet->CharacterIndex;
    
    Client->CharacterDatabaseID = Packet->Character.ID;
    Client->CharacterIndex = Packet->CharacterIndex;
    Client->CharacterEntity = Character->ID;

    Response->Entity = Character->ID;
    Response->CharacterIndex = Packet->CharacterIndex;

    // TODO: Check if character is in quest dungeon instance

    Int32 WorldID = Packet->Character.CharacterData.Position.WorldID;
    RTWorldRef World = RTRuntimeGetWorldByID(Runtime, Character->Info.Position.WorldID);
    assert(World);

    if (World->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
        /*
        RTQuestDungeonDataRef QuestDungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);
        if (!QuestDungeonData) goto error;

        assert(0 <= QuestDungeonData->FailWarpNpcID - 1 && QuestDungeonData->FailWarpNpcID - 1 < Runtime->WarpIndexCount);
        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[QuestDungeonData->FailWarpNpcID - 1];
        RTPositionRef Position = &WarpIndex->Target[0];
        World = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
        assert(World);

        Character->Info.Position.X = Position->X;
        Character->Info.Position.Y = Position->Y;
        Character->Info.Position.WorldID = WarpIndex->WorldID;

        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            Position->X,
            Position->Y,
            RUNTIME_MOVEMENT_SPEED_BASE
        );
        */
    }

    RTWorldSpawnCharacter(Runtime, World, Character->ID);

    // TODO: Check if max hp or base hp is requested here...
    Response->BaseHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    Response->MaxMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
    Response->MaxSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX];
    Response->MaxBP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX];

    Response->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Response->CurrentMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
    Response->CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
    Response->CurrentBP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
    Response->CurrentRage = 0;

    return SocketSend(Server->ClientSocket, ClientConnection, Response);

error:
	if (ClientConnection) {
		return SocketDisconnect(Server->ClientSocket, ClientConnection);
	}
}
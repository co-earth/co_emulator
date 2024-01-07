#include "ClientSocket.h"
#include "MasterSocket.h"
#include "Notification.h"
#include "Server.h"
#include "ServerDataLoader.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

struct _RuntimeDataIndex {
    Archive* Archive;
    CString FileName;
};

// TODO: Free memory for RuntimeData!!!

Void ServerLoadRuntimeData(
    ServerContextRef Server
) {
    memset(Server->RuntimeData, 0, sizeof(struct _RuntimeData));

    const struct _RuntimeDataIndex RuntimeDataIndex[] = {
        { &Server->RuntimeData->Ability, "ability.enc" },
        { &Server->RuntimeData->Achievement, "achievement.enc" },
        { &Server->RuntimeData->Assistant, "assistant.enc" },
        { &Server->RuntimeData->AwakenAuramode, "Awaken_auramode.enc" },
        { &Server->RuntimeData->BattleStyleChange, "BattleStyleChange.enc" },
        { &Server->RuntimeData->BossResistList, "boss_resistlist.enc" },
        { &Server->RuntimeData->BossArena, "BossArena.enc" },
        { &Server->RuntimeData->Main, "cabal.enc" },
        { &Server->RuntimeData->PremiumShop, "caz.enc" },
        { &Server->RuntimeData->ChallengeMissionData, "ChallengeMissionData.enc" },
        { &Server->RuntimeData->ChangeShape, "change_shape.enc" },
        { &Server->RuntimeData->ChaosUpgrade, "chaos_upgrade.enc" },
        { &Server->RuntimeData->ChatBalloon, "chat_balloon.enc" },
        { &Server->RuntimeData->CheckAbility, "checkAB.enc" },
        { &Server->RuntimeData->Collection, "Collection.enc" },
        { &Server->RuntimeData->Dungeon1, "cont.enc" },
        { &Server->RuntimeData->Dungeon2, "cont2.enc" },
        { &Server->RuntimeData->Dungeon3, "cont3.enc" },
        { &Server->RuntimeData->Craft, "craft.enc" },
        { &Server->RuntimeData->Custom, "custom.enc" },
        { &Server->RuntimeData->Destroy, "destroy.enc" },
        { &Server->RuntimeData->DivineUpgrade, "divineupgrade.enc" },
        { &Server->RuntimeData->DropEffect, "DropEffect.enc" },
        { &Server->RuntimeData->DungeonTimeLimit, "dungeon_timelimit.enc" },
        { &Server->RuntimeData->Effector, "effecter.enc" },
        { &Server->RuntimeData->Enchant, "Enchant.enc" },
        { &Server->RuntimeData->EnvironmentSettings, "EnvironmentSetting.enc" },
        { &Server->RuntimeData->EventEnchant, "event_enchant.enc" },
        { &Server->RuntimeData->ExtraObject, "extra_obj.enc" },
        { &Server->RuntimeData->FieldBossRaid, "FieldBossRaid.enc" },
        // { &Server->RuntimeData->FieldWar, "fieldwar.enc" },
        { &Server->RuntimeData->ForceCoreOption, "forcecore_option.enc" },
        { &Server->RuntimeData->ForceWing, "ForceWing.enc" },
        { &Server->RuntimeData->Guide, "guide.enc" },
        { &Server->RuntimeData->GuildDungeon, "GuildDungeon.enc" },
        { &Server->RuntimeData->GuildTreasure, "GuildTreasure.enc" },
        { &Server->RuntimeData->HonorMedal, "honor_medal.enc" },
        { &Server->RuntimeData->Keymap, "keymap.enc" },
        { &Server->RuntimeData->LimitUpgrade, "limit_upgrade.enc" },
        { &Server->RuntimeData->MapInfo, "mapinfo.enc" },
        { &Server->RuntimeData->Market, "market.enc" },
        { &Server->RuntimeData->Mastery, "meister.enc" },
        { &Server->RuntimeData->MeritSystem, "meritsystem.enc" },
        { &Server->RuntimeData->MissionBattle, "missionbattle.enc" },
        { &Server->RuntimeData->NewAssistant, "new_assistant.enc" },
        { &Server->RuntimeData->NPCShop, "NPCShop.enc" },
        { &Server->RuntimeData->OverlordMastery, "overloadmastery.enc" },
        { &Server->RuntimeData->Pet, "pet.enc" },
        { &Server->RuntimeData->PvpBattle, "pvpbattle.enc" },
        { &Server->RuntimeData->Quest, "quest.enc" },
        { &Server->RuntimeData->RandomWarp, "RandomWarp.enc" },
        { &Server->RuntimeData->Rank, "rank.enc" },
        { &Server->RuntimeData->Request, "request.enc" },
        { &Server->RuntimeData->SecretShop, "secretshop.enc" },
        { &Server->RuntimeData->SetEffect, "seteffect.enc" },
        { &Server->RuntimeData->SkillEnhanced, "Skill_Enhanced.enc" },
        { &Server->RuntimeData->Skill, "skill.enc" },
        { &Server->RuntimeData->SpecialMob, "smob.enc" },
        { &Server->RuntimeData->SpecialBox, "SpecialBox.enc" },
        { &Server->RuntimeData->Title, "title.enc" },
        { &Server->RuntimeData->Transform, "Transform.enc" },
        { &Server->RuntimeData->WarAdvantage, "WarAdventage.enc" },
    };
    
    Bool IgnoreErrors = true;
    for (Int32 Index = 0; Index < sizeof(RuntimeDataIndex) / sizeof(struct _RuntimeDataIndex); Index++) {
        Archive* Archive = RuntimeDataIndex[Index].Archive;
        CString FileName = RuntimeDataIndex[Index].FileName;

        LogMessageFormat(
            LOG_LEVEL_INFO,
            "Loading runtime data: %s",
            FileName
        );

        Bool Success = ArchiveLoadFromFileEncrypted(
            Archive,
            PathCombineNoAlloc(Server->Config->WorldSvr.RuntimeDataPath, FileName),
            IgnoreErrors
        );
        if (!Success) {
            FatalErrorFormat("Error loading runtime data: %s", FileName);
        }
    }

    const struct _RuntimeDataIndex ServerRuntimeDataIndex[] = {
        { &Server->RuntimeData->ServerCharacterInit, "server_character_init.xml" },
    };

    for (Int32 Index = 0; Index < sizeof(ServerRuntimeDataIndex) / sizeof(struct _RuntimeDataIndex); Index++) {
        Archive* Archive = ServerRuntimeDataIndex[Index].Archive;
        CString FileName = ServerRuntimeDataIndex[Index].FileName;

        Bool Success = ArchiveLoadFromFile(
            Archive,
            PathCombineNoAlloc(Server->Config->WorldSvr.ServerDataPath, FileName),
            IgnoreErrors
        );
        if (!Success) {
            FatalErrorFormat("Error loading runtime data: %s", FileName);
        }
    }
}

Void ServerRun(
	ServerContextRef Server,
	CString ConfigFilePath
) {
    LogMessage(LOG_LEVEL_INFO, "Loading configuration...");
    
    if (!LoadConfig(Server->Config, ConfigFilePath)) {
        FatalError("Loading configuration failed!");
    }

    Server->Runtime = RTRuntimeCreate(
        ServerRuntimeOnEvent,
        Server
    );
    Server->Runtime->Config.ExpMultiplier = Server->Config->WorldSvr.ExpMultiplier;
    Server->Runtime->Config.SkillExpMultiplier = Server->Config->WorldSvr.SkillExpMultiplier;

    RTRuntimeLoadData(Server->Runtime, Server->Config->WorldSvr.RuntimeDataPath);
    ServerLoadRuntimeData(Server);

    Bool Loaded = true;
    Loaded &= ServerLoadQuestData(Server);
    Loaded &= ServerLoadBattleStyleFormulaData(Server);
    Loaded &= ServerLoadItemData(Server, Server->Config->WorldSvr.RuntimeDataPath, Server->Config->WorldSvr.ServerDataPath);
    Loaded &= ServerLoadMobData(Server, Server->Config->WorldSvr.RuntimeDataPath, Server->Config->WorldSvr.ServerDataPath);
    Loaded &= ServerLoadWarpData(Server);
    Loaded &= ServerLoadShopData(Server, Server->Config->WorldSvr.ServerDataPath, Server->Config->WorldSvr.ServerDataPath);
    Loaded &= ServerLoadUpgradeData(Server, Server->Config->WorldSvr.RuntimeDataPath, Server->Config->WorldSvr.ServerDataPath);
    Loaded &= ServerLoadWorldData(Server, Server->Config->WorldSvr.RuntimeDataPath, Server->Config->WorldSvr.ServerDataPath, true);
    Loaded &= ServerLoadCharacterTemplateData(Server);
    Loaded &= ServerLoadSkillData(Server, Server->Config->WorldSvr.RuntimeDataPath, Server->Config->WorldSvr.ServerDataPath);
    Loaded &= ServerLoadLevelData(Server);
    Loaded &= ServerLoadDungeonData(Server, Server->Config->WorldSvr.RuntimeDataPath, Server->Config->WorldSvr.ServerDataPath);
    if (!Loaded) FatalError("Runtime data loading failed!");

    SocketInitEncryptedServer(
        Server->ClientSocket,
        Server->Config->WorldSvr.MaxConnectionCount,
        &ClientSocketOnConnect,
        &ClientSocketOnDisconnect,
        &ClientSocketOnSend,
        &ClientSocketOnReceived,
        Server
    );

    SocketInitPlainServer(
        Server->MasterSocket,
        1,
        &MasterSocketOnConnect,
        &MasterSocketOnDisconnect,
        NULL,
        &MasterSocketOnReceived,
        Server
    );

    SocketListen(
        Server->ClientSocket, 
        Server->Config->WorldSvr.Port
    );

    srand(GetTimestamp());

    while (true) {
        Timestamp CurrentTimestamp = GetTimestamp();
        Timestamp ElapsedTime = (CurrentTimestamp - Server->Timestamp);
        Server->Timestamp = CurrentTimestamp;

        if (!(Server->Flags & SERVER_FLAGS_MASTER_CONNECTED) &&
            !((Server->MasterSocket->Flags & SOCKET_FLAGS_CONNECTING) ||
              (Server->MasterSocket->Flags & SOCKET_FLAGS_CONNECTED))) {
            SocketConnect(
                Server->MasterSocket,
                Server->Config->MasterSvr.Host,
                Server->Config->MasterSvr.Port,
                0
            );
        }

        SocketUpdate(Server->ClientSocket);
        SocketUpdate(Server->MasterSocket);
        RTRuntimeUpdate(Server->Runtime);

        if (Server->UserListBroadcastTimestamp < Server->UserListUpdateTimestamp &&
            (Server->Flags & SERVER_FLAGS_MASTER_CONNECTED)) {
            Server->UserListBroadcastTimestamp = Server->Timestamp;
            Server->UserListUpdateTimestamp = Server->UserListBroadcastTimestamp;
            BroadcastUserList(Server);
        }

        ServerSyncDB(Server, false);
    }

    RTRuntimeDestroy(Server->Runtime);
}

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Server,
    UInt32 AuthKey,
    UInt16 EntityID
) {
    for (Int32 Index = 0; Index < Server->ClientSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Server->ClientSocket->Connections,
            Index
        );
        ClientContextRef Client = (ClientContextRef)Connection->Userdata;

        if ((Client->Flags & CLIENT_FLAGS_CONNECTED) &&
            Client->AuthKey == AuthKey &&
            Connection->ID == EntityID) {
            return Client;
        }
    }

    return NULL;
}

ClientContextRef ServerGetClientByEntity(
    ServerContextRef Server,
    RTEntity Entity
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    for (Int32 Index = 0; Index < Server->ClientSocket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(&Server->ClientSocket->Connections, Index);
        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client && Client->CharacterEntity.Serial == Entity.Serial) {
            return Client;
        }
    }

    return NULL;
}

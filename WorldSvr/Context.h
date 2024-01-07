#pragma once

#include "Base.h"
#include "Config.h"
#include "Constants.h"
#include "GameProtocol.h"
#include "RuntimeProtocol.h"
#include "MasterDBProtocol.h"

EXTERN_C_BEGIN

enum {
    SERVER_FLAGS_MASTER_CONNECTED               = 1 << 0,
};

enum {
    CLIENT_FLAGS_CONNECTED                      = 1 << 0,
    CLIENT_FLAGS_VERIFIED                       = 1 << 1,
    CLIENT_FLAGS_CHARACTER_INDEX_LOADED         = 1 << 2,
    CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION  = 1 << 5,
};

struct _RuntimeDataCharacterTemplate {
    Int32 BattleStyleIndex;
    struct _RTCharacterInventoryInfo Inventory;
    struct _RTCharacterSkillSlotInfo SkillSlots;
    struct _RTCharacterQuickSlotInfo QuickSlots;
};

struct _RuntimeData {
    Archive Ability;
    Archive Achievement;
    Archive Assistant;
    Archive AwakenAuramode;
    Archive BattleStyleChange;
    Archive BossResistList;
    Archive BossArena;
    Archive Main;
    Archive PremiumShop;
    Archive ChallengeMissionData;
    Archive ChangeShape;
    Archive ChaosUpgrade;
    Archive ChatBalloon;
    Archive CheckAbility;
    Archive Collection;
    Archive Dungeon1;
    Archive Dungeon2;
    Archive Dungeon3;
    Archive Craft;
    Archive Custom;
    Archive Destroy;
    Archive DivineUpgrade;
    Archive DropEffect;
    Archive DungeonTimeLimit;
    Archive Effector;
    Archive Enchant;
    Archive EnvironmentSettings;
    Archive EventEnchant;
    Archive ExtraObject;
    Archive FieldBossRaid;
    Archive FieldWar;
    Archive ForceCoreOption;
    Archive ForceWing;
    Archive Guide;
    Archive GuildDungeon;
    Archive GuildTreasure;
    Archive HonorMedal;
    Archive Keymap;
    Archive LimitUpgrade;
    Archive MapInfo;
    Archive Market;
    Archive Mastery;
    Archive MeritSystem;
    Archive MissionBattle;
    Archive NewAssistant;
    Archive NPCShop;
    Archive OverlordMastery;
    Archive Pet;
    Archive PvpBattle;
    Archive Quest;
    Archive RandomWarp;
    Archive Rank;
    Archive Request;
    Archive SecretShop;
    Archive SetEffect;
    Archive SkillEnhanced;
    Archive Skill;
    Archive SpecialMob;
    Archive SpecialBox;
    Archive Title;
    Archive Transform;
    Archive WarAdvantage;
    Archive ServerCharacterInit;
    Archive Temporary;

    struct _RuntimeDataCharacterTemplate CharacterTemplate[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
};
typedef struct _RuntimeData* RuntimeDataRef;

struct _RuntimeContext {
    CString DataDirectory;
    RuntimeDataRef Data;
};
typedef struct _RuntimeContext* RuntimeContextRef;

struct _ServerContext {
#pragma pack(push, 8)
    struct {
        struct _Config ConfigMemory;
        struct _Socket ClientSocketMemory;
        struct _Socket MasterSocketMemory;
        struct _RuntimeData RuntimeDataMemory;
        struct _RuntimeContext RuntimeContextMemory;
    } Memory;
#pragma pack(pop)

    ConfigRef Config;
    SocketRef ClientSocket;
    SocketRef MasterSocket;
    RTRuntimeRef Runtime;
    RuntimeDataRef RuntimeData;
    RuntimeContextRef RuntimeContext;
    UInt32 Flags;

    Timestamp Timestamp;
    Timestamp UserListBroadcastTimestamp;
    Timestamp UserListUpdateTimestamp;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 Flags;
    Timestamp PasswordVerificationTimestamp;
    UInt32 AuthKey;
    UInt8 SubpasswordFailureCount;
    GAME_DATA_ACCOUNT Account;
    GAME_DATA_CHARACTER_INDEX Characters[MAX_CHARACTER_COUNT];

    // TODO: Store upgrade point & expiration time in temp memory
    Int32 UpgradePoint;

    /* Runtime Data */
    Int32 CharacterDatabaseID;
    UInt32 CharacterIndex;
    RTEntity CharacterEntity;
};
typedef struct _ClientContext* ClientContextRef;

struct _MasterContext {
    UInt8 ServerID;
};
typedef struct _MasterContext* MasterContextRef;

EXTERN_C_END
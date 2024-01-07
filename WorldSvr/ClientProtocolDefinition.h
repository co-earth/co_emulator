
#include "Constants.h"
#include "GameProtocol.h"

#ifndef CLIENT_PROTOCOL_ENUM(...)
#define CLIENT_PROTOCOL_ENUM(...)
#endif

#ifndef CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __COMMAND__, __VERSION__, __BODY__)
#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __COMMAND__, __VERSION__, __BODY__)
#endif

CLIENT_PROTOCOL_STRUCT(S2C_POSITION,
    UInt16 X;
    UInt16 Y;
)

CLIENT_PROTOCOL(C2S, GET_CHARACTERS, 133, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_CHARACTERS, 133, X596,
    S2C_DATA_SIGNATURE;
    UInt8 IsSubpasswordSet;
    UInt8 Unknown1[12];
    UInt8 Unknown2;
    UInt32 CharacterSlotID;
    UInt64 CharacterSlotOrder;
    UInt32 Unknown3;
    UInt32 Unknown4;
    UInt32 Unknown5;
    GAME_DATA_CHARACTER_INDEX Characters[MAX_CHARACTER_COUNT];
)

CLIENT_PROTOCOL(C2S, CREATE_CHARACTER, 134, X596,
    C2S_DATA_SIGNATURE;
    UInt32 Style;
    UInt8 EnterBeginnerGuild;
    UInt8 Unknown;
    UInt8 SlotIndex;
    UInt8 NameLength;
    Char Name[];
)

CLIENT_PROTOCOL(S2C, CREATE_CHARACTER, 134, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt8 Status;
)

CLIENT_PROTOCOL(C2S, CONNECT, 140, X596,
    C2S_DATA_SIGNATURE;
    UInt8 ServerID;
    UInt8 ChannelID;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(S2C, CONNECT, 140, X596,
    S2C_DATA_SIGNATURE;
    UInt32 XorKey;
    UInt32 AuthKey;
    UInt16 ConnectionID;
    UInt16 XorKeyIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, INITIALIZE, 142, X596,
    C2S_DATA_SIGNATURE;
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SECRET_SHOP_INDEX,
    UInt16 SlotIndex;
    UInt32 ShopItemID;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_SERVER_ADDRESS,
    Char Host[MAX_HOSTNAME_LENGTH + 1];
    UInt16 Port;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_SERVER,
    UInt8 ServerID;
    UInt8 WorldID;
    UInt16 PlayerCount;
    UInt8 Unknown1[22];
    UInt16 MaxPlayerCount;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS Address;
    UInt32 WorldType;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_QUEST_INDEX,
    UInt16 QuestIndex;
    UInt16 NpcActionFlags;
    UInt8 Unknown1;
    UInt8 Unknown2;
    UInt8 QuestSlotIndex;
)

CLIENT_PROTOCOL(S2C, INITIALIZE, 142, X596,
    S2C_DATA_SIGNATURE_EXTENDED;
    UInt8 Unknown1[59];
    S2C_DATA_INITIALIZE_SERVER Server;
    RTEntity Entity;
    UInt32 MapID;
    UInt32 UnknownUserID;
    S2C_POSITION Position;
    UInt64 Exp;
    UInt64 Alz;
    UInt64 Wexp;
    Int32 Level;
    UInt32 Unknown4;
    UInt32 STR;
    UInt32 DEX;
    UInt32 INT;
    UInt32 PNT;
    Int32 SkillRank;
    UInt32 Unknown5;
    UInt64 BaseHP;
    UInt64 CurrentHP;
    UInt32 MaxMP;
    UInt32 CurrentMP;
    UInt16 MaxSP;
    UInt16 CurrentSP;
    UInt32 MaxRage;
    UInt32 CurrentRage;
    UInt32 MaxBP;
    UInt32 CurrentBP;
    UInt32 CharacterIndex;
    UInt32 Unknown6;
    UInt8 Unknown7[3];
    UInt8 UnknownSkillRank;
    UInt32 SkillLevel;
    UInt32 SkillExp;
    UInt32 SkillPoint;
    UInt32 Unknown8;
    UInt32 Unknown9;
    UInt64 HonorPoint;
    UInt8 Unknown11[22];
    S2C_DATA_INITIALIZE_SERVER_ADDRESS ChatServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS AuctionServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS UnknownServerAddress;
    UInt16 Unknown12;
    UInt16 NationMask;
    UInt8 Unknown13[3];
    UInt32 WarpMask;
    UInt32 MapsMask;
    UInt32 CharacterStyle;
    UInt8 CharacterStyleFlags;
    UInt8 Unknown14[285];

    UInt16 EquipmentSlotCount;
    UInt8 Unknown99;
    UInt32 InventorySlotCount;
    UInt16 SkillSlotCount;
    UInt16 QuickSlotCount;
    UInt16 MercenaryCount;
    UInt16 UnknownCount1;
    UInt16 UnknownCount2;
    UInt16 AP;
    UInt32 Axp;
    UInt8 EssenceAbilityCount;
    UInt8 UnknownDailyMissionCount;
    UInt16 BlendedAbilityCount;
    UInt16 PremiumServiceCount;
    UInt16 BlessingBeadCount;
    UInt16 QuestSlotCount;
    struct _RTCharacterQuestFlagInfo QuestFlagInfo;
    struct _RTCharacterQuestFlagInfo DungeonFlagInfo;
    UInt8 Unknown15[4097];
    UInt32 UnknownDailyQuestCount;
    
    struct {
        UInt32 Unknown1;
        UInt32 PetSerial;
        UInt32 Unknown3;
        UInt32 Unknown4;
        UInt64 PetItemKind;
        UInt32 Unknown5;
        UInt16 Unknown6;
        Char PetName[MAX_CHARACTER_NAME_LENGTH + 1];
    } Pet;

    UInt8 Unknown16[206];

    UInt8 EquipmentAppearanceCount;
    UInt16 InventoryAppearanceCount;
    Int32 AllAchievementScore;
    Int32 NormalAchievementScore;
    Int32 QuestAchievementScore;
    Int32 DungeonAchievementScore;
    Int32 ItemsAchievementScore;
    Int32 PvpAchievementScore;
    Int32 WarAchievementScore;
    Int32 HuntingAchievementScore;
    Int32 CraftAchievementScore;
    Int32 CommunityAchievementScore;
    Int32 SharedAchievementScore;
    Int32 SpecialAchievementScore;
    UInt16 DisplayTitle;
    UInt16 EventTitle;
    UInt16 GuildTitle;
    UInt16 WarTitle;
    UInt32 AchievementCount;
    UInt32 TitleCount;
    UInt32 UnknownCount5;
    UInt32 CraftCount;
    UInt8 Unknown199;
    UInt32 CraftEnergy;
    UInt8 Unknown17[6];
    UInt8 SortingOrderMask;
    UInt16 UnknownBasicCraftCount;
    UInt16 BasicCraftExp;
    // UInt16 UnknownBasicCraftExp;
    // UInt8 BasicCraftFlags[256];
    // UInt8 Unknown19[1796];
    UInt8 Unknown19[2072];
    // UInt32 SkillCooldownCount;
    // UInt8 Unknown65[14];
    UInt32 GoldMeritCount;
    UInt32 GoldMeritExp;
    UInt32 GoldMeritPoint;
    UInt8 PlatinumMeritCount;
    UInt32 PlatinumMeritExp;
    UInt32 PlatinumMeritPoint[2];
    UInt8 Unknown21;
    Timestamp CharacterCreationDate;
    UInt8 Unknown22[9];
    UInt32 ExtendedTitleCount;
    UInt64 ForceGem;
    UInt8 Unknown33[10];
    UInt16 OverlordLevel;
    UInt64 OverlordExp;
    UInt16 OverlordPoint;
    UInt8 OverlordMasteryCount;
    UInt32 UnknownMeritMasteryCount;
    // UInt32 HonorMedalCount;
    // UInt32 HonorMedalExp;
    UInt8 Unknown84[4];
    UInt8 UnknownSpecialGiftBoxCount;

    struct {
        UInt8 Rank;
        UInt8 Level;
        UInt64 Exp;
    } ForceWing;

    UInt8 Unknown24[38];
    // UInt16 UnknownSpecialGiftboxPoint;
    // UInt8 UnknownSpecialGiftboxCount;
    UInt8 Unknown26[95];
    UInt32 Unknown2733[13];
    UInt16 CollectionCount;
    UInt16 TransformCount;
    UInt8 Unknown32[7];
    // Timestamp SecretShopResetTime;
    // UInt8 SecretShopRefreshCost;
    UInt8 UnknownSecretShopRefreshCost;
    Timestamp UnknownSecretShopResetTime;
    S2C_DATA_SECRET_SHOP_INDEX SecretShops[MAX_SECRET_SHOP_COUNT];
    UInt8 Unknown44[9];
    UInt32 TranscendencePoint;
    UInt32 TranscendenceCount;
    UInt8 U_UPDATEINV[3134];
    //UInt32 EventPassMissionCount;
    //UInt32 Unknown45;
    UInt8 NameLength;
    Char Name[0];
)

CLIENT_PROTOCOL(C2S, SET_QUICKSLOT, 146, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuickSlotIndex;
    UInt16 SkillSlotIndex;
)

CLIENT_PROTOCOL(S2C, SET_QUICKSLOT, 146, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, SWAP_QUICKSLOT, 147, X596,
    C2S_DATA_SIGNATURE;
    UInt16 SourceSlotIndex;
    UInt16 TargetSlotIndex;
)

CLIENT_PROTOCOL(S2C, SWAP_QUICKSLOT, 147, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, GET_SERVER_TIME, 148, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_SERVER_TIME, 148, X596,
    S2C_DATA_SIGNATURE;
    Timestamp Timestamp;
    Int16 Timezone;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_LOOT_RESULT_SUCCESS = 0x60,
    S2C_DATA_LOOT_RESULT_OWNERSHIP_ERROR,
    S2C_DATA_LOOT_RESULT_UNKNOWN1,
    S2C_DATA_LOOT_RESULT_SLOTINUSE_ERROR,
    S2C_DATA_LOOT_RESULT_ANTIADDICTION_ERROR,
    S2C_DATA_LOOT_RESULT_OUTOFRANGE_ERROR
)

CLIENT_PROTOCOL(C2S, LOOT_INVENTORY_ITEM, 153, X596,
    C2S_DATA_SIGNATURE;
    RTEntity Entity;
    UInt16 UniqueKey;
    UInt64 ItemID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, LOOT_INVENTORY_ITEM, 153, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_LOOT_CURRENCY_ITEM_INDEX,
    RTEntity Entity;
    UInt16 UniqueKey;
)

CLIENT_PROTOCOL(C2S, LOOT_CURRENCY_ITEM, 154, X596,
    C2S_DATA_SIGNATURE;
    UInt16 Count;
    C2S_DATA_LOOT_CURRENCY_ITEM_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, LOOT_CURRENCY_ITEM, 154, X596,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    UInt8 Result;
    UInt64 Currency;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_ITEM_PRICE_INDEX,
    UInt32 SlotIndexItemPrice;
    UInt32 AmountItemPrice;
)

CLIENT_PROTOCOL(C2S, BUY_ITEM, 161, CHECK_OST,
    C2S_DATA_SIGNATURE;
    UInt32 TabIndex;
    UInt16 NpcID;
    UInt8 Unknown1;
    UInt16 SlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 SlotIndex2;
    UInt32 Unknown2;
    UInt32 InventoryIndexCount;
    UInt32 ItemPriceCount;
    S2C_DATA_ITEM_PRICE_INDEX ItemPriceTable[522];
    UInt8 Unknown3;
    Int32 InventoryIndex[0];
)

CLIENT_PROTOCOL(S2C, BUY_ITEM, 161, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 Unknown2;
    UInt16 Unknown3;
    UInt16 Unknown4;
)

CLIENT_PROTOCOL(C2S, SELL_ITEM, 162, X596,
    C2S_DATA_SIGNATURE;
    UInt16 NpcID;
    UInt16 Unknown1;
    UInt8 Unknown2;
    UInt32 InventoryIndexCount;
    Int32 InventoryIndex[0];
)

CLIENT_PROTOCOL(S2C, SELL_ITEM, 162, X596,
    S2C_DATA_SIGNATURE;
    UInt64 Currency;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, RECOVER_ITEM, 166, X596,
    C2S_DATA_SIGNATURE;
    UInt8 RecoverySlotIndex;
    UInt32 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, RECOVER_ITEM, 166, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, GET_ITEM_RECOVERY_LIST, 167, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_ITEM_RECOVERY_LIST_SLOT,
    UInt64 ItemID;
    UInt64 ItemSerial;
    UInt64 ItemOption;
    UInt64 RecoveryPrice;
    UInt8 RecoverySlotIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
    Timestamp ExpirationTimestamp;
)

CLIENT_PROTOCOL(S2C, GET_ITEM_RECOVERY_LIST, 167, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Count;
    S2C_ITEM_RECOVERY_LIST_SLOT Slots[0];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_MOB_TARGET,
    RTEntity Entity;
    UInt8 EntityIDType;
    UInt16 Unknown2;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_MOB, 174, X596,
    C2S_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt32 Unknown1;
    S2C_POSITION PositionSet;
    UInt8 Unknown2;
    UInt32 Unknown3;
    UInt8 TargetCount;
    S2C_POSITION PositionCharacter;
    S2C_POSITION PositionTarget;
    C2S_DATA_SKILL_TO_MOB_TARGET Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_TO_MOB_TARGET,
    RTEntity Entity;
    UInt8 EntityIDType;
    UInt8 AttackType;
    UInt32 MobAppliedDamage;
    UInt32 MobTotalDamage;
    UInt32 MobAdditionalDamage;
    UInt64 MobHP;
    S2C_POSITION PositionSet;
    UInt32 BfxType;
    UInt32 BfxDuration;
    UInt8 Unknown3;
)

CLIENT_PROTOCOL(S2C, SKILL_TO_MOB, 174, X596,
    S2C_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt64 CharacterHP;
    UInt32 CharacterMP;
    UInt16 CharacterSP;
    UInt64 AccumulatedExp;
    UInt64 AccumulatedOxp;
    UInt64 ReceivedMxp;
    UInt32 ReceivedSkillExp;
    UInt16 AP;
    UInt64 AXP;
    UInt8 Unknown2[20];
    Int32 Unknown4;
    UInt8 Unknown5;
    UInt64 CharacterMaxHP;
    UInt32 Unknown6;
    UInt8 TargetCount;
    S2C_DATA_SKILL_TO_MOB_TARGET Data[0];
)

/*
enum {
    RUNTIME_SKILL_GROUP_PASSIVE = 0,
    RUNTIME_SKILL_GROUP_ATTACK = 1,
    RUNTIME_SKILL_GROUP_ATTACK_UNKNOWN = 3,
    RUNTIME_SKILL_GROUP_MOVEMENT = 5,
    RUNTIME_SKILL_GROUP_UNKNOWN_13 = 13,
    RUNTIME_SKILL_GROUP_BUFF = 24,
    RUNTIME_SKILL_GROUP_DEBUFF = 25,
    RUNTIME_SKILL_GROUP_HEAL = 26,
    RUNTIME_SKILL_GROUP_ASTRAL = 31,
    RUNTIME_SKILL_GROUP_UNKNOWN_32 = 32,
    RUNTIME_SKILL_GROUP_WING = 33,
    RUNTIME_SKILL_GROUP_VEHICLE = 38,
    RUNTIME_SKILL_GROUP_COMBO = 40,
    RUNTIME_SKILL_GROUP_UNKNOWN_42 = 42,
    RUNTIME_SKILL_GROUP_TOTEM = 43,
    RUNTIME_SKILL_GROUP_AGGRO = 44,
    RUNTIME_SKILL_GROUP_COUNT = 50
};
*/

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_MOVEMENT,
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_ASTRAL,
    UInt16 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_CHARACTER, 175, X596,
    C2S_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_MOVEMENT,
    UInt32 Unknown1;
    UInt32 CharacterMP;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_ASTRAL,
    UInt16 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, SKILL_TO_CHARACTER, 175, X596,
    S2C_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, ATTACK_TO_MOB, 176, X596,
    C2S_DATA_SIGNATURE;
    RTEntity Entity;
    UInt8 EntityIDType;
    UInt8 MissMob;
)

CLIENT_PROTOCOL(S2C, ATTACK_TO_MOB, 176, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    UInt8 EntityIDType;
    UInt64 CharacterHP;
    UInt32 CharacterMP;
    UInt16 CharacterSP;
    UInt8 AttackType;
    UInt64 AccumulatedExp;
    UInt64 AccumulatedOxp;
    UInt64 ReceivedSkillExp;
    UInt32 MobAppliedDamage;
    UInt32 MobTotalDamage;
    UInt32 MobAdditionalDamage;
    UInt64 MobHP;
    UInt32 Unknown1;
    UInt16 AP;
    UInt32 AXP;
    UInt8 Unknown2;
    UInt64 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_BEGIN, 190, X596,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
    S2C_POSITION PositionCurrent;
    UInt16 WorldID;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_BEGIN, 210, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_END, 191, X596,
    C2S_DATA_SIGNATURE;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_END, 211, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_CHANGE, 192, X596,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
    S2C_POSITION PositionCurrent;
    UInt16 WorldID;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_WAYPOINTS, 193, X596,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionCurrent;
    S2C_POSITION PositionNext;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_TILE_POSITION, 194, X596,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionCurrent;
    S2C_POSITION PositionNext;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_CHANGE, 212, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_MOVEMENT_BEGIN, 213, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_MOVEMENT_END, 214, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL(S2C, MOB_CHASE_BEGIN, 215, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_CHASE_END, 216, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_ENTITY_SPAWN_TYPE_INIT = 0x30,
    S2C_DATA_ENTITY_SPAWN_TYPE_WARP,
    S2C_DATA_ENTITY_SPAWN_TYPE_MOVE,
    S2C_DATA_ENTITY_SPAWN_TYPE_UNKNOWN
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_MOBS_SPAWN_INDEX,
    RTEntity Entity;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
    UInt16 MobSpeciesID;
    UInt64 MaxHP;
    UInt64 CurrentHP;
    UInt8 IsChasing;
    UInt8 Level;
    UInt8 UnknownNation;
    UInt8 Unknown1;
    UInt16 UnknownAnimationID;
    UInt32 UnknownAnimationTickCount;
    UInt8 Unknown2;
    UInt8 UnknownEvent;
    UInt8 Unknown3;
    UInt32 UnknownCharacterIndex;
    UInt8 Unknown4[12];
)

CLIENT_PROTOCOL(S2C, MOBS_SPAWN, 202, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Count;
    S2C_DATA_MOBS_SPAWN_INDEX Data[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_ENTITY_DESPAWN_TYPE_DEAD = 0x10,
    S2C_DATA_ENTITY_DESPAWN_TYPE_WARP,
    S2C_DATA_ENTITY_DESPAWN_TYPE_UNKNOWN1,
    S2C_DATA_ENTITY_DESPAWN_TYPE_RETURN,
    S2C_DATA_ENTITY_DESPAWN_TYPE_DISAPPEAR,
    S2C_DATA_ENTITY_DESPAWN_TYPE_NOTIFY_DEAD
)

CLIENT_PROTOCOL(S2C, MOBS_DESPAWN, 203, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    UInt8 DespawnType;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SPAWN_ITEM_INDEX,
    RTEntity Entity;
    UInt64 ItemOptions;
    UInt32 SourceIndex;
    UInt64 ItemID;
    UInt16 X;
    UInt16 Y;
    UInt16 UniqueKey;
    UInt8 Type;
    UInt8 Unknown4;
)

CLIENT_PROTOCOL(S2C, NFY_SPAWN_ITEM, 204, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Count;
    S2C_DATA_NFY_SPAWN_ITEM_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, ITEM_DESPAWN, 205, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    UInt8 DespawnType;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_TO_MOB_TARGET,
    RTEntity Entity;
    UInt32 EntityIDType;
    UInt8 AttackType;
    UInt64 MobHP;
    UInt32 BfxType;
    UInt32 BfxDuration;
    UInt8 Unknown2;
    UInt32 Unknown3;
    S2C_POSITION PositionSet;
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_MOB, 220, X596,
    S2C_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 TargetCount;
    UInt32 CharacterIndex;
    S2C_POSITION PositionSet;
    UInt8 Unknown1;
    UInt64 CharacterHP;
    UInt32 Unknown2;
    S2C_DATA_NFY_SKILL_TO_MOB_TARGET Data[0];
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_CHARACTER, 221, X596,
    S2C_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt32 CharacterIndex;
    RTEntity Entity;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL(S2C, NFY_ATTACK_TO_MOB, 225, X596,
    S2C_DATA_SIGNATURE;
    UInt8 AttackType;
    UInt32 CharacterIndex;
    RTEntity Mob;
    UInt8 MobIDType;
    UInt64 MobHP;
    UInt8 Unknown1;
    UInt64 CharacterHP;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, WARP, 244, X596,
    C2S_DATA_SIGNATURE;
    UInt8 NpcID;
    UInt16 Unknown1;
    UInt16 SlotIndex;
    UInt8 Unknown2[20];
    Int32 WarpIndex;
    UInt8 Unknown3;
)

CLIENT_PROTOCOL(S2C, WARP, 244, X596,
    S2C_DATA_SIGNATURE;
    S2C_POSITION Position;
    UInt64 AccumulatedExp;
    UInt64 AccumulatedOxp;
    UInt64 ReceivedSkillExp;
    UInt64 Currency;
    RTEntity Entity;
    UInt8 WorldType;
    UInt32 Unknown2;
    UInt32 WorldID;
    UInt32 DungeonID;
    UInt64 Result;
)

CLIENT_PROTOCOL(S2C, BATTLE_RANK_UP, 276, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Level;
)

CLIENT_PROTOCOL(C2S, ADD_FORCE_SLOT_OPTION, 280, 0000,
    C2S_DATA_SIGNATURE;
    Int32 ItemSlotIndex;
    Int32 Unknown1;
    Int32 OptionScrollSlotIndex;
    Int32 Unknown2;
    UInt8 ForceCoreCount;
    Int32 ForceCoreSlotIndices[10];
)

CLIENT_PROTOCOL(S2C, ADD_FORCE_SLOT_OPTION, 280, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1[10];
)

CLIENT_PROTOCOL(C2S, QUEST_BEGIN, 282, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt8 SlotID;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, QUEST_BEGIN, 282, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, QUEST_CLEAR, 283, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt16 SlotID;
    UInt16 RewardItemIndex;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_QUEST_CLEAR_RESULT_TYPE_UNKNOWN,
    S2C_DATA_QUEST_CLEAR_RESULT_TYPE_SUCCESS = 4
)

CLIENT_PROTOCOL(S2C, QUEST_CLEAR, 283, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt16 ResultType;
    UInt32 Unknown1;
    UInt64 RewardExp;
)

CLIENT_PROTOCOL(C2S, QUEST_CANCEL, 284, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt8 SlotID;
    UInt8 QuestDungeonCount;
    UInt8 AddItemCount;
    UInt8 DeleteItemCount;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, QUEST_CANCEL, 284, 0000,
    S2C_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(C2S, USE_ITEM, 285, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 InventorySlotIndex;
    UInt8 Data[1];
)

CLIENT_PROTOCOL(S2C, USE_ITEM, 285, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_CHARACTER_UPDATE_TYPE_HPPOTION = 1,
    S2C_DATA_CHARACTER_UPDATE_TYPE_MPPOTION,
    S2C_DATA_CHARACTER_UPDATE_TYPE_HP,
    S2C_DATA_CHARACTER_UPDATE_TYPE_MP,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP_INCREASE,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP_DECREASE,
    S2C_DATA_CHARACTER_UPDATE_TYPE_EXP,
    S2C_DATA_CHARACTER_UPDATE_TYPE_RANK,
    S2C_DATA_CHARACTER_UPDATE_TYPE_LEVEL,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP_DECREASE_EX,
    S2C_DATA_CHARACTER_UPDATE_TYPE_BUFF_POTION,
    S2C_DATA_CHARACTER_UPDATE_TYPE_REPUTATION,
    S2C_DATA_CHARACTER_UPDATE_TYPE_GUIDITEMFX,
    S2C_DATA_CHARACTER_UPDATE_TYPE_RESURRECTION,
    S2C_DATA_CHARACTER_UPDATE_TYPE_PENALTY_EXP,
    S2C_DATA_CHARACTER_UPDATE_TYPE_DAMAGE_CELL,
    S2C_DATA_CHARACTER_UPDATE_TYPE_DEFICIENCY,
    S2C_DATA_CHARACTER_UPDATE_TYPE_AUTH_HP_POTION
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_DATA, 287, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Type;

    union {
        struct { UInt8 _0[34]; UInt64 HPAfterPotion; UInt8 _1[4]; };
        struct { UInt8 _2[38]; UInt64 HP; };
        struct { UInt8 _3[34]; UInt32 MP; UInt8 _4[8]; };
        struct { UInt8 _5[34]; UInt32 SP; UInt8 _6[8]; };
        struct { UInt8 _7[34]; UInt32 Level; UInt8 _8[8]; };
    };
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_CHARACTER_EVENT_TYPE_LEVELUP = 1,
    S2C_DATA_CHARACTER_EVENT_TYPE_RANKUP = 2,
    S2C_DATA_CHARACTER_EVENT_TYPE_UNKNOWN = 4
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_EVENT, 288, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Type;
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_START, 290, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_START, 290, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_END, 291, X596,
    C2S_DATA_SIGNATURE;
    Int16 Unknown1;
    Int16 Unknown2;
    Int8 Unknown3;
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_END, 291, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_END, 294, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt16 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_SPAWN, 292, X596,
    C2S_DATA_SIGNATURE;
    UInt8 IsActive;
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_SPAWN, 301, X596,
    S2C_DATA_SIGNATURE;
    UInt32 DungeonTimeout1;
    UInt32 DungeonTimeout2;
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_PATTERN_PART_COMPLETED, 303, X596,
    S2C_DATA_SIGNATURE;
    UInt8 PatternPartIndex;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_QUEST_ACTION_INDEX,
    UInt32 ActionIndex;
    UInt16 SlotIndex;
)

CLIENT_PROTOCOL(C2S, QUEST_ACTION, 320, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt32 NpcSetID;
    UInt8 SlotIndex;
    UInt8 ActionCount;
    C2S_DATA_QUEST_ACTION_INDEX Actions[0];
)

CLIENT_PROTOCOL(S2C, QUEST_ACTION, 320, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
)

CLIENT_PROTOCOL(C2S, ATTACK_BOSS_MOB, 321, 0000,
    C2S_DATA_SIGNATURE;
    RTEntity Entity;
)

CLIENT_PROTOCOL(S2C, ATTACK_BOSS_MOB, 321, 0000,
    S2C_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(C2S, GET_PREMIUM_SERVICE, 324, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_PREMIUM_SERVICE, 324, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt32 ServiceType;
    Timestamp ExpirationDate;
    UInt32 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
    UInt32 Unknown5;
)

CLIENT_PROTOCOL(S2C, NFY_PREMIUM_SERVICE, 325, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt32 ServiceType;
    Timestamp ExpirationDate;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, CHANGE_DIRECTION, 391, 0000,
    C2S_DATA_SIGNATURE;
    Float32 Direction;
)

CLIENT_PROTOCOL(S2C, CHANGE_DIRECTION, 392, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Float32 Direction;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_BEGIN, 401, 0000,
    C2S_DATA_SIGNATURE;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_BEGIN, 403, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Timestamp TickCount;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_END, 402, 0000,
    C2S_DATA_SIGNATURE;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_END, 404, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_CHANGE, 405, 0000,
    C2S_DATA_SIGNATURE;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_CHANGE, 406, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Timestamp TickCount;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(C2S, EXTRACT_ITEM, 411, X596,
    C2S_DATA_SIGNATURE;
    UInt32 InventorySlotIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_EXTRACT_ITEM_SLOT_INDEX,
    UInt64 ItemID;
    UInt64 Unknown1;
    UInt64 ItemOption;
    UInt16 InventorySlotIndex;
    UInt32 Unnown3;
)

CLIENT_PROTOCOL(S2C, EXTRACT_ITEM, 411, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt8 ItemCount;
    S2C_EXTRACT_ITEM_SLOT_INDEX Items[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_MOB_ATTACK_TARGET,
    UInt32 CharacterIndex;
    Bool IsDead;
    UInt8 Result;
    UInt32 AppliedDamage;
    UInt64 TargetHP;
    UInt8 Unknown1[33];
)

CLIENT_PROTOCOL(S2C, NFY_MOB_ATTACK_AOE, 413, X596,
    S2C_DATA_SIGNATURE;
    RTEntity Entity;
    Bool IsDefaultSkill;
    UInt8 Unknown1;
    UInt64 MobHP;
    UInt32 Unknown2;
    UInt16 TargetCount;
    S2C_DATA_MOB_ATTACK_TARGET Data[0];
)

CLIENT_PROTOCOL(C2S, GET_SERVER_ENVIRONMENT, 464, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_SERVER_ENVIRONMENT, 464, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 MaxLevel;
    UInt8 DummyEnabled;
    UInt8 CashshopEnabled;
    UInt8 NetCafePointEnabled;
    UInt16 MinChatLevel;
    UInt16 MinShoutLevel;
    UInt16 MinShoutSkillRank;
    UInt64 MaxInventoryCurrency;
    UInt64 MaxWarehouseCurrency;
    UInt64 MaxAuctionCurrency;
    UInt8 Unknown4[39];
    Int64 MaxHonorPoint;
    Int64 MinHonorPoint;
    UInt8 Unknown5[316];
)

CLIENT_PROTOCOL(C2S, CHECK_DUNGEON_PLAYTIME, 485, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 DungeonID;
)

CLIENT_PROTOCOL(S2C, CHECK_DUNGEON_PLAYTIME, 485, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 DungeonID;
    Int32 MaxInstanceCount;
    Int32 InstanceCount;
    UInt32 Unknown1[3];
    UInt16 Unknown2;
    UInt16 RemainingPlayTimeInSeconds;
    UInt32 Unknown3[7];
    UInt16 Unknown4;
    UInt16 MaxEntryCount;
    UInt16 Unknown5;
    UInt32 Unknown6;
)

CLIENT_PROTOCOL(S2C, UPDATE_SKILL_STATUS, 760, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 SkillRank;
    UInt32 SkillLevel;
    UInt32 SkillLevelMax;
    UInt32 SkillExp;
    UInt32 SkillPoint;
)

CLIENT_PROTOCOL_ENUM(
    C2S_DATA_VERIFY_CREDENTIALS_TYPE_UNKNOWN,
    C2S_DATA_VERIFY_CREDENTIALS_TYPE_PASSWORD
)

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS, 800, 0000,
    C2S_DATA_SIGNATURE;
    Int32 CredentialsType;
    Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS, 800, 0000,
    S2C_DATA_SIGNATURE;
    Bool Success;
)

CLIENT_PROTOCOL(C2S, UPGRADE_ITEM_LEVEL, 951, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt16 InventorySlotIndex;
    UInt16 CoreCount;
    UInt8 Unknown2;
    UInt16 UnknownCounter1;
    UInt16 UnknownCounter2;
    UInt8 Unknown3[0];
)

CLIENT_PROTOCOL(S2C, UPGRADE_ITEM_LEVEL, 951, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt64 ItemID;
    UInt64 ItemOption;
    UInt32 ItemDuration;
    UInt8 Unknown5;
    UInt32 Unknown6;
    UInt32 Unknown7;
    UInt32 Unknown8;
)

CLIENT_PROTOCOL(C2S, CREATE_SUBPASSWORD, 1030, 0000,
    C2S_DATA_SIGNATURE;
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 Type;
    UInt32 Question;
    Char Answer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
    UInt8 Unknown2[111];
    UInt32 Mode;
)

CLIENT_PROTOCOL(S2C, CREATE_SUBPASSWORD, 1030, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt32 Mode;
    UInt32 Type;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, CHECK_SUBPASSWORD, 1032, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, CHECK_SUBPASSWORD, 1032, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 IsVerificationRequired;
)

CLIENT_PROTOCOL(C2S, VERIFY_SUBPASSWORD, 1034, 0000,
    C2S_DATA_SIGNATURE;
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 Type;
    UInt32 ExpirationInHours;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(S2C, VERIFY_SUBPASSWORD, 1034, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt8 FailureCount;
    UInt32 Unknown1;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, VERIFY_DELETE_SUBPASSWORD, 1040, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 Type;
    UInt8 Unknown1[65];
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_DELETE_SUBPASSWORD, 1040, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt8 FailureCount;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, DELETE_SUBPASSWORD, 1042, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 Type;
)

CLIENT_PROTOCOL(S2C, DELETE_SUBPASSWORD, 1042, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, ADD_PASSIVE_ABILITY, 1060, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 InventorySlotIndex;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, ADD_PASSIVE_ABILITY, 1060, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, UPGRADE_PASSIVE_ABILITY, 1061, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 AbilityID;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(S2C, UPGRADE_PASSIVE_ABILITY, 1061, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, REMOVE_PASSIVE_ABILITY, 1062, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 AbilityID;
    UInt16 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, REMOVE_PASSIVE_ABILITY, 1062, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, BUY_SKILLBOOK, 2003, 0000,
    C2S_DATA_SIGNATURE;
    UInt8 NpcID;
    UInt16 SlotID;
    UInt16 SkillID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, BUY_SKILLBOOK, 2003, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt32 ItemID;
    UInt32 Unknown3[6];
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_MOB_KILL, 2006, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 MobSpeciesID;
    UInt16 SkillIndex;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, ENTER_DUNGEON_GATE, 2029, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 DungeonID;
    UInt32 Unknown1;
    UInt32 NpcID;
    UInt32 Unknown3;
    Int32 Unknown4;
    UInt32 WorldID;
    UInt8 Unknown5;
    UInt16 DungeonBoostLevel;
)

CLIENT_PROTOCOL(S2C, ENTER_DUNGEON_GATE, 2029, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
    UInt32 DungeonID;
    UInt32 Unknown1;
    UInt32 NpcID;
    UInt32 Unknown3;
    Int32 Unknown4;
    UInt32 WorldID;
    UInt8 Unknown5;
    UInt16 DungeonBoostLevel;
)

CLIENT_PROTOCOL(C2S, GET_SPECIAL_EVENT_CHARACTER, 2156, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_SPECIAL_EVENT_CHARACTER, 2156, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown[521];
)

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS_SUBPASSWORD, 2160, 0000,
    C2S_DATA_SIGNATURE;
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS_SUBPASSWORD, 2160, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(CSC_DATA_ITEM_SLOT_INDEX,
    UInt32 StorageType;
    UInt32 Index;
)

CLIENT_PROTOCOL(C2S, MOVE_INVENTORY_ITEM, 2165, 0000,
    C2S_DATA_SIGNATURE;
    CSC_DATA_ITEM_SLOT_INDEX Source;
    CSC_DATA_ITEM_SLOT_INDEX Destination;
    UInt16 Unknown1;
    UInt16 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, MOVE_INVENTORY_ITEM, 2165, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, MOVE_INVENTORY_ITEM_LIST, 2167, X596,
    C2S_DATA_SIGNATURE;
    UInt16 ItemCount;
    UInt32 Unknown1[3];
    UInt8 Data[0];
//    CSC_DATA_ITEM_SLOT_INDEX Source[];
//    CSC_DATA_ITEM_SLOT_INDEX Destination[];
)

CLIENT_PROTOCOL(S2C, MOVE_INVENTORY_ITEM_LIST, 2167, X596,
    S2C_DATA_SIGNATURE;
    UInt16 Result;
)

CLIENT_PROTOCOL(C2S, UPDATE_QUEST_LIST, 2175, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 QuestID;
    UInt8 UnknownIndex1;
    UInt8 UnknownIndex2;
)

CLIENT_PROTOCOL(C2S, GET_SHOP_LIST, 2179, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_LIST_INDEX,
    UInt32 WorldID;
    UInt8 ShopID;
    UInt16 ShopIndex;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(S2C, GET_SHOP_LIST, 2179, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    S2C_DATA_GET_SHOP_LIST_INDEX Shops[0];
)

CLIENT_PROTOCOL(C2S, GET_SHOP_DATA, 2180, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 ShopIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_DATA_INDEX,
    UInt16 Unknown1;
    UInt16 ShopSlotIndex;
    UInt32 ItemID;
    UInt32 ItemType;
    UInt32 ItemOptions;
    UInt32 Unknown2;
    UInt32 ItemDuration;
    UInt8 Unknown5[3];
    Int8 MinHonorRank;
    Int8 MaxHonorRank;
    Int8 Unknown6[6];
    UInt64 AlzPrice;
    UInt64 WexpPrice;
    UInt16 ApPrice;
    UInt16 DpPrice;
    UInt32 Unknown8;
    UInt64 CashPrice;
)

CLIENT_PROTOCOL(S2C, GET_SHOP_DATA, 2180, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 ShopIndex;
    UInt16 Count;
    S2C_DATA_GET_SHOP_DATA_INDEX Data[0];
)

CLIENT_PROTOCOL(C2S, GET_CASH_BALANCE, 2181, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_CASH_BALANCE, 2182, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Amount;
)

CLIENT_PROTOCOL(S2C, UPDATE_UPGRAGE_POINTS, 2506, 0000,
    S2C_DATA_SIGNATURE;
    Int32 UpgradePoint;
    UInt32 Timestamp;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_DUNGEON_REWARD_LIST, 2520, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 DungeonID;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY_INDEX,
    UInt8 Unknown1;
    UInt32 ItemID;
    UInt32 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
    UInt32 Unknown5;
    UInt8 Unknown6;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY,
    UInt8 Position;
    UInt32 RepeatCount;
    UInt32 RewardCount;
    // S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, GET_DUNGEON_REWARD_LIST, 2520, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 PayloadLength;
    UInt32 DungeonID;
    UInt32 Unknown2;
    // S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY Data[0];
)

CLIENT_PROTOCOL(S2C, NFY_DUNGEON_COMPLETE_INFO, 2521, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt64 Unknown2;
    UInt8 Unknown3;
    UInt64 Unknown4;
    UInt8 Unknown5;
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_2522, 2522, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_2522, 2522, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1;
    UInt32 Unknown2[4];
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_2571, 2571, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_UNKNOWN_2571_INDEX,
    UInt32 Unknown[3];
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_2571, 2571, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    S2C_DATA_UNKNOWN_2571_INDEX Data[0];
)

// C2S_COMMAND(C2S_GET_CASHSHOP_LIST, 2627)
// S2C_COMMAND(S2C_GET_CASHSHOP_LIST, 2627)

CLIENT_PROTOCOL(C2S, CHANGE_CHARACTER_SKILL_LEVEL, 2646, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt16 CurrentSkillLevel;
    UInt16 TargetSkillLevel;
)

CLIENT_PROTOCOL(S2C, CHANGE_CHARACTER_SKILL_LEVEL, 2646, 0000,
    S2C_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(C2S, ADD_CHARACTER_STATS, 2648, 0000,
    C2S_DATA_SIGNATURE;
    Int32 Stats[3];
    Int32 StatsAddition[3];
)

CLIENT_PROTOCOL(S2C, ADD_CHARACTER_STATS, 2648, 0000,
    S2C_DATA_SIGNATURE;
    Int32 Result;
    Int32 Stats[3];
)

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX, 2797, 0000,
    C2S_DATA_SIGNATURE;
    UInt8 Index;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OPEN_GIFTBOX_ITEM,
    UInt64 ItemID;
    UInt64 ItemOptions;
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX, 2797, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    S2C_DATA_OPEN_GIFTBOX_ITEM Data[0];
)

CLIENT_PROTOCOL(C2S, SORT_INVENTORY, 2800, CHECK_OST,
    C2S_DATA_SIGNATURE;
    UInt16 Count;
    UInt8 UnknownSlotIndices[1];
)

CLIENT_PROTOCOL(S2C, SORT_INVENTORY, 2800, CHECK_OST,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_OPEN_GIFTBOX_ITEMSLOT,
    UInt16 InventorySlotIndex;
    UInt16 Unknown;
)

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX_UNKNOWN, 2819, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 Count;
    C2S_DATA_OPEN_GIFTBOX_ITEMSLOT Data[1];
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX_UNKNOWN, 2819, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    UInt8 Unknown[43];
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_PASS_PROGRESS, 3001, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 GroupID;
    UInt32 MissionID;
    UInt32 Progress;
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_PASS_RESUME, 3002, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

#undef CLIENT_PROTOCOL_ENUM
#undef CLIENT_PROTOCOL_STRUCT
#undef CLIENT_PROTOCOL
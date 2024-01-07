#include "MasterDB.h"

static StatementRef kStatementTable[MASTERDB_STATEMENT_COUNT];

Void MasterDBPrepareStatements(
	DatabaseRef Database
) {
#define STATEMENT(__NAME__, __QUERY__)						                \
	LogMessageFormat(LOG_LEVEL_INFO, "Create Statement: %s", #__NAME__);	\
	kStatementTable[__NAME__] = DatabaseCreateStatement(	                \
    	Database,											                \
    	__QUERY__											                \
	);
#include "MasterDBStatements.h"
}

StatementRef MasterDBGetStatement(
	DatabaseRef Database,
	Int32 StatementID
) {
	assert(0 <= StatementID && StatementID < MASTERDB_STATEMENT_COUNT);

	return kStatementTable[StatementID];
}

Bool MasterDBGetOrCreateAccount(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Account
) {
    if (MasterDBSelectAccountByID(Database, Account)) return true;

    DatabaseBeginTransaction(Database);
    {
        if (!MasterDBInsertAccount(Database, Account)) goto error;
        if (!MasterDBInsertSubpassword(Database, Account)) goto error;

        Account->AccountID = DatabaseGetLastInsertID(Database);
    }
    DatabaseCommitTransaction(Database);

    if (!MasterDBSelectAccountByID(Database, Account)) goto error;

    return true;

error:
    DatabaseRollbackTransaction(Database);
    return false;
}

Bool MasterDBInsertAccount(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_INSERT_ACCOUNT);
    StatementBindParameterInt32(Statement, 0, Data->AccountID);
    StatementBindParameterBinary(Statement, 1, &Data->AchievementData, sizeof(MASTERDB_DATA_ACCOUNT_ACHIEVEMENT_DATA));
    StatementBindParameterBinary(Statement, 2, &Data->WarehouseData, sizeof(MASTERDB_DATA_ACCOUNT_WAREHOUSE_DATA));
    StatementBindParameterBinary(Statement, 3, &Data->MeritData, sizeof(MASTERDB_DATA_ACCOUNT_MERIT_DATA));
    StatementBindParameterBinary(Statement, 4, &Data->CollectionData, sizeof(MASTERDB_DATA_ACCOUNT_COLLECTION_DATA));

    if (!StatementExecute(Statement)) return false;

    return true;
}

Bool MasterDBSelectAccountByID(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_ACCOUNT_BY_ID);
    StatementBindParameterInt32(Statement, 0, Data->AccountID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Data->AccountID);
    StatementReadResultString(Statement, 1, MAX_ADDRESSIP_LENGTH, Data->SessionIP);
    StatementReadResultTimestamp(Statement, 2, &Data->SessionTimeout);
    StatementReadResultInt32(Statement, 3, &Data->CharacterSlotID);
    StatementReadResultUInt64(Statement, 4, &Data->CharacterSlotOrder);
    StatementReadResultUInt32(Statement, 5, &Data->CharacterSlotFlags);
    StatementReadResultString(Statement, 6, MAX_SUBPASSWORD_LENGTH, Data->CharacterPassword);
    StatementReadResultUInt32(Statement, 7, &Data->CharacterQuestion);
    StatementReadResultString(Statement, 8, MAX_SUBPASSWORD_ANSWER_LENGTH, Data->CharacterAnswer);
    StatementReadResultBinary(Statement, 9, sizeof(MASTERDB_DATA_ACCOUNT_ACHIEVEMENT_DATA), &Data->AchievementData, NULL);
    StatementReadResultBinary(Statement, 10, sizeof(MASTERDB_DATA_ACCOUNT_WAREHOUSE_DATA), &Data->WarehouseData, NULL);
    StatementReadResultBinary(Statement, 11, sizeof(MASTERDB_DATA_ACCOUNT_MERIT_DATA), &Data->MeritData, NULL);
    StatementReadResultBinary(Statement, 12, sizeof(MASTERDB_DATA_ACCOUNT_COLLECTION_DATA), &Data->CollectionData, NULL);
    StatementReadResultTimestamp(Statement, 13, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 14, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}

Bool MasterDBUpdateAccountSession(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_SESSION);
    StatementBindParameterString(Statement, 0, Data->SessionIP);
    StatementBindParameterUInt64(Statement, 1, Data->SessionTimeout);
    StatementBindParameterInt32(Statement, 2, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountCharacterSlot(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_CHARACTER_SLOT);
    StatementBindParameterInt32(Statement, 0, Data->CharacterSlotID);
    StatementBindParameterUInt64(Statement, 1, Data->CharacterSlotOrder);
    StatementBindParameterUInt32(Statement, 2, Data->CharacterSlotFlags);
    StatementBindParameterInt32(Statement, 3, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountCharacterPassword(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_CHARACTER_PASSWORD);
    StatementBindParameterString(Statement, 0, Data->CharacterPassword);
    StatementBindParameterUInt32(Statement, 1, Data->CharacterQuestion);
    StatementBindParameterString(Statement, 2, Data->CharacterAnswer);
    StatementBindParameterInt32(Statement, 3, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountAchievementData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_ACHIEVEMENT_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->AchievementData, sizeof(MASTERDB_DATA_ACCOUNT_ACHIEVEMENT_DATA));
    StatementBindParameterInt32(Statement, 1, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountWarehouseData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_WAREHOUSE_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->WarehouseData, sizeof(MASTERDB_DATA_ACCOUNT_WAREHOUSE_DATA));
    StatementBindParameterInt32(Statement, 1, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountMeritData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_MERIT_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->MeritData, sizeof(MASTERDB_DATA_ACCOUNT_MERIT_DATA));
    StatementBindParameterInt32(Statement, 1, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountCollectionData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_COLLECTION_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->MeritData, sizeof(MASTERDB_DATA_ACCOUNT_MERIT_DATA));
    StatementBindParameterInt32(Statement, 1, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBInsertCharacter(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_INSERT_CHARACTER);
    StatementBindParameterInt32(Statement, 0, Data->AccountID);
    StatementBindParameterString(Statement, 1, Data->Name);
    StatementBindParameterUInt8(Statement, 2, Data->Index);
    StatementBindParameterBinary(Statement, 3, &Data->CharacterData, sizeof(struct _RTCharacterInfo));
    StatementBindParameterBinary(Statement, 4, &Data->EquipmentData, sizeof(struct _RTCharacterEquipmentInfo));
    StatementBindParameterBinary(Statement, 5, &Data->InventoryData, sizeof(struct _RTCharacterInventoryInfo));
    StatementBindParameterBinary(Statement, 6, &Data->SkillSlotData, sizeof(struct _RTCharacterSkillSlotInfo));
    StatementBindParameterBinary(Statement, 7, &Data->QuickSlotData, sizeof(struct _RTCharacterQuickSlotInfo));
    StatementBindParameterBinary(Statement, 8, &Data->QuestSlotData, sizeof(struct _RTCharacterQuestSlotInfo));
    StatementBindParameterBinary(Statement, 9, &Data->QuestFlagData, sizeof(struct _RTCharacterQuestFlagInfo));
    StatementBindParameterBinary(Statement, 10, &Data->AchievementData, sizeof(GAME_DATA_CHARACTER_ACHIEVEMENT));
    StatementBindParameterBinary(Statement, 11, &Data->EssenceAbilityData, sizeof(struct _RTCharacterEssenceAbilityInfo));
    StatementBindParameterBinary(Statement, 12, &Data->BlendedAbilityData, sizeof(GAME_DATA_CHARACTER_BLENDEDABILITY));
    StatementBindParameterBinary(Statement, 13, &Data->HonorMedalData, sizeof(GAME_DATA_CHARACTER_HONORMEDAL));
    StatementBindParameterBinary(Statement, 14, &Data->OverlordData, sizeof(GAME_DATA_CHARACTER_OVERLORD));
    StatementBindParameterBinary(Statement, 15, &Data->TransformData, sizeof(GAME_DATA_CHARACTER_TRANSFORM));
    StatementBindParameterBinary(Statement, 16, &Data->TranscendenceData, sizeof(GAME_DATA_CHARACTER_TRANSCENDENCE));
    StatementBindParameterBinary(Statement, 17, &Data->MercenaryData, sizeof(GAME_DATA_CHARACTER_MERCENARY));
    StatementBindParameterBinary(Statement, 18, &Data->CraftData, sizeof(GAME_DATA_CHARACTER_CRAFT));

    if (!StatementExecute(Statement)) return false;

    return true;
}

Bool MasterDBSelectCharacterByID(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_BY_ID);
    StatementBindParameterInt32(Statement, 0, Data->CharacterID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Data->AccountID);
    StatementReadResultInt32(Statement, 1, &Data->CharacterID);
    StatementReadResultString(Statement, 2, MAX_CHARACTER_NAME_LENGTH, Data->Name);
    StatementReadResultUInt8(Statement, 3, &Data->Index);
    StatementReadResultBinary(Statement, 4, sizeof(struct _RTCharacterInfo), &Data->CharacterData, NULL);
    StatementReadResultBinary(Statement, 5, sizeof(struct _RTCharacterEquipmentInfo), &Data->EquipmentData, NULL);
    StatementReadResultBinary(Statement, 6, sizeof(struct _RTCharacterInventoryInfo), &Data->InventoryData, NULL);
    StatementReadResultBinary(Statement, 7, sizeof(struct _RTCharacterSkillSlotInfo), &Data->SkillSlotData, NULL);
    StatementReadResultBinary(Statement, 8, sizeof(struct _RTCharacterQuickSlotInfo), &Data->QuickSlotData, NULL);
    StatementReadResultBinary(Statement, 9, sizeof(struct _RTCharacterQuestSlotInfo), &Data->QuestSlotData, NULL);
    StatementReadResultBinary(Statement, 10, sizeof(struct _RTCharacterQuestFlagInfo), &Data->QuestFlagData, NULL);
    StatementReadResultBinary(Statement, 11, sizeof(GAME_DATA_CHARACTER_ACHIEVEMENT), &Data->AchievementData, NULL);
    StatementReadResultBinary(Statement, 12, sizeof(struct _RTCharacterEssenceAbilityInfo), &Data->EssenceAbilityData, NULL);
    StatementReadResultBinary(Statement, 13, sizeof(GAME_DATA_CHARACTER_BLENDEDABILITY), &Data->BlendedAbilityData, NULL);
    StatementReadResultBinary(Statement, 14, sizeof(GAME_DATA_CHARACTER_HONORMEDAL), &Data->HonorMedalData, NULL);
    StatementReadResultBinary(Statement, 15, sizeof(GAME_DATA_CHARACTER_OVERLORD), &Data->OverlordData, NULL);
    StatementReadResultBinary(Statement, 16, sizeof(GAME_DATA_CHARACTER_TRANSFORM), &Data->TransformData, NULL);
    StatementReadResultBinary(Statement, 17, sizeof(GAME_DATA_CHARACTER_TRANSCENDENCE), &Data->TranscendenceData, NULL);
    StatementReadResultBinary(Statement, 18, sizeof(GAME_DATA_CHARACTER_MERCENARY), &Data->MercenaryData, NULL);
    StatementReadResultBinary(Statement, 19, sizeof(GAME_DATA_CHARACTER_CRAFT), &Data->CraftData, NULL);
    StatementReadResultTimestamp(Statement, 20, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 21, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}

StatementRef MasterDBSelectCharacterByAccount(
    DatabaseRef Database,
    Int32 AccountID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_BY_ACCOUNT);
    StatementBindParameterInt32(Statement, 0, AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBSelectCharacterFetchNext(
    DatabaseRef Database,
    StatementRef Statement,
    MASTERDB_DATA_CHARACTER* Result
) {
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Result->AccountID);
    StatementReadResultInt32(Statement, 1, &Result->CharacterID);
    StatementReadResultString(Statement, 2, MAX_CHARACTER_NAME_LENGTH, Result->Name);
    StatementReadResultUInt8(Statement, 3, &Result->Index);
    StatementReadResultBinary(Statement, 4, sizeof(struct _RTCharacterInfo), &Result->CharacterData, NULL);
    StatementReadResultBinary(Statement, 5, sizeof(struct _RTCharacterEquipmentInfo), &Result->EquipmentData, NULL);
    StatementReadResultBinary(Statement, 6, sizeof(struct _RTCharacterInventoryInfo), &Result->InventoryData, NULL);
    StatementReadResultBinary(Statement, 7, sizeof(struct _RTCharacterSkillSlotInfo), &Result->SkillSlotData, NULL);
    StatementReadResultBinary(Statement, 8, sizeof(struct _RTCharacterQuickSlotInfo), &Result->QuickSlotData, NULL);
    StatementReadResultBinary(Statement, 9, sizeof(struct _RTCharacterQuestSlotInfo), &Result->QuestSlotData, NULL);
    StatementReadResultBinary(Statement, 10, sizeof(struct _RTCharacterQuestFlagInfo), &Result->QuestFlagData, NULL);
    StatementReadResultBinary(Statement, 11, sizeof(GAME_DATA_CHARACTER_ACHIEVEMENT), &Result->AchievementData, NULL);
    StatementReadResultBinary(Statement, 12, sizeof(struct _RTCharacterEssenceAbilityInfo), &Result->EssenceAbilityData, NULL);
    StatementReadResultBinary(Statement, 13, sizeof(GAME_DATA_CHARACTER_BLENDEDABILITY), &Result->BlendedAbilityData, NULL);
    StatementReadResultBinary(Statement, 14, sizeof(GAME_DATA_CHARACTER_HONORMEDAL), &Result->HonorMedalData, NULL);
    StatementReadResultBinary(Statement, 15, sizeof(GAME_DATA_CHARACTER_OVERLORD), &Result->OverlordData, NULL);
    StatementReadResultBinary(Statement, 16, sizeof(GAME_DATA_CHARACTER_TRANSFORM), &Result->TransformData, NULL);
    StatementReadResultBinary(Statement, 17, sizeof(GAME_DATA_CHARACTER_TRANSCENDENCE), &Result->TranscendenceData, NULL);
    StatementReadResultBinary(Statement, 18, sizeof(GAME_DATA_CHARACTER_MERCENARY), &Result->MercenaryData, NULL);
    StatementReadResultBinary(Statement, 19, sizeof(GAME_DATA_CHARACTER_CRAFT), &Result->CraftData, NULL);
    StatementReadResultTimestamp(Statement, 20, &Result->CreatedAt);
    StatementReadResultTimestamp(Statement, 21, &Result->UpdatedAt);

    return true;
}

StatementRef MasterDBSelectCharacterIndexByAccount(
    DatabaseRef Database,
    Int32 AccountID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_INDEX_BY_ACCOUNT);
    StatementBindParameterInt32(Statement, 0, AccountID);

    if (StatementExecute(Statement)) return Statement;

    return NULL;
}

Bool MasterDBSelectCharacterIndexFetchNext(
    DatabaseRef Database,
    StatementRef Statement,
    MASTERDB_DATA_CHARACTER_INDEX* Result
) {
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Result->AccountID);
    StatementReadResultInt32(Statement, 1, &Result->CharacterID);
    StatementReadResultString(Statement, 2, MAX_CHARACTER_NAME_LENGTH, Result->Name);
    StatementReadResultUInt8(Statement, 3, &Result->Index);
    StatementReadResultBinary(Statement, 4, sizeof(struct _RTCharacterInfo), &Result->CharacterData, NULL);
    StatementReadResultBinary(Statement, 5, sizeof(struct _RTCharacterEquipmentInfo), &Result->EquipmentData, NULL);
    StatementReadResultTimestamp(Statement, 6, &Result->CreatedAt);
    StatementReadResultTimestamp(Statement, 7, &Result->UpdatedAt);

    return true;
}

Bool MasterDBSelectCharacterNameCount(
    DatabaseRef Database,
    CString Name,
    Int32* Result
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_NAME_COUNT);
    StatementBindParameterString(Statement, 0, Name);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, Result);
    StatementFlushResults(Statement);
    
    return true;
}

Bool MasterDBUpdateCharacterInfo(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterInfoRef CharacterInfo
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_DATA);
    StatementBindParameterBinary(Statement, 0, CharacterInfo, sizeof(struct _RTCharacterInfo));
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterEquipmentData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterEquipmentInfoRef EquipmentInfo
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_EQUIPMENT_DATA);
    StatementBindParameterBinary(Statement, 0, EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterInventoryData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterInventoryInfoRef InventoryInfo
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_INVENTORY_DATA);
    StatementBindParameterBinary(Statement, 0, InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterSkillSlotData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterSkillSlotInfoRef SkillSlotInfo
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_SKILLSLOT_DATA);
    StatementBindParameterBinary(Statement, 0, SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterQuickSlotData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterQuickSlotInfoRef QuickSlotInfo
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_QUICKSLOT_DATA);
    StatementBindParameterBinary(Statement, 0, QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterQuestSlotData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterQuestSlotInfoRef QuestSlotInfo
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_QUEST_DATA);
    StatementBindParameterBinary(Statement, 0, QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterQuestFlagData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterQuestFlagInfoRef QuestFlagInfo
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_QUESTFLAG_DATA);
    StatementBindParameterBinary(Statement, 0, QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterAchievementData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_ACHIEVEMENT_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->AchievementData, sizeof(GAME_DATA_CHARACTER_ACHIEVEMENT));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterEssenceAbilityData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_ESSENCE_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->EssenceAbilityData, sizeof(struct _RTCharacterEssenceAbilityInfo));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterBlendedAbilityData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_BLENDED_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->BlendedAbilityData, sizeof(GAME_DATA_CHARACTER_BLENDEDABILITY));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterHonorMedalData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_HONORMEDAL_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->HonorMedalData, sizeof(GAME_DATA_CHARACTER_HONORMEDAL));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterOverlordData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_OVERLORD_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->OverlordData, sizeof(GAME_DATA_CHARACTER_OVERLORD));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterTransformData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_TRANSFORM_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->TransformData, sizeof(GAME_DATA_CHARACTER_TRANSFORM));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterTranscendenceData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_TRANSCENDENCE_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->TranscendenceData, sizeof(GAME_DATA_CHARACTER_TRANSCENDENCE));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterMercenaryData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_MERCENARY_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->MercenaryData, sizeof(GAME_DATA_CHARACTER_MERCENARY));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateCharacterCraftData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_CHARACTER_CRAFT_DATA);
    StatementBindParameterBinary(Statement, 0, &Data->CraftData, sizeof(GAME_DATA_CHARACTER_CRAFT));
    StatementBindParameterInt32(Statement, 1, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBDeleteCharacter(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_DELETE_CHARACTER);
    StatementBindParameterInt32(Statement, 0, Data->CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBInsertSubpassword(
    DatabaseRef Database,
    Int32 AccountID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERBD_INSERT_SUBPASSWORD);
    StatementBindParameterInt32(Statement, 0, AccountID);

    if (!StatementExecute(Statement)) return false;

    return true;
}

Bool MasterDBSelectSubpassword(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERBD_SELECT_SUBPASSWORD);
    StatementBindParameterInt32(Statement, 0, Data->AccountID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Data->AccountID);
    StatementReadResultString(Statement, 1, MAX_SUBPASSWORD_LENGTH, Data->EquipmentPassword);
    StatementReadResultUInt32(Statement, 2, &Data->EquipmentQuestion);
    StatementReadResultString(Statement, 3, MAX_SUBPASSWORD_ANSWER_LENGTH, Data->EquipmentAnswer);
    StatementReadResultBool(Statement, 4, &Data->EquipmentLocked);
    StatementReadResultString(Statement, 5, MAX_SUBPASSWORD_LENGTH, Data->WarehousePassword);
    StatementReadResultUInt32(Statement, 6, &Data->WarehouseQuestion);
    StatementReadResultString(Statement, 7, MAX_SUBPASSWORD_ANSWER_LENGTH, Data->WarehouseAnswer);
    StatementReadResultBool(Statement, 8, &Data->WarehouseLocked);
    StatementReadResultTimestamp(Statement, 9, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 10, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}

Bool MasterDBUpdateSubpasswordEquipment(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_SUBPASSWORD_EQUIPMENT);
    StatementBindParameterString(Statement, 0, Data->EquipmentPassword);
    StatementBindParameterUInt32(Statement, 1, Data->EquipmentQuestion);
    StatementBindParameterString(Statement, 2, Data->EquipmentAnswer);
    StatementBindParameterBool(Statement, 3, Data->EquipmentLocked);
    StatementBindParameterInt32(Statement, 4, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateSubpasswordWarehouse(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_SUBPASSWORD_WAREHOUSE);
    StatementBindParameterString(Statement, 0, Data->WarehousePassword);
    StatementBindParameterUInt32(Statement, 1, Data->WarehouseQuestion);
    StatementBindParameterString(Statement, 2, Data->WarehouseAnswer);
    StatementBindParameterBool(Statement, 3, Data->WarehouseLocked);
    StatementBindParameterInt32(Statement, 4, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBInsertService(
    DatabaseRef Database,
    Int32 AccountID,
    UInt32 ServiceType,
    Timestamp Duration
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_INSERT_SERVICE);
    StatementBindParameterInt32(Statement, 0, AccountID);
    StatementBindParameterUInt32(Statement, 1, ServiceType);
    StatementBindParameterUInt64(Statement, 2, Duration);

    if (!StatementExecute(Statement)) return false;

    return true;
}

Bool MasterDBSelectCurrentActiveServiceByAccount(
    DatabaseRef Database,
    MASTERDB_DATA_SERVICE* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CURRENT_ACTIVE_SERVICE_BY_ACCOUNT);
    StatementBindParameterInt32(Statement, 0, Data->AccountID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Data->ID);
    StatementReadResultInt32(Statement, 1, &Data->AccountID);
    StatementReadResultUInt32(Statement, 2, &Data->ServiceType);
    StatementReadResultTimestamp(Statement, 3, &Data->StartedAt);
    StatementReadResultTimestamp(Statement, 4, &Data->ExpiredAt);
    StatementReadResultTimestamp(Statement, 5, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 6, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}

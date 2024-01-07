#pragma once

#include "Base.h"
#include "MasterDBProtocol.h"

EXTERN_C_BEGIN

enum {
#define STATEMENT(__NAME__, __QUERY__) \
	__NAME__,
#include "MasterDBStatements.h"

	MASTERDB_STATEMENT_COUNT,
};

Void MasterDBPrepareStatements(
	DatabaseRef Database
);

StatementRef MasterDBGetStatement(
	DatabaseRef Database,
	Int32 StatementID
);

Bool MasterDBGetOrCreateAccount(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Account
);

Bool MasterDBInsertAccount(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBSelectAccountByID(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBUpdateAccountSession(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBUpdateAccountCharacterSlot(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBUpdateAccountCharacterPassword(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBUpdateAccountAchievementData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBUpdateAccountWarehouseData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBUpdateAccountMeritData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBUpdateAccountCollectionData(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
);

Bool MasterDBInsertCharacter(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBSelectCharacterByID(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

StatementRef MasterDBSelectCharacterByAccount(
    DatabaseRef Database,
    Int32 AccountID
);

Bool MasterDBSelectCharacterFetchNext(
    DatabaseRef Database,
    StatementRef Statement,
    MASTERDB_DATA_CHARACTER* Result
);

StatementRef MasterDBSelectCharacterIndexByAccount(
    DatabaseRef Database,
    Int32 AccountID
);

Bool MasterDBSelectCharacterIndexFetchNext(
    DatabaseRef Database,
    StatementRef Statement,
    MASTERDB_DATA_CHARACTER_INDEX* Result
);

Bool MasterDBSelectCharacterNameCount(
    DatabaseRef Database,
    CString Name,
    Int32* Result
);

Bool MasterDBUpdateCharacterInfo(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterInfoRef CharacterInfo
);

Bool MasterDBUpdateCharacterEquipmentData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterEquipmentInfoRef EquipmentInfo
);

Bool MasterDBUpdateCharacterInventoryData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterInventoryInfoRef InventoryInfo
);

Bool MasterDBUpdateCharacterSkillSlotData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterSkillSlotInfoRef SkillSlotInfo
);

Bool MasterDBUpdateCharacterQuickSlotData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterQuickSlotInfoRef QuickSlotInfo
);

Bool MasterDBUpdateCharacterQuestSlotData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterQuestSlotInfoRef QuestSlotInfo
);

Bool MasterDBUpdateCharacterQuestFlagData(
    DatabaseRef Database,
    Int32 CharacterID,
    RTCharacterQuestFlagInfoRef QuestFlagInfo
);

Bool MasterDBUpdateCharacterAchievementData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterEssenceAbilityData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterBlendedAbilityData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterHonorMedalData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterOverlordData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterTransformData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterTranscendenceData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterMercenaryData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBUpdateCharacterCraftData(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBDeleteCharacter(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
);

Bool MasterDBInsertSubpassword(
    DatabaseRef Database,
    Int32 AccountID
);

Bool MasterDBSelectSubpassword(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
);

Bool MasterDBUpdateSubpasswordEquipment(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
);

Bool MasterDBUpdateSubpasswordWarehouse(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
);

Bool MasterDBInsertService(
    DatabaseRef Database,
    Int32 AccountID,
    UInt32 ServiceType,
    Timestamp Duration
);

Bool MasterDBSelectCurrentActiveServiceByAccount(
    DatabaseRef Database,
    MASTERDB_DATA_SERVICE* Data
);

EXTERN_C_END
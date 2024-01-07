#ifndef STATEMENT(__NAME__, __QUERY__)
#define STATEMENT(__NAME__, __QUERY__)
#endif

STATEMENT(
    MASTERDB_INSERT_ACCOUNT, 
    "INSERT INTO `accounts` (`account_id`, `achievement_data`, `warehouse_data`, `merit_data`, `collection_data`) VALUES (?, ?, ?, ?, ?);"
)

STATEMENT(
    MASTERDB_SELECT_ACCOUNT_BY_ID, 
    "SELECT `account_id`, `session_ip`, `session_timeout`, `character_slot_id`, `character_slot_order`, `character_slot_flags`, `character_password`, `character_question`, `character_answer`, `achievement_data`, `warehouse_data`, `merit_data`, `collection_data`, `created_at`, `updated_at` FROM `accounts` WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_SESSION,
    "UPDATE `accounts` SET `session_ip` = ?, `session_timeout` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_CHARACTER_SLOT,
    "UPDATE `accounts` SET `character_slot_id` = ?, `character_slot_order` = ?, `character_slot_flags` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_CHARACTER_PASSWORD,
    "UPDATE `accounts` SET `character_password` = ?, `character_question` = ?, `character_answer` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_ACHIEVEMENT_DATA,
    "UPDATE `accounts` SET `achievement_data` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_WAREHOUSE_DATA,
    "UPDATE `accounts` SET `warehouse_data` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_MERIT_DATA,
    "UPDATE `accounts` SET `merit_data` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_COLLECTION_DATA,
    "UPDATE `accounts` SET `collection_data` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_INSERT_CHARACTER,
    "INSERT INTO `characters` (`account_id`, `name`, `index`, `character_data`, `equipment_data`, `inventory_data`, `skillslot_data`, `quickslot_data`, `quest_data`, `questflag_data`, `achievement_data`, `essence_data`, `blended_data`, `honormedal_data`, `overlord_data`, `transform_data`, `transcendence_data`, `mercenary_data`, `craft_data`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_BY_ID,
    "SELECT `account_id`, `id`, `name`, `index`, `character_data`, `equipment_data`, `inventory_data`, `skillslot_data`, `quickslot_data`, `quest_data`, `questflag_data`, `achievement_data`, `essence_data`, `blended_data`, `honormedal_data`, `overlord_data`, `transform_data`, `transcendence_data`, `mercenary_data`, `craft_data`, `created_at`, `updated_at` FROM `characters` WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_BY_ACCOUNT,
    "SELECT `account_id`, `id`, `name`, `index`, `character_data`, `equipment_data`, `inventory_data`, `skillslot_data`, `quickslot_data`, `quest_data`, `questflag_data`, `achievement_data`, `essence_data`, `blended_data`, `honormedal_data`, `overlord_data`, `transform_data`, `transcendence_data`, `mercenary_data`, `craft_data`, `created_at`, `updated_at` FROM `characters` WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_INDEX_BY_ACCOUNT,
    "SELECT `account_id`, `id`, `name`, `index`, `character_data`, `equipment_data`, `created_at`, `updated_at` FROM `characters` WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_NAME_COUNT,
    "SELECT COUNT(*) FROM `characters` WHERE `name` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_DATA,
    "UPDATE `characters` SET `character_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_EQUIPMENT_DATA,
    "UPDATE `characters` SET `equipment_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_INVENTORY_DATA,
    "UPDATE `characters` SET `inventory_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_SKILLSLOT_DATA,
    "UPDATE `characters` SET `skillslot_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_QUICKSLOT_DATA,
    "UPDATE `characters` SET `quickslot_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_QUEST_DATA,
    "UPDATE `characters` SET `quest_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_QUESTFLAG_DATA,
    "UPDATE `characters` SET `questflag_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_ACHIEVEMENT_DATA,
    "UPDATE `characters` SET `achievement_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_ESSENCE_DATA,
    "UPDATE `characters` SET `essence_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_BLENDED_DATA,
    "UPDATE `characters` SET `blended_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_HONORMEDAL_DATA,
    "UPDATE `characters` SET `honormedal_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_OVERLORD_DATA,
    "UPDATE `characters` SET `overlord_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_TRANSFORM_DATA,
    "UPDATE `characters` SET `transform_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_TRANSCENDENCE_DATA,
    "UPDATE `characters` SET `transcendence_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_MERCENARY_DATA,
    "UPDATE `characters` SET `mercenary_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_CHARACTER_CRAFT_DATA,
    "UPDATE `characters` SET `craft_data` = ? WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_DELETE_CHARACTER,
    "DELETE FROM `characters` WHERE `id` = ?;"
)

STATEMENT(
    MASTERBD_INSERT_SUBPASSWORD,
    "INSERT INTO `subpasswords` (`account_id`) VALUES (?);"
)

STATEMENT(
    MASTERBD_SELECT_SUBPASSWORD,
    "SELECT `account_id`, `equipment_password`, `equipment_question`, `equipment_answer`, `equipment_locked`, `warehouse_password`, `warehouse_question`, `warehouse_answer`, `warehouse_locked`, `created_at`, `updated_at` FROM `subpasswords` WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_SUBPASSWORD_EQUIPMENT,
    "UPDATE `subpasswords` SET `equipment_password` = ?, `equipment_question` = ?, `equipment_answer` = ?, `equipment_locked` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_SUBPASSWORD_WAREHOUSE,
    "UPDATE `subpasswords` SET `warehouse_password` = ?, `warehouse_question` = ?, `warehouse_answer` = ?, `warehouse_locked` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_INSERT_SERVICE,
    "INSERT INTO `services` (`account_id`, `service_type`, `started_at`, `expired_at`) VALUES (?, ?, UNIX_TIMESTAMP(), UNIX_TIMESTAMP() + ?);"
)

STATEMENT(
    MASTERDB_SELECT_CURRENT_ACTIVE_SERVICE_BY_ACCOUNT,
    "SELECT `id`, `account_id`, `service_type`, `started_at`, `expired_at`, `created_at`, `updated_at` FROM `services` WHERE `started_at` <= UNIX_TIMESTAMP() AND `expired_at` >= UNIX_TIMESTAMP() AND `account_id` = ?;"
)

#undef STATEMENT
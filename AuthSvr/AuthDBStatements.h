#ifndef STATEMENT(__NAME__, __QUERY__)
#define STATEMENT(__NAME__, __QUERY__)
#endif

STATEMENT(
    AUTHDB_INSERT_ACCOUNT, 
    "INSERT INTO `accounts` (`username`, `email`, `salt`, `hash`) VALUES(?, ?, ?, ?);"
)

STATEMENT(
    AUTHDB_SELECT_ACCOUNT_BY_ID, 
    "SELECT * FROM `accounts` WHERE `id` = ? AND `deleted_at` IS NULL;"
)

STATEMENT(
    AUTHDB_SELECT_ACCOUNT_BY_USERNAME,
    "SELECT * FROM `accounts` WHERE `username` = ? AND `deleted_at` IS NULL;"
)

STATEMENT(
    AUTHDB_UPSERT_SESSION,
    "INSERT INTO `sessions` (`account_id`, `online`, `session_key`, `session_ip`, `session_at`) VALUES (?, ?, ?, ?, UNIX_TIMESTAMP(CURRENT_TIMESTAMP)) ON DUPLICATE KEY UPDATE `online` = ?, `session_key` = ?, `session_ip` = ?, `session_at` = UNIX_TIMESTAMP(CURRENT_TIMESTAMP);"
)

STATEMENT(
    AUTHDB_SELECT_SESSION,
    "SELECT * FROM `sessions` WHERE `account_id` = ?;"
)

STATEMENT(
    AUTHDB_INSERT_BLACKLIST_ACCOUNT,
    "INSERT INTO `blacklists` (`account_id`, `description`) VALUES (?, ?);"
)

STATEMENT(
    AUTHDB_INSERT_BLACKLIST_ADDRESS,
    "INSERT INTO `blacklists` (`address_ip`, `description`) VALUES (?, ?);"
)

STATEMENT(
    AUTHDB_SELECT_BLACKLIST_BY_ID,
    "SELECT * FROM `blacklists` WHERE `account_id` = ?;"
)

STATEMENT(
    AUTHDB_SELECT_BLACKLIST_BY_IP,
    "SELECT * FROM `blacklists` WHERE `address_ip` = ?;"
)

STATEMENT(
    AUTHDB_UPSERT_CHARACTER,
    "INSERT INTO `characters` (`account_id`, `server_id`, `character_count`) VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE `character_count` = ?;"
)

STATEMENT(
    AUTHDB_SELECT_CHARACTER_BY_ID,
    "SELECT * FROM `characters` WHERE `account_id` = ?;"
)

#undef STATEMENT
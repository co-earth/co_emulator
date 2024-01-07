USE `master`;

DROP TABLE IF EXISTS `accounts`;

CREATE TABLE `accounts` (
    `account_id` INT NOT NULL,

    `session_ip` VARCHAR(15) NOT NULL DEFAULT '127.0.0.1',
    `session_timeout` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    `character_slot_id` INT NOT NULL DEFAULT 0,
    `character_slot_order` BIGINT UNSIGNED NOT NULL DEFAULT 0x0123456789ABCDEF,
    `character_slot_flags` INT UNSIGNED NOT NULL DEFAULT 0x3F,
    `character_password` VARCHAR(11) DEFAULT NULL,
    `character_question` INT UNSIGNED NOT NULL DEFAULT 0,
    `character_answer` VARCHAR(17) DEFAULT NULL,

    `achievement_data` BLOB NOT NULL,
    `warehouse_data` BLOB NOT NULL,
    `merit_data` BLOB NOT NULL,
    `collection_data` BLOB NOT NULL,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY (`account_id`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;

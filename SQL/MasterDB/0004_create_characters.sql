USE `master`;

DROP TABLE IF EXISTS `characters`;

CREATE TABLE `characters` (
    `account_id` INT NOT NULL,
    
    `id` INT NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(16) NOT NULL,
    `index` TINYINT UNSIGNED NOT NULL,

    `character_data` BLOB NOT NULL,
    `equipment_data` BLOB NOT NULL,
    `inventory_data` BLOB NOT NULL,
    `skillslot_data` BLOB NOT NULL,
    `quickslot_data` BLOB NOT NULL,
    `quest_data` BLOB NOT NULL,
    `questflag_data` BLOB NOT NULL,
    `achievement_data` BLOB NOT NULL,
    `essence_data` BLOB NOT NULL,
    `blended_data` BLOB NOT NULL,
    `honormedal_data` BLOB NOT NULL,
    `overlord_data` BLOB NOT NULL,
    `transform_data` BLOB NOT NULL,
    `transcendence_data` BLOB NOT NULL,
    `mercenary_data` BLOB NOT NULL,
    `craft_data` BLOB NOT NULL,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY (`id`),
    UNIQUE (`account_id`, `index`),
    UNIQUE (`name`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;

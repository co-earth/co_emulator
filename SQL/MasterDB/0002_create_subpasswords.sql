USE `master`;

DROP TABLE IF EXISTS `subpasswords`;

CREATE TABLE `subpasswords` (
    `account_id` INT NOT NULL,

    `equipment_password` VARCHAR(11) DEFAULT NULL,
    `equipment_question` INT UNSIGNED NOT NULL DEFAULT 0,
    `equipment_answer` VARCHAR(17) DEFAULT NULL,
    `equipment_locked` BOOLEAN DEFAULT FALSE,

    `warehouse_password` VARCHAR(11) DEFAULT NULL,
    `warehouse_question` INT UNSIGNED NOT NULL DEFAULT 0,
    `warehouse_answer` VARCHAR(17) DEFAULT NULL,
    `warehouse_locked` BOOLEAN DEFAULT FALSE,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY (`account_id`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;

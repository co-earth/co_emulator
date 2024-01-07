USE `auth`;

DROP TABLE IF EXISTS `accounts`;

CREATE TABLE `accounts` (
    `id` INT NOT NULL AUTO_INCREMENT,

    `username` VARCHAR(16) NOT NULL,
    `email` VARCHAR(255) NOT NULL,
    `email_verified` BOOLEAN NOT NULL DEFAULT FALSE,
    `salt` BINARY(64) NOT NULL,
    `hash` BINARY(64) NOT NULL,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `deleted_at` BIGINT UNSIGNED DEFAULT NULL,

    PRIMARY KEY (`id`),
    UNIQUE (`username`),
    UNIQUE (`email`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;

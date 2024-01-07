#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

typedef struct {
    Int32 ID;
    
    Char Username[MAX_USERNAME_LENGTH];
    Char Email[MAX_EMAIL_LENGTH];
    Bool EmailVerified;
    UInt8 Salt[MAX_PASSWORD_SALT_LENGTH];
    UInt8 Hash[MAX_PASSWORD_HASH_LENGTH];

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
    Timestamp DeletedAt;
} AUTHDB_DATA_ACCOUNT;

typedef struct {
    Int32 AccountID;

    Bool Online;
    Char SessionKey[MAX_SESSIONKEY_LENGTH];
    Char SessionIP[MAX_ADDRESSIP_LENGTH];
    Timestamp SessionAt;

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} AUTHDB_DATA_SESSION;

typedef struct {
    Int32 ID;

    Int32 AccountID;
    Char AddressIP[MAX_ADDRESSIP_LENGTH];
    Char Description[MAX_DESCRIPTION_LENGTH];

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} AUTHDB_DATA_BLACKLIST;

typedef struct {
    Int32 AccountID;

    UInt8 ServerID;
    UInt8 CharacterCount;

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} AUTHDB_DATA_CHARACTER;

#pragma pack(pop)

EXTERN_C_END
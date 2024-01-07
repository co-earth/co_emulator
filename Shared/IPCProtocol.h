#pragma once

#include <NetLib/NetLib.h>
#include <RuntimeLib/RuntimeLib.h>
#include "Constants.h"
#include "GameProtocol.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
#define IPC_COMMAND(__NAME__, __VALUE__) \
	__NAME__ = __VALUE__,
#include "IPCCommands.h"
};

/* --- AUTH <> MASTER --- */

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 ServerID;
} IPC_DATA_AUTH_REQCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE;
	Bool Success;
} IPC_DATA_AUTH_ACKCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 WorldCount;
	UInt8 Data[0];
	/* IPC_DATA_AUTH_WORLD[WorldCount] */
} IPC_DATA_AUTH_NFYWORLDLIST;

typedef struct {
	UInt8 WorldID;
	UInt16 PlayerCount;
	UInt16 MaxPlayerCount;
	Char WorldHost[65];
	UInt16 WorldPort;
	UInt32 WorldType;
} IPC_DATA_AUTH_WORLD;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 ServerID;
	UInt8 WorldID;
	Int32 AccountID;
	UInt32 AuthKey;
	UInt16 EntityID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH];
} IPC_DATA_AUTH_VERIFYLINKS;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 ServerID;
	UInt8 WorldID;
	UInt8 Status;
} IPC_DATA_AUTH_VERIFYRESULT;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
} IPC_DATA_AUTH_REQVERIFYPASSWORD;

typedef struct {
	IPC_DATA_SIGNATURE;
	Bool Success;
} IPC_DATA_AUTH_ACKVERIFYPASSWORD;

/* --- WORLD <> MASTER --- */

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 WorldID;
	Char WorldHost[65];
	UInt16 WorldPort;
	UInt32 WorldType;
	UInt16 MaxPlayerCount;
} IPC_DATA_WORLD_REQCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 ServerID;
	Bool Success;
} IPC_DATA_WORLD_ACKCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt16 PlayerCount;
	UInt16 MaxPlayerCount;
} IPC_DATA_WORLD_NFYUSERLIST;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 ServerID;
	UInt8 WorldID;
	UInt32 AuthKey;
	UInt16 EntityID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
	GAME_DATA_ACCOUNT Account;
} IPC_DATA_WORLD_VERIFYLINKS;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 ServerID;
	UInt8 WorldID;
	Int32 AccountID;
	UInt8 Status;
} IPC_DATA_WORLD_VERIFYRESULT;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
} IPC_DATA_WORLD_REQGETCHARACTERS;

typedef struct {
	IPC_DATA_SIGNATURE;
	GAME_DATA_CHARACTER_INDEX Characters[MAX_CHARACTER_COUNT];
} IPC_DATA_WORLD_ACKGETCHARACTERS;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
} IPC_DATA_WORLD_REQPREMIUMSERVICE;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Bool HasService;
	UInt32 ServiceType;
	Timestamp StartedAt;
	Timestamp ExpiredAt;
} IPC_DATA_WORLD_ACKPREMIUMSERVICE;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	UInt8 SlotIndex;
	UInt8 NameLength;
	Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
	struct _RTCharacterInfo CharacterData;
    struct _RTCharacterEquipmentInfo CharacterEquipment;
	struct _RTCharacterInventoryInfo CharacterInventory;
	struct _RTCharacterSkillSlotInfo CharacterSkillSlots;
	struct _RTCharacterQuickSlotInfo CharacterQuickSlots;
} IPC_DATA_WORLD_REQCREATECHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt8 Status;
	UInt8 SlotIndex;
	GAME_DATA_CHARACTER_INDEX Character;
} IPC_DATA_WORLD_ACKCREATECHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
	Timestamp SessionTimeout;
} IPC_DATA_WORLD_UPDATE_ACCOUNT_SESSION_DATA;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Int32 CharacterSlotID;
	UInt64 CharacterSlotOrder;
	UInt32 CharacterSlotFlags;
} IPC_DATA_WORLD_UPDATE_ACCOUNT_CHARACTER_DATA;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Char CharacterPassword[MAX_SUBPASSWORD_LENGTH + 1];
	UInt32 CharacterQuestion;
	Char CharacterAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
} IPC_DATA_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Int32 CharacterID;
	UInt32 CharacterIndex;
} IPC_DATA_WORLD_REQGETCHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE;
	UInt32 CharacterIndex;
	Bool Success;
	GAME_DATA_CHARACTER Character;
} IPC_DATA_WORLD_ACKGETCHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Int32 CharacterID;
	UInt32 DBSyncMask;
    UInt32 DBSyncPriority;
	UInt8 Data[0];
} IPC_DATA_WORLD_REQDBSYNC;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Int32 CharacterID;
	UInt32 DBSyncMaskFailure;
    UInt32 DBSyncPriority;
} IPC_DATA_WORLD_ACKDBSYNC;

typedef struct {
	IPC_DATA_SIGNATURE;
	Int32 AccountID;
	Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
} IPC_DATA_WORLD_REQVERIFYPASSWORD;

typedef struct {
	IPC_DATA_SIGNATURE;
	Bool Success;
} IPC_DATA_WORLD_ACKVERIFYPASSWORD;

#pragma pack(pop)

EXTERN_C_END
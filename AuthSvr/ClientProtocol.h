#pragma once

#include <NetLib/NetLib.h>
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
#define  C2S_COMMAND(__NAME__, __VALUE__) \
	__NAME__ = __VALUE__,
#include "ClientCommands.h"
};

enum {
#define S2C_COMMAND(__NAME__, __VALUE__) \
	__NAME__ = __VALUE__,
#include "ClientCommands.h"
};

typedef struct {
	C2S_DATA_SIGNATURE;
	UInt32 AuthKey;
} C2S_DATA_CONNECT;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt32 XorKey;
	UInt32 AuthKey;
	UInt16 ConnectionID;
	UInt16 XorKeyIndex;
	UInt32 Unknown1;
	UInt32 Unknown2;
} S2C_DATA_CONNECT;

typedef struct {
	C2S_DATA_SIGNATURE;
	UInt32 AuthKey;
	UInt16 EntityID;
	UInt8 WorldID;
	UInt8 ServerID;
	UInt32 ClientMagicKey;
} C2S_DATA_VERIFYLINKS;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 WorldID;
	UInt8 ServerID;
	UInt8 Status;
} S2C_DATA_VERIFYLINKS;

typedef struct {
	C2S_DATA_SIGNATURE;
	UInt32 Unknown1;
} C2S_DATA_AUTHACCOUNT;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt32 ServerStatus;
	UInt32 Unknown1;
	UInt32 Unknown2;
} S2C_DATA_AUTHACCOUNT;

typedef struct {
	C2S_DATA_SIGNATURE;
} C2S_DATA_DISCONNECT;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 Message;
	UInt8 Unknown1;
	UInt8 Unknown2;
} S2C_DATA_SYSTEMMESSAGE;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 ServerCount;
	/* S2C_DATA_SERVER[ServerCount] */
} S2C_DATA_SERVERLIST;

typedef struct {
	UInt32 ServerID;
	UInt32 Unknown1;
	UInt8 WorldCount;
	/* S2C_DATA_WORLD[WorldCount] */
} S2C_DATA_SERVER;

typedef struct {
	UInt8 ServerID;
	UInt8 WorldID;
	UInt16 PlayerCount;
	UInt8 Unknown1[22];
	UInt16 MaxPlayerCount;
	Char WorldHost[65];
	UInt16 WorldPort;
	UInt32 WorldType;
	UInt32 Unknown2;
} S2C_DATA_WORLD;

typedef struct {
	C2S_DATA_SIGNATURE;
	UInt32 ClientVersion;
	UInt32 DebugVersion;
	UInt32 Reserved[2];
} C2S_DATA_CHECKVERSION;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt32 ClientVersion;
	UInt32 ServerMagicKey;
	UInt32 Reserved[2];
} S2C_DATA_CHECKVERSION;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 Unknown1;
	UInt32 Unknown2[4];
	UInt8 Unknown3;
	UInt32 Unknown4[4];
} S2C_DATA_UNKNOWN124;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt16 PayloadLength[2];
	/* S2C_DATA_URL[] */
} S2C_DATA_URLLIST;

typedef struct {
	UInt32 Length;
	/* Char[Length] */
} S2C_DATA_URL;

typedef struct {
	C2S_DATA_SIGNATURE;
} C2S_DATA_PUBLICKEY;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 Unknown1;
	UInt16 PublicKeyLength;
	/* UInt8[PublicKeyLength] */
} S2C_DATA_PUBLICKEY;

typedef struct {
	C2S_DATA_SIGNATURE;
	Char Username[129];
} C2S_DATA_SERVERENV;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 AuthCaptcha[4108];
} S2C_DATA_SERVERENV;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt64 Timeout;
	UInt8 Unknown1;
} S2C_DATA_DCTIMER;

typedef struct {
	C2S_DATA_SIGNATURE;
	UInt32 Padding0;
	UInt8 Payload[0];
} C2S_DATA_AUTHENTICATE;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 KeepAlive;
	UInt32 Unknown1;
	UInt32 Unknown2;
	Int32 LoginStatus;
	UInt32 Extended;
	Int32 AccountStatus;
	/* S2C_DATA_AUTHENTICATE_EXTENSION */
} S2C_DATA_AUTHENTICATE;

typedef struct {
	UInt8 Unknown1[55];
	Char AuthKey[32];
	UInt8 Unknown2[3];

	struct {
		UInt8 ServerID;
		UInt8 Count;
	} Characters[MAX_SERVER_COUNT];

} S2C_DATA_AUTHENTICATE_EXTENSION;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt32 Timeout;
} S2C_DATA_AUTHTIMER;

typedef struct {
	C2S_DATA_SIGNATURE;
} C2S_DATA_UNKNOWN3383;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 Unknown1[8];
} S2C_DATA_UNKNOWN3383;

typedef struct {
	C2S_DATA_SIGNATURE;
} C2S_DATA_UNKNOWN5383;

typedef struct {
	S2C_DATA_SIGNATURE;
	UInt8 Unknown1[8];
} S2C_DATA_UNKNOWN5383;

#pragma pack(pop)

EXTERN_C_END
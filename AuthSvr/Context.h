#pragma once

#include "Base.h"
#include "Config.h"
#include "Constants.h"

EXTERN_C_BEGIN

enum {
    CLIENT_FLAGS_CONNECTED            = 1 << 0,
    CLIENT_FLAGS_VERSION_CHECKED        = 1 << 1,
    CLIENT_FLAGS_USERNAME_CHECKED       = 1 << 2,
    CLIENT_FLAGS_PUBLICKEY_INITIALIZED  = 1 << 3,
    CLIENT_FLAGS_AUTHORIZED             = 1 << 4,
    CLIENT_FLAGS_AUTHENTICATED          = 1 << 5,
    CLIENT_FLAGS_VERIFIED               = 1 << 6,
    CLIENT_FLAGS_CHECK_DISCONNECT_TIMER = 1 << 7,
};

struct _ServerContext {
#pragma pack(push, 8)
    struct {
        struct _Config ConfigMemory;
        struct _Socket ClientSocketMemory;
        struct _Socket MasterSocketMemory;
    } Memory;
#pragma pack(pop)

    ConfigRef Config;
    SocketRef ClientSocket;
    SocketRef MasterSocket;
    DatabaseRef Database;
    UInt32 Flags;
    Timestamp Timestamp;
    Timestamp WorldListBroadcastTimestamp;
    Timestamp WorldListUpdateTimestamp;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 ClientVersion;
    UInt32 Flags;
    Timestamp DisconnectTimestamp;
    RSA* RSA;
    UInt8 RSAPayloadBuffer[CLIENT_RSA_PAYLOAD_LENGTH];
    Int32 AccountID;
    Int32 LoginStatus;
    Int32 AccountStatus;
    Char Username[MAX_USERNAME_LENGTH];
};
typedef struct _ClientContext* ClientContextRef;

struct _MasterContextWorld {
    UInt8 WorldID;
    UInt16 PlayerCount;
    UInt16 MaxPlayerCount;
    Char WorldHost[65];
    UInt16 WorldPort;
    UInt32 WorldType;
};
typedef struct _MasterContextWorld* MasterContextWorldRef;

struct _MasterContext {
    UInt8 ServerID;
    Array Worlds;
};
typedef struct _MasterContext* MasterContextRef;

EXTERN_C_END
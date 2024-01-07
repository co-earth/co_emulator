#pragma once

#include "Base.h"
#include "Config.h"

EXTERN_C_BEGIN

enum {
    SERVER_FLAGS_AUTH_CONNECTED = 1 << 0,
};

enum {
    CLIENT_FLAGS_WORLD_INITIALIZED = 1 << 0,
};

struct _ServerContext {
#pragma pack(push, 8)
    struct {
        struct _Config ConfigMemory;
        struct _Socket AuthSocketMemory;
        struct _Socket WorldSocketMemory;
    } Memory;
#pragma pack(pop)

    ConfigRef Config;
    SocketRef AuthSocket;
    SocketRef WorldSocket;
    DatabaseRef Database;
    UInt32 Flags;
    Timestamp Timestamp;
    Timestamp WorldListBroadcastTimestamp;
    Timestamp WorldListUpdateTimestamp;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 Flags;

    union {
        struct { 
            UInt8 _;
        } Auth;
        
        struct {
            UInt8 WorldID;
            Char WorldHost[65];
            UInt16 WorldPort;
            UInt32 WorldType;
            UInt16 PlayerCount;
            UInt16 MaxPlayerCount;
        } World;
    };
};
typedef struct _ClientContext* ClientContextRef;

EXTERN_C_END
#pragma once

#include "Base.h"
#include "Keychain.h"
#include "Platform.h"

EXTERN_C_BEGIN

#define SOCKET_MAX_PACKET_SIZE 0xFFFF

#pragma pack(push, 1)
struct _PacketSignature {
    UInt16 Magic;
    UInt16 Length;
};
#pragma pack(pop)

typedef struct _PacketSignature* PacketRef;
typedef struct _Socket* SocketRef;
typedef struct _SocketConnection* SocketConnectionRef;

enum {
    SOCKET_FLAGS_LISTENING = 1 << 0,
    SOCKET_FLAGS_CONNECTING = 1 << 1,
    SOCKET_FLAGS_CONNECTED = 1 << 2,
    SOCKET_FLAGS_ENCRYPTED = 1 << 3,
    SOCKET_FLAGS_CLIENT = 1 << 4,
    SOCKET_FLAGS_PLAIN = 1 << 4,
};

enum {
    SOCKET_CONNECTION_FLAGS_DISCONNECTED = 1 << 0,
    SOCKET_CONNECTION_FLAGS_DISCONNECTED_END = 1 << 1,
};

typedef Void (*SocketConnectionCallback)(
    SocketRef Socket,
    SocketConnectionRef Connection
);

typedef Void (*SocketPacketCallback)(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
);

typedef Int32 (*SocketRawPacketCallback)(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt8 *Buffer,
    Int32 Length
);

struct _Socket {
    SocketHandle Handle;
    SocketAddress Address;
    UInt32 Flags;
    Int32 MaxConnectionCount;
    Int32 NextConnectionID;
    Timestamp Timeout;
    SocketConnectionCallback OnConnect;
    SocketConnectionCallback OnDisconnect;
    SocketPacketCallback OnSend;
    SocketPacketCallback OnReceived;
    SocketRawPacketCallback OnSendRaw;
    SocketRawPacketCallback OnReceivedRaw;
    Array Connections;
    Void* Userdata;
};

struct _SocketConnection {
    SocketHandle Handle;
    SocketAddress Address;
    Char AddressIP[16];
    Int32 ID;
    UInt32 Flags;
    struct _Keychain Keychain;
    Int32 ReadBufferOffset;
    UInt8 ReadBuffer[SOCKET_MAX_PACKET_SIZE];
    Int32 WriteBufferOffset;
    UInt8 WriteBuffer[SOCKET_MAX_PACKET_SIZE];
    Void* Userdata;
};

Void SocketInitEncryptedServer(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
);

Void SocketInitEncryptedClient(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
);

Void SocketInitPlainServer(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
);

Void SocketInitRaw(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketRawPacketCallback OnSend,
    SocketRawPacketCallback OnReceived,
    Void* Userdata
);

Void SocketConnect(
    SocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
);

Void SocketListen(
    SocketRef Socket,
    UInt16 Port
);

Void SocketSendRaw(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt8* Data,
    Int32 Length
);

Void SocketSendAllRaw(
    SocketRef Socket,
    UInt8* Data,
    Int32 Length
);

Void SocketSend(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
);

Void SocketSendAll(
    SocketRef Socket,
    PacketRef Packet
);

Void SocketUpdate(
    SocketRef Socket
);

Void SocketDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void SocketClose(
    SocketRef Socket
);

SocketConnectionRef SocketGetConnection(
    SocketRef Socket,
    Int32 ConnectionID
);

Void SocketConnectionKeychainSeed(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt32 Key,
    UInt32 Step
);

EXTERN_C_END

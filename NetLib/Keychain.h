#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#define KEYCHAIN_XORKEY_COUNT 0x4000

enum {
    KEYCHAIN_FLAGS_INITIAL = 1 << 0,
};

struct _Keychain {
    UInt32 Flags;
    UInt32 HeaderXor;
    UInt32 Step;
    UInt32 Mul;
    UInt8 Key[0x20000];
    UInt32 Mask[4];
    Bool Client;
};
typedef struct _Keychain Keychain;
typedef struct _Keychain* KeychainRef;

Void KeychainInit(
    KeychainRef Keychain,
    Bool Client
);

Void KeychainGenerate(
    KeychainRef Keychain,
    UInt32 Key,
    Int32 Position,
    Int32 Size
);

Void KeychainSeed(
    KeychainRef Keychain,
    UInt32 Key,
    UInt32 Step
);

Void KeychainEncryptPacket(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
);

Void KeychainDecryptPacket(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
);

Int32 KeychainGetPacketLength(
    KeychainRef Keychain,
    UInt8* Packet,
    Int32 Length
);

EXTERN_C_END

#pragma once

#include "Socket.h"

EXTERN_C_BEGIN

#define PACKET_BUILDER_BUFFER_SIZE  0x1FFFF
#define PACKET_BUILDER_BUFFER_COUNT 8

PacketRef _PacketInit(
    Int32 Length,
    Bool Extended
);

#define PacketInit(__TYPE__) \
(__TYPE__*)_PacketInit(sizeof(__TYPE__), false)

#define PacketInitExtended(__TYPE__) \
(__TYPE__*)_PacketInit(sizeof(__TYPE__), true)

UInt8* PacketAppendMemory(
    Int32 Length
);

UInt8* PacketAppendMemoryCopy(
    UInt8* Source,
    Int32 Length
);

#define PacketAppendCString(__STRING__) \
(CString*)PacketAppendMemoryCopy(__STRING__, strlen(__STRING__) + 1)

#define PacketAppendValue(__TYPE__, __VALUE__) \
*((__TYPE__*)PacketAppendMemory(sizeof(__TYPE__))) = __VALUE__

#define PacketAppendStruct(__TYPE__) \
(__TYPE__*)PacketAppendMemory(sizeof(__TYPE__))

Void PacketLogBytes(
    PacketRef Packet
);

EXTERN_C_END

#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define C2S_DATA_SIGNATURE          \
struct _PacketSignature Signature;  \
UInt32 Checksum;                    \
UInt16 Command

#define S2C_DATA_SIGNATURE			\
struct _PacketSignature Signature;	\
UInt16 Command						\

#define S2C_DATA_SIGNATURE_EXTENDED	\
struct _PacketSignature Signature;	\
UInt16 Padding;						\
UInt16 Command

#define IPC_DATA_SIGNATURE			\
struct _PacketSignature Signature;	\
UInt16 ConnectionID;				\
UInt16 Command

#pragma pack(pop)

EXTERN_C_END
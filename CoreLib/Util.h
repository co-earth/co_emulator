#pragma once

#include "Base.h"

EXTERN_C_BEGIN

Timestamp GetTimestamp();

Bool GetPlatformErrorMessage(
	CString Buffer,
	Int32 Length
);

Bool CreatePasswordHash(
    CString Password,
    UInt8** Salt,
    Int32* SaltLength,
    UInt8** Hash,
    Int32* HashLength
);

Bool ValidatePasswordHash(
    CString Password,
    UInt8* Salt,
    Int32 SaltLength,
    UInt8* Hash,
    Int32 HashLength
);

Void GenerateRandomKey(
    CString Buffer,
    Int32 Length
);


UInt32 SwapUInt32(
    UInt32 Value
);

Int32 Random(Int32* Seed);

Int32 RandomRange(Int32* Seed, Int32 Min, Int32 Max);

Void ReadConfigString(
    CString FilePath,
    CString KeyPath,
    CString Default,
    CString* Result,
    Int32 ResultLength
);

Void ReadConfigCharArray(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Char* Result,
    Int32 ResultLength
);

Void ReadConfigBool(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Bool* Result
);

Void ReadConfigInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int8* Result
);

Void ReadConfigInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int16* Result
);

Void ReadConfigInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int32* Result
);

Void ReadConfigInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int64* Result
);

Void ReadConfigUInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt8* Result
);

Void ReadConfigUInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt16* Result
);

Void ReadConfigUInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt32* Result
);

Void ReadConfigUInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt64* Result
);

EXTERN_C_END
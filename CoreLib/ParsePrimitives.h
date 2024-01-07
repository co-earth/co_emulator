#include "Base.h"
#include "Archive.h"

EXTERN_C_BEGIN

Void ParseBool(
    CString Value,
    Bool* Result
);

Void ParseInt8(
    CString Value,
    Int8* Result
);

Void ParseInt16(
    CString Value,
    Int16* Result
);

Void ParseInt32(
    CString Value,
    Int32* Result
);

Void ParseInt64(
    CString Value,
    Int64* Result
);

Void ParseUInt8(
    CString Value,
    UInt8* Result
);

Void ParseUInt16(
    CString Value,
    UInt16* Result
);

Void ParseUInt32(
    CString Value,
    UInt32* Result
);

Void ParseUInt64(
    CString Value,
    UInt64* Result
);

Bool ParseAttributeInt8(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int8* Result
);

Bool ParseAttributeInt16(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int16* Result
);

Bool ParseAttributeInt32(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result
);

Bool ParseAttributeInt64(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int64* Result
);

Bool ParseAttributeUInt8(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt8* Result
);

Bool ParseAttributeUInt16(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt16* Result
);

Bool ParseAttributeUInt32(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result
);

Bool ParseAttributeUInt64(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt64* Result
);

Bool ParseAttributeFloat32(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Float32* Result
);

Bool ParseAttributeInt32Array(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int32 Count,
    Char Separator
);

Int32 ParseAttributeInt32ArrayCounted(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int32 Count,
    Char Separator
);

Bool ParseAttributeUInt32Array(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result,
    Int32 Count
);

Int32 ParseAttributeInt32Array2D(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int32 Count,
    Int32 GroupCount,
    Char Separator,
    Char GroupSeparator
);

Bool ParseAttributeString(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    CString Result,
    Int32 Length
);

EXTERN_C_END
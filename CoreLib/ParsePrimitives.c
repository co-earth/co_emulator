#include "ParsePrimitives.h"

// TODO: Refactor the structure of this to support more flexible parsing patterns

Void ParseBool(
    CString Value,
    Bool* Result
) {
    *Result = (Bool)atoll(Value);
}

Void ParseInt8(
    CString Value,
    Int8* Result
) {
    *Result = (Int8)atoll(Value);
}

Void ParseInt16(
    CString Value,
    Int16* Result
) {
    *Result = (Int16)atoll(Value);
}

Void ParseInt32(
    CString Value,
    Int32* Result
) {
    *Result = (Int32)atoll(Value);
}

Void ParseInt64(
    CString Value,
    Int64* Result
) {
    *Result = (Int64)atoll(Value);
}

Void ParseUInt8(
    CString Value,
    UInt8* Result
) {
    *Result = (UInt8)strtoull(Value, NULL, 10);
}

Void ParseUInt16(
    CString Value,
    UInt16* Result
) {
    *Result = (UInt16)strtoull(Value, NULL, 10);
}

Void ParseUInt32(
    CString Value,
    UInt32* Result
) {
    *Result = (UInt32)strtoull(Value, NULL, 10);
}

Void ParseUInt64(
    CString Value,
    UInt64* Result
) {
    *Result = (UInt64)strtoull(Value, NULL, 10);
}


Bool ParseAttributeInt8(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int8* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = -1;
        return true;
    }

    ParseInt8(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeInt16(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int16* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = -1;
        return true;
    }

    ParseInt16(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeInt32(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = -1;
        return true;
    }

    ParseInt32(Data->Data, Result);
    return true;
    
error:
    return false;
}

Bool ParseAttributeInt64(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int64* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = -1;
        return true;
    }

    ParseInt64(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt8(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt8* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = UINT32_MAX;
        return true;
    }

    ParseUInt8(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt16(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt16* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = UINT32_MAX;
        return true;
    }

    ParseUInt16(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt32(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = UINT32_MAX;
        return true;
    }

    ParseUInt32(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt64(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt64* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = -1;
        return true;
    }

    ParseUInt64(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeFloat32(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Float32* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1) {
        *Result = -1;
        return true;
    }

    *Result = strtof(Data->Data, NULL);
    return true;

error:
    return false;
}

Bool ParseAttributeInt32Array(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int32 Count,
    Char Separator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(Int32) * Count);

    if (Data->Length < 1) {
        return true;
    }

    Int32 Index = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length && Index < Count) {
        if (*Cursor == '\0') break;

        if (*Cursor == Separator) {
            Cursor += 1;
            continue;
        }

        Char* Next;
        Result[Index] = (Int32)strtoll(Cursor, &Next, 10);
        Index += 1;
        Cursor = Next;
    }

    return true;

error:
    return false;
}

Int32 ParseAttributeInt32ArrayCounted(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int32 Count,
    Char Separator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(Int32) * Count);

    if (Data->Length < 1) {
        return true;
    }

    Int32 Index = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length && Index < Count) {
        if (*Cursor == '\0') break;

        if (*Cursor == Separator) {
            Cursor += 1;
            continue;
        }

        Char* Next;
        Result[Index] = (Int32)strtoll(Cursor, &Next, 10);
        Index += 1;
        Cursor = Next;
    }

    return Index;

error:
    return 0;
}

Bool ParseAttributeUInt32Array(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result,
    Int32 Count
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(UInt32) * Count);

    if (Data->Length < 1) {
        return true;
    }

    Int32 Index = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length && Index < Count) {
        if (*Cursor == '\0') break;

        if (*Cursor == ':') {
            Cursor += 1;
            continue;
        }

        Char* Next = NULL;
        Result[Index] = (UInt32)strtoull(Cursor, &Next, 10);
        Index += 1;
        Cursor = Next;
    }

    return true;

error:
    return false;
}

Int32 ParseAttributeInt32Array2D(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int32 Count,
    Int32 GroupCount,
    Char Separator,
    Char GroupSeparator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(Int32) * Count * GroupCount);

    if (Data->Length < 1) {
        return true;
    }

    Int32 Index = 0;
    Int32 GroupIndex = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length && Index < Count && GroupIndex < GroupCount) {
        if (*Cursor == '\0') break;

        if (*Cursor == Separator) {
            Cursor += 1;
            Index += 1;
            continue;
        }

        if (*Cursor == GroupSeparator) {
            Cursor += 1;
            GroupIndex += 1;
            continue;
        }

        Char* Next;
        Result[GroupIndex * Count + Index] = (Int32)strtoll(Cursor, &Next, 10);
        Cursor = Next;
    }

    return (GroupIndex < 1 && Index > 0) ? 1 : GroupIndex;

error:
    return 0;
}

Bool ParseAttributeString(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    CString Result,
    Int32 Length
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveString* Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    assert(Data->Length + 1 <= Length);
    memcpy(Result, Data->Data, Data->Length);
    Result[Data->Length] = '\0';
    return true;

error:
    return false;
}

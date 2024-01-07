#include "String.h"

static Char Buffer[32] = { 0 };

CString UInt8ToStringNoAlloc(UInt8 Value) {
    sprintf(Buffer, "%hhu", Value);
    return Buffer;
}

CString UInt16ToStringNoAlloc(UInt16 Value) {
    sprintf(Buffer, "%hu", Value);
    return Buffer;
}

CString UInt32ToStringNoAlloc(UInt32 Value) {
    sprintf(Buffer, "%lu", Value);
    return Buffer;
}

CString UInt64ToStringNoAlloc(UInt64 Value) {
    sprintf(Buffer, "%llu", Value);
    return Buffer;
}

CString Int8ToStringNoAlloc(Int8 Value) {
    sprintf(Buffer, "%hhd", Value);
    return Buffer;
}

CString Int16ToStringNoAlloc(Int16 Value) {
    sprintf(Buffer, "%hd", Value);
    return Buffer;
}

CString Int32ToStringNoAlloc(Int32 Value) {
    sprintf(Buffer, "%ld", Value);
    return Buffer;
}

CString Int64ToStringNoAlloc(Int64 Value) {
    sprintf(Buffer, "%lld", Value);
    return Buffer;
}

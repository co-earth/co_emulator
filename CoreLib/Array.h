#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#define ARRAY_MIN_CAPACITY 8

static const Int32 kArrayElementNotFound = INT32_MAX;

struct _Array {
    Int32 Size;
    Int32 Count;
    Int32 Capacity;
    UInt8* Memory;
};
typedef struct _Array Array;

typedef Bool(*ArrayPredicate)(MemoryRef Lhs, MemoryRef Rhs);

Void ArrayInitialize(
    Array* array, 
    Int32 size, 
    UInt8* buffer, 
    Int32 count
);

Void ArrayInitializeEmpty(
    Array* array, 
    Int32 size, 
    Int32 capacity
);

Void ArrayDeinitialize(
    Array* array
);

UInt8* ArrayGetElementAtIndex(
    Array* array, 
    Int32 index
);

Void ArrayCopyElementAtIndex(
    Array* array, 
    Int32 index, 
    UInt8* buffer
);

Void ArrayAppendElement(
    Array* array, 
    UInt8* buffer
);

UInt8* ArrayAppendUninitializedElement(
    Array* array
);

Void ArrayAppendArray(
    Array* array, 
    Array* other);

Void ArrayAppendMemory(
    Array* array, 
    UInt8* buffer, 
    Int32 length
);

UInt8* ArrayAppendUninitializedMemory(
    Array* array, 
    Int32 length
);

Void ArrayInsertElementAtIndex(
    Array* array, 
    Int32 index, 
    UInt8* buffer
);

Void ArraySetElementAtIndex(
    Array* array, 
    Int32 index, 
    UInt8* buffer
);

Void ArrayRemoveElementAtIndex(
    Array* array, 
    Int32 index
);

Int32 ArrayGetElementIndexInMemoryBounds(
    Array* array,
    UInt8* offset
);

Void ArrayRemoveElementInMemoryBounds(
    Array* array, 
    UInt8* offset
);

Void ArrayRemoveAllElements(
    Array* array, 
    Bool keepCapacity
);

Int32 ArrayGetIndexOfElement(
    Array* array, 
    ArrayPredicate predicate, 
    MemoryRef element
);

EXTERN_C_END

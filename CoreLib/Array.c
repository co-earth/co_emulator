#include "Array.h"

static Void _ArrayReserveCapacity(Array* array, Int32 capacity);

Void ArrayInitialize(Array* array, Int32 size, UInt8* buffer, Int32 count) {
    assert(array);
    assert(size > 0);
    assert(buffer);
    assert(count > 0);

    ArrayInitializeEmpty(array, size, count);
    memcpy(array->Memory, buffer, size * count);
}

Void ArrayInitializeEmpty(Array* array, Int32 size, Int32 capacity) {
    assert(array);
    array->Size = size;
    array->Count = 0;
    array->Capacity = MAX(ARRAY_MIN_CAPACITY, capacity);
    array->Memory = (UInt8*)malloc(size * array->Capacity);
    assert(array->Memory);
}

Void ArrayDeinitialize(Array* array) {
    assert(array);
    free(array->Memory);
    memset(array, 0, sizeof(Array));
}

UInt8* ArrayGetElementAtIndex(Array* array, Int32 index) {
    assert(index < array->Count);
    return array->Memory + array->Size * index;
}

Void ArrayCopyElementAtIndex(Array* array, Int32 index, UInt8* buffer) {
    assert(index < array->Count);
    memcpy(buffer, ArrayGetElementAtIndex(array, index), array->Size);
}

Void ArrayAppendElement(Array* array, UInt8* buffer) {
    _ArrayReserveCapacity(array, array->Count + 1);
    memcpy(array->Memory + array->Size * array->Count, buffer, array->Size);
    array->Count += 1;
}

UInt8* ArrayAppendUninitializedElement(Array* array) {
    _ArrayReserveCapacity(array, array->Count + 1);
    array->Count += 1;
    return ArrayGetElementAtIndex(array, array->Count - 1);
}

Void ArrayAppendArray(Array* array, Array* other) {
    assert(array->Size == other->Size);

    if (other->Count < 1) return;

    _ArrayReserveCapacity(array, array->Count + other->Count);
    memcpy(array->Memory + array->Size * array->Count, other->Memory, array->Size * other->Count);
    array->Count += other->Count;
}

Void ArrayAppendMemory(Array* array, UInt8* buffer, Int32 length) {
    assert(length % array->Size == 0);

    _ArrayReserveCapacity(array, array->Count + length);

    memcpy(array->Memory + array->Size * array->Count, buffer, length);
    array->Count += length / array->Size;
}

UInt8* ArrayAppendUninitializedMemory(Array* array, Int32 length) {
    assert(length % array->Size == 0);

    _ArrayReserveCapacity(array, array->Count + length);

    Int32 index = array->Count;
    array->Count += length / array->Size;
    return ArrayGetElementAtIndex(array, index);
}

Void ArrayInsertElementAtIndex(Array* array, Int32 index, UInt8* buffer) {
    if (index == array->Count) {
        ArrayAppendElement(array, buffer);
        return;
    }

    assert(index < array->Count);
    _ArrayReserveCapacity(array, array->Count + 1);

    Int32 tailLength = array->Count - index;
    if (tailLength > 0) {
        void* source = array->Memory + array->Size * index;
        void* destination = array->Memory + array->Size * (index + 1);
        memmove(destination, source, array->Size * tailLength);
    }

    memcpy(array->Memory + array->Size * index, buffer, array->Size);
    array->Count += 1;
}

Void ArraySetElementAtIndex(Array* array, Int32 index, UInt8* buffer) {
    assert(index < array->Count);
    memcpy(array->Memory + array->Size * index, buffer, array->Size);
}

Void ArrayRemoveElementAtIndex(Array* array, Int32 index) {
    assert(index < array->Count);

    Int32 tailLength = array->Count - index - 1;
    if (tailLength > 0) {
        void* source = ArrayGetElementAtIndex(array, index + 1);
        void* destination = ArrayGetElementAtIndex(array, index);
        memmove(destination, source, array->Size * tailLength);
    }

    array->Count -= 1;
}

Int32 ArrayGetElementIndexInMemoryBounds(Array* array, UInt8* offset) {
    assert(array->Memory <= offset);
    assert(offset < array->Memory + array->Size * array->Count);
    return (offset - array->Memory) / array->Size;
}

Void ArrayRemoveElementInMemoryBounds(Array* array, UInt8* offset) {
    Int32 index = ArrayGetElementIndexInMemoryBounds(array, offset);
    ArrayRemoveElementAtIndex(array, index);
}

Void ArrayRemoveAllElements(Array* array, Bool keepCapacity) {
    array->Count = 0;

    if (!keepCapacity) {
        Int32 capacity = MIN(ARRAY_MIN_CAPACITY, array->Capacity);
        UInt8* memory = realloc(array->Memory, capacity);
        assert(memory);
        array->Capacity = capacity;
        array->Memory = memory;
    }
}

static Void _ArrayReserveCapacity(Array* array, Int32 newCapacity) {
    Int32 capacity = MAX(array->Capacity, ARRAY_MIN_CAPACITY);
    while (capacity < newCapacity) {
        capacity <<= 1;
    }

    if (array->Capacity < capacity) {
        UInt8* memory = realloc(array->Memory, array->Size * capacity);
        assert(memory);
        array->Capacity = capacity;
        array->Memory = memory;
    }
}

Int32 ArrayGetIndexOfElement(
    Array* array,
    ArrayPredicate predicate,
    MemoryRef element
) {
    for (Int32 index = 0; index < array->Count; index += 1) {
        MemoryRef lhs = ArrayGetElementAtIndex(array, index);
        if (predicate(lhs, element)) {
            return index;
        }
    }

    return kArrayElementNotFound;
}
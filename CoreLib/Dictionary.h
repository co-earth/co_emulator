#pragma once

#include "Allocator.h"

EXTERN_C_BEGIN

typedef Bool (*DictionaryKeyComparator)(const void *lhs, const void *rhs);
typedef UInt64 (*DictionaryKeyHasher)(const void *key);
typedef void *(*DictionaryKeySizeCallback)(const void *key);

typedef struct _Dictionary *DictionaryRef;

DictionaryRef DictionaryCreate(
    AllocatorRef allocator, 
    DictionaryKeyComparator comparator, 
    DictionaryKeyHasher hasher,
    DictionaryKeySizeCallback keySizeCallback, 
    Int32 capacity
);

DictionaryRef CStringDictionaryCreate(
    AllocatorRef allocator, 
    Int32 capacity
);

void DictionaryDestroy(
    DictionaryRef dictionary
);

void DictionaryInsert(
    DictionaryRef dictionary, 
    const void *key, 
    const void *element, 
    Int32 elementSize
);

const void *DictionaryLookup(
    DictionaryRef dictionary, 
    const void *key
);

void DictionaryRemove(
    DictionaryRef dictionary,
    const void *key
);

void DictionaryGetKeyBuffer(
    DictionaryRef dictionary, 
    void **memory, 
    Int32 *length
);

void DictionaryGetValueBuffer(
    DictionaryRef dictionary, 
    void **memory, 
    Int32 *length
);

EXTERN_C_END
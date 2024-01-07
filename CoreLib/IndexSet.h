#pragma once

#include "Array.h"

EXTERN_C_BEGIN

struct _IndexSetIterator {
    Int32 Value;
};

typedef struct {
    Array Indices;
} IndexSet;

typedef struct _IndexSetIterator* IndexSetIteratorRef;

Void IndexSetInitialize(IndexSet* set, Int32 capacity);

Void IndexSetDeinitialize(IndexSet* set);

Void IndexSetClear(IndexSet* set);

Void IndexSetInsert(IndexSet* set, Int32 index);

Void IndexSetRemove(IndexSet* set, Int32 index);

Bool IndexSetContains(IndexSet* set, Int32 index);

Void IndexSetUnion(IndexSet* result, IndexSet* lhs, IndexSet* rhs);

Void IndexSetIntersection(IndexSet* result, IndexSet* lhs, IndexSet* rhs);

Void IndexSetDifference(IndexSet* result, IndexSet* lhs, IndexSet* rhs);

Void IndexSetSymmetricDifference(IndexSet* result, IndexSet* lhs, IndexSet* rhs);

Bool IndexSetIsSubsetOf(IndexSet* set, IndexSet* other);

Bool IndexSetIsSupersetOf(IndexSet* set, IndexSet* other);

IndexSetIteratorRef IndexSetGetIterator(IndexSet* set);

IndexSetIteratorRef IndexSetIteratorNext(IndexSet* set, IndexSetIteratorRef iterator);

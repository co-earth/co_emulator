#include "IndexSet.h"

Void IndexSetInitialize(IndexSet* set, Int32 capacity) {
    ArrayInitializeEmpty(&set->Indices, sizeof(Int32), capacity);
}

Void IndexSetDeinitialize(IndexSet* set) {
    ArrayDeinitialize(&set->Indices);
}

Void IndexSetClear(IndexSet* set) {
    ArrayRemoveAllElements(&set->Indices, true);
}

Void IndexSetInsert(IndexSet* set, Int32 index) {
    if (IndexSetContains(set, index)) return;
    
    ArrayAppendElement(&set->Indices, (UInt8*)&index);
}

Void IndexSetRemove(IndexSet* set, Int32 index) {
    for (Int32 i = 0; i < set->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&set->Indices, i));
        if (value == index) {
            ArrayRemoveElementAtIndex(&set->Indices, i);
            break;
        }
    }
}

Bool IndexSetContains(IndexSet* set, Int32 index) {
    for (Int32 i = 0; i < set->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&set->Indices, i));
        if (value == index) {
            return true;
        }
    }

    return false;
}

Void IndexSetUnion(IndexSet* result, IndexSet* lhs, IndexSet* rhs) {
    assert(result->Indices.Memory == NULL);
    IndexSetInitialize(result, lhs->Indices.Count + rhs->Indices.Count);
    
    for (Int32 i = 0; i < lhs->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&lhs->Indices, i));
        IndexSetInsert(result, value);
    }

    for (Int32 i = 0; i < rhs->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&rhs->Indices, i));
        IndexSetInsert(result, value);
    }
}

Void IndexSetIntersection(IndexSet* result, IndexSet* lhs, IndexSet* rhs) {
    assert(result->Indices.Memory == NULL);
    IndexSetInitialize(result, lhs->Indices.Count + rhs->Indices.Count);

    for (Int32 i = 0; i < lhs->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&lhs->Indices, i));
        if (IndexSetContains(rhs, value)) {
            IndexSetInsert(result, value);
        }
    }
}

Void IndexSetDifference(IndexSet* result, IndexSet* lhs, IndexSet* rhs) {
    assert(result->Indices.Memory == NULL);
    IndexSetInitialize(result, lhs->Indices.Count + rhs->Indices.Count);

    for (Int32 i = 0; i < lhs->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&lhs->Indices, i));
        IndexSetInsert(result, value);
    }

    for (Int32 i = 0; i < rhs->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&rhs->Indices, i));
        IndexSetRemove(result, value);
    }
}

Void IndexSetSymmetricDifference(IndexSet* result, IndexSet* lhs, IndexSet* rhs) {
    assert(result->Indices.Memory == NULL);
    IndexSetInitialize(result, lhs->Indices.Count + rhs->Indices.Count);

    for (Int32 i = 0; i < lhs->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&lhs->Indices, i));
        IndexSetInsert(result, value);
    }

    for (Int32 i = 0; i < rhs->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&rhs->Indices, i));
        if (IndexSetContains(lhs, value)) {
            IndexSetRemove(result, value);
        }
        else {
            IndexSetInsert(result, value);
        }
    }
}

Bool IndexSetIsSubsetOf(IndexSet* set, IndexSet* other) {
    for (Int32 i = 0; i < set->Indices.Count; i++) {
        Int32 value = *((Int32*)ArrayGetElementAtIndex(&set->Indices, i));
        if (!IndexSetContains(other, value)) {
            return false;
        }
    }

    return true;
}

Bool IndexSetIsSupersetOf(IndexSet* set, IndexSet* other) {
    return IndexSetIsSubsetOf(other, set);
}

IndexSetIteratorRef IndexSetGetIterator(IndexSet* set) {
    if (set->Indices.Count < 1) return NULL;

    return (IndexSetIteratorRef)&set->Indices.Memory[0];
}

IndexSetIteratorRef IndexSetIteratorNext(IndexSet* set, IndexSetIteratorRef iterator) {
    Int32 index = ((UInt8*)iterator - &set->Indices.Memory[0]) / set->Indices.Size + 1;
    if (0 <= index && index < set->Indices.Count) {
        return (IndexSetIteratorRef)(&set->Indices.Memory[0] + set->Indices.Size * index);
    }

    return NULL;
}

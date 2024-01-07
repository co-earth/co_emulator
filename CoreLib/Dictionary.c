#include "Dictionary.h"
#include "TempAllocator.h"

const Int32 _kDictionaryBufferDefaultCapacity = 65535;
const Float32 _kDictionaryBufferGrowthFactor = 1.5;

struct _DictionaryBucket {
    UInt64 Hash;
    Int32 KeyOffset;
    Int32 ElementOffset;
    Bool IsFilled;
    struct _DictionaryBucket* Next;
};
typedef struct _DictionaryBucket* DictionaryBucketRef;

struct _DictionaryBuffer {
    Int32 Offset;
    Int32 Capacity;
    MemoryRef Memory;
};
typedef struct _DictionaryBuffer DictionaryBuffer;

struct _Dictionary {
    AllocatorRef Allocator;
    AllocatorRef BucketAllocator;
    DictionaryKeyComparator Comparator;
    DictionaryKeyHasher Hasher;
    DictionaryKeySizeCallback KeySizeCallback;
    DictionaryBuffer KeyBuffer;
    DictionaryBuffer ElementBuffer;
    Int32 Capacity;
    Int32 ElementCount;
    DictionaryBucketRef* Buckets;
};

static inline void _DictionaryBufferInit(DictionaryRef Dictionary, DictionaryBuffer* Buffer);
static inline void _DictionaryBufferReserveCapacity(DictionaryRef Dictionary, DictionaryBuffer* Buffer, Int32 Capacity);
static inline void* _DictionaryBufferGetElement(DictionaryRef Dictionary, DictionaryBuffer* Buffer, Int32 offset);
static inline Int32 _DictionaryBufferInsertElement(DictionaryRef Dictionary, DictionaryBuffer* Buffer, const void* Element,
    Int32 ElementSize);
static inline void _DictionaryBufferDeinit(DictionaryRef Dictionary, DictionaryBuffer* Buffer);

Bool _CStringDictionaryKeyComparator(const void* Lhs, const void* Rhs);
UInt64 _CStringDictionaryKeyHasher(const void* Key);
void* _CStringDictionaryKeySizeCallback(const void* Key);

DictionaryRef DictionaryCreate(AllocatorRef Allocator, DictionaryKeyComparator Comparator, DictionaryKeyHasher Hasher,
    DictionaryKeySizeCallback KeySizeCallback, Int32 Capacity) {
    DictionaryRef Dictionary = (DictionaryRef)AllocatorAllocate(Allocator,
        sizeof(struct _Dictionary) + sizeof(struct _DictionaryBucket) * Capacity);
    Dictionary->Allocator = Allocator;
    Dictionary->BucketAllocator = TempAllocatorCreate(Allocator);
    Dictionary->Comparator = Comparator;
    Dictionary->Hasher = Hasher;
    Dictionary->KeySizeCallback = KeySizeCallback;
    Dictionary->Capacity = Capacity;
    Dictionary->ElementCount = 0;
    Dictionary->Buckets = (DictionaryBucketRef*)(((UInt8*)Dictionary) + sizeof(struct _Dictionary));
    memset(Dictionary->Buckets, 0, sizeof(struct _DictionaryBucket) * Capacity);
    _DictionaryBufferInit(Dictionary, &Dictionary->KeyBuffer);
    _DictionaryBufferInit(Dictionary, &Dictionary->ElementBuffer);
    return Dictionary;
}

DictionaryRef CStringDictionaryCreate(AllocatorRef Allocator, Int32 Capacity) {
    return DictionaryCreate(Allocator, &_CStringDictionaryKeyComparator, &_CStringDictionaryKeyHasher, &_CStringDictionaryKeySizeCallback,
        Capacity);
}

void DictionaryDestroy(DictionaryRef Dictionary) {
    AllocatorDestroy(Dictionary->BucketAllocator);
    _DictionaryBufferDeinit(Dictionary, &Dictionary->ElementBuffer);
    _DictionaryBufferDeinit(Dictionary, &Dictionary->KeyBuffer);
    AllocatorDeallocate(Dictionary->Allocator, Dictionary);
}

void DictionaryInsert(DictionaryRef Dictionary, const void* Key, const void* Element, Int32 ElementSize) {
    if (Element == NULL || ElementSize < 1) {
        return DictionaryRemove(Dictionary, Key);
    }

    UInt64 hash = Dictionary->Hasher(Key);
    Int32 index = hash % Dictionary->Capacity;
    DictionaryBucketRef bucket = (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * index);
    while (bucket && bucket->IsFilled) {
        const void* bucketKey = _DictionaryBufferGetElement(Dictionary, &Dictionary->KeyBuffer, bucket->KeyOffset);
        if (bucket->Hash == hash && Dictionary->Comparator(bucketKey, Key)) {
            bucket->ElementOffset = _DictionaryBufferInsertElement(Dictionary, &Dictionary->ElementBuffer, Element, ElementSize);
            return;
        }

        if (bucket->Next) {
            bucket = bucket->Next;
        }
        else {
            break;
        }
    }

    if (bucket->IsFilled) {
        bucket->Next = (DictionaryBucketRef)AllocatorAllocate(Dictionary->BucketAllocator, sizeof(struct _DictionaryBucket));
        bucket = bucket->Next;
    }

    bucket->Hash = hash;
    bucket->KeyOffset = _DictionaryBufferInsertElement(Dictionary, &Dictionary->KeyBuffer, Key, Dictionary->KeySizeCallback(Key));
    bucket->ElementOffset = _DictionaryBufferInsertElement(Dictionary, &Dictionary->ElementBuffer, Element, ElementSize);
    bucket->IsFilled = true;
    bucket->Next = NULL;

    Dictionary->ElementCount += 1;
}

const void* DictionaryLookup(DictionaryRef Dictionary, const void* Key) {
    UInt64 hash = Dictionary->Hasher(Key);
    Int32 index = hash % Dictionary->Capacity;
    DictionaryBucketRef bucket = (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * index);
    while (bucket && bucket->IsFilled) {
        const void* bucketKey = _DictionaryBufferGetElement(Dictionary, &Dictionary->KeyBuffer, bucket->KeyOffset);
        if (bucket->Hash == hash && Dictionary->Comparator(bucketKey, Key)) {
            return _DictionaryBufferGetElement(Dictionary, &Dictionary->ElementBuffer, bucket->ElementOffset);
        }

        bucket = bucket->Next;
    }

    return NULL;
}

void DictionaryRemove(DictionaryRef Dictionary, const void* Key) {
    UInt64 hash = Dictionary->Hasher(Key);
    Int32 index = hash % Dictionary->Capacity;
    DictionaryBucketRef bucket = (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * index);
    DictionaryBucketRef previousBucket = NULL;
    while (bucket && bucket->IsFilled) {
        const void* bucketKey = _DictionaryBufferGetElement(Dictionary, &Dictionary->KeyBuffer, bucket->KeyOffset);
        if (bucket->Hash == hash && Dictionary->Comparator(bucketKey, Key)) {
            if (!previousBucket) {
                if (bucket->Next) {
                    memcpy((DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * index), bucket->Next,
                        sizeof(struct _DictionaryBucket));
                }
                else {
                    bucket->IsFilled = false;
                }
            }
            else {
                previousBucket->Next = bucket->Next;
            }


            Dictionary->ElementCount -= 1;
            return;
        }

        previousBucket = bucket;
        bucket = bucket->Next;
    }
}

void DictionaryGetKeyBuffer(DictionaryRef Dictionary, void** Memory, Int32* Length) {
    *Memory = Dictionary->KeyBuffer.Memory;
    *Length = Dictionary->KeyBuffer.Offset;
}

void DictionaryGetValueBuffer(DictionaryRef Dictionary, void** Memory, Int32* Length) {
    *Memory = Dictionary->ElementBuffer.Memory;
    *Length = Dictionary->ElementBuffer.Offset;
}

static inline void _DictionaryBufferInit(DictionaryRef Dictionary, DictionaryBuffer* Buffer) {
    Buffer->Offset = 0;
    Buffer->Capacity = _kDictionaryBufferDefaultCapacity;
    Buffer->Memory = AllocatorAllocate(Dictionary->Allocator, Buffer->Capacity);
}

static inline void _DictionaryBufferReserveCapacity(DictionaryRef Dictionary, DictionaryBuffer* Buffer, Int32 Capacity) {
    Int32 newCapacity = Buffer->Capacity;
    while (newCapacity < Capacity) {
        newCapacity *= _kDictionaryBufferGrowthFactor;
    }

    if (newCapacity > Buffer->Capacity) {
        Buffer->Capacity = newCapacity;
        Buffer->Memory = AllocatorReallocate(Dictionary->Allocator, Buffer->Memory, Buffer->Capacity);
    }
}

static inline void* _DictionaryBufferGetElement(DictionaryRef Dictionary, DictionaryBuffer* Buffer, Int32 offset) {
    return (void*)(((UInt8*)Buffer->Memory) + offset);
}

static inline Int32 _DictionaryBufferInsertElement(DictionaryRef Dictionary, DictionaryBuffer* Buffer, const void* Element,
    Int32 ElementSize) {
    Int32 requiredCapacity = Buffer->Offset + ElementSize;
    _DictionaryBufferReserveCapacity(Dictionary, Buffer, requiredCapacity);
    Int32 offset = Buffer->Offset;
    UInt8* destination = ((UInt8*)Buffer->Memory) + Buffer->Offset;
    memcpy(destination, Element, ElementSize);
    Buffer->Offset += ElementSize;
    return offset;
}

static inline void _DictionaryBufferDeinit(DictionaryRef Dictionary, DictionaryBuffer* Buffer) {
    AllocatorDeallocate(Dictionary->Allocator, Buffer->Memory);
}

Bool _CStringDictionaryKeyComparator(const void* Lhs, const void* Rhs) {
    return strcmp((const char*)Lhs, (const char*)Rhs) == 0;
}

UInt64 _CStringDictionaryKeyHasher(const void* Key) {
    UInt64 hash = 5381;
    const char* current = (const char*)Key;

    while (*current != '\0') {
        hash = hash * 33 + (*current);
        current += 1;
    }

    return hash;
}

void* _CStringDictionaryKeySizeCallback(const void* Key) {
    return strlen((const char*)Key) + 1;
}
#include "Array.h"
#include "TempAllocator.h"

struct _TempAllocatorContext {
    AllocatorRef allocator;
    Array allocations;
};

Bool _IsPointerEqual(MemoryRef Lhs, MemoryRef Rhs);

void *_AllocatorTemp(AllocatorMode mode, Int32 capacity, MemoryRef memory, MemoryRef context);

AllocatorRef TempAllocatorCreate(AllocatorRef allocator) {
    struct _TempAllocatorContext *context = AllocatorAllocate(allocator, sizeof(struct _TempAllocatorContext));
    context->allocator                    = allocator;
    ArrayInitializeEmpty(&context->allocations, sizeof(MemoryRef), 8);
    return AllocatorCreate(allocator, &_AllocatorTemp, context);
}

MemoryRef _AllocatorTemp(AllocatorMode mode, Int32 capacity, MemoryRef memory, MemoryRef context) {
    struct _TempAllocatorContext *tempContext = context;
    assert(tempContext);

    switch (mode) {
    case AllocatorModeAllocate: {
        void *memory = AllocatorAllocate(tempContext->allocator, capacity);
        assert(memory);
        ArrayAppendElement(&tempContext->allocations, &memory);
        return memory;
    }

    case AllocatorModeReallocate: {
        void *newMemory = AllocatorReallocate(tempContext->allocator, memory, capacity);
        if (newMemory) {
            Int32 index = ArrayGetIndexOfElement(&tempContext->allocations, &_IsPointerEqual, memory);
            if (index != kArrayElementNotFound) {
                ArraySetElementAtIndex(&tempContext->allocations, index, &newMemory);
            }
        }
        return newMemory;
    }

    case AllocatorModeDeallocate:
        return NULL;

    case AllocatorModeDestroy: {
        for (Int32 index = 0; index < tempContext->allocations.Count; index++) {
            MemoryRef memory = *((MemoryRef*)ArrayGetElementAtIndex(&tempContext->allocations, index));
            AllocatorDeallocate(tempContext->allocator, memory);
        }

        ArrayDestroy(tempContext->allocations);
        AllocatorDeallocate(tempContext->allocator, tempContext);
        return NULL;
    }

    default:
        UNREACHABLE("Invalid value for mode!");
    }
}

Bool _IsPointerEqual(MemoryRef Lhs, MemoryRef Rhs) {
    return *((MemoryRef*)Lhs) == Rhs;
}
#include "BumpAllocator.h"

const Int32 kBumpAllocatorDefaultPageCapacity = 0xFFFF;

#pragma pack(1)
struct _BumpAllocatorPage {
    struct _BumpAllocatorPage *next;
    Int32 size;
    Int32 capacity;
    Int32 index;
    UInt8 *memory;
};
#pragma pack(pop)

struct _BumpAllocatorContext {
    AllocatorRef allocator;
    Int32 alignment;
    Int32 pageHeaderSize;
    struct _BumpAllocatorPage *firstPage;
    struct _BumpAllocatorPage *currentPage;
};

void *_AllocatorBump(AllocatorMode mode, Int32 capacity, void *memory, void *context);

static inline Bool _IsPowerOfTwo(Int32 value);
static inline Int32 _Align(Int32 value, Int32 alignment);

AllocatorRef BumpAllocatorCreate(AllocatorRef allocator) {
    struct _BumpAllocatorContext *context = AllocatorAllocate(allocator, sizeof(struct _BumpAllocatorContext));
    context->allocator                    = allocator;
    context->alignment                    = 2 * sizeof(void *);
    context->pageHeaderSize               = _Align(sizeof(struct _BumpAllocatorPage), context->alignment);
    context->firstPage                    = NULL;
    context->currentPage                  = NULL;
    return AllocatorCreate(allocator, &_AllocatorBump, context);
}

void *_AllocatorBump(AllocatorMode mode, Int32 capacity, void *memory, void *context) {
    struct _BumpAllocatorContext *bumpContext = context;
    assert(bumpContext);

    switch (mode) {
    case AllocatorModeAllocate: {
        if (bumpContext->firstPage == NULL) {
            Int32 memoryCapacity = MAX(kBumpAllocatorDefaultPageCapacity, capacity + bumpContext->pageHeaderSize);
            memoryCapacity = _Align(memoryCapacity, bumpContext->alignment);
            struct _BumpAllocatorPage* page = AllocatorAllocate(bumpContext->allocator, memoryCapacity);
            page->size = memoryCapacity;
            page->next = NULL;
            page->capacity = memoryCapacity - bumpContext->pageHeaderSize;
            page->index = 0;
            page->memory = (UInt8*)page + bumpContext->pageHeaderSize;
            bumpContext->firstPage = page;
            bumpContext->currentPage = page;
        }
        else if (bumpContext->currentPage->index + capacity > bumpContext->currentPage->capacity) {
            Int32 memoryCapacity = MAX(kBumpAllocatorDefaultPageCapacity, capacity + bumpContext->pageHeaderSize);
            memoryCapacity = _Align(memoryCapacity, bumpContext->alignment);
            struct _BumpAllocatorPage* page = AllocatorAllocate(bumpContext->allocator, memoryCapacity);
            page->size = memoryCapacity;
            page->next = NULL;
            page->capacity = memoryCapacity - bumpContext->pageHeaderSize;
            page->index = 0;
            page->memory = (UInt8*)page + bumpContext->pageHeaderSize;
            bumpContext->currentPage->next = page;
            bumpContext->currentPage = page;
        }

        void *memory = bumpContext->currentPage->memory + sizeof(UInt8) * bumpContext->currentPage->index;
        bumpContext->currentPage->index += capacity;
        return memory;
    }

    case AllocatorModeReallocate:
        UNREACHABLE("BumpAllocator doesn't support reallocation!");
        return NULL;

    case AllocatorModeDeallocate:
        return NULL;

    case AllocatorModeDestroy: {
        struct _BumpAllocatorPage *page = bumpContext->firstPage;
        while (page) {
            struct _BumpAllocatorPage *next = page->next;
            AllocatorDeallocate(bumpContext->allocator, page);
            page = next;
        }

        AllocatorDeallocate(bumpContext->allocator, bumpContext);
        return NULL;
    }

    default:
        JELLY_UNREACHABLE("Invalid value for mode!");
    }
}

static inline Bool _IsPowerOfTwo(Int32 value) {
    return (value & (value - 1)) == 0;
}

static inline Int32 _Align(Int32 value, Int32 alignment) {
    assert(_IsPowerOfTwo(alignment));

    return (value + alignment - 1) & ~(alignment - 1);
}
#include "Allocator.h"

struct _Allocator {
    struct _Allocator *Allocator;
    AllocatorCallback Callback;
    MemoryRef Context;
};


MemoryRef _AllocatorMalloc(
    AllocatorMode Mode,
    Int32 Capacity,
    MemoryRef Memory,
    MemoryRef Context
);

MemoryRef _AllocatorNull(
    AllocatorMode Mode,
    Int32 Capacity,
    MemoryRef Memory,
    MemoryRef Context
);

AllocatorRef _AllocatorGetDefault(
    AllocatorRef Allocator
);

MemoryRef _AllocatorInvokeCallback(
    AllocatorRef Allocator,
    AllocatorMode Mode,
    Int32 Capacity,
    MemoryRef Memory
);

static struct _Allocator _kAllocatorSystemDefault = { NULL, &_AllocatorMalloc, NULL };
static struct _Allocator _kAllocatorMalloc = { NULL, &_AllocatorMalloc, NULL };
static struct _Allocator _kAllocatorNull = { NULL, &_AllocatorNull, NULL };

const AllocatorRef kAllocatorDefault = NULL;
const AllocatorRef kAllocatorSystemDefault = &_kAllocatorSystemDefault;
const AllocatorRef kAllocatorMalloc = &_kAllocatorMalloc;
const AllocatorRef kAllocatorNull = &_kAllocatorNull;

static AllocatorRef kAllocatorCurrentDefault = NULL;

AllocatorRef AllocatorGetDefault() {
    return kAllocatorDefault;
}

AllocatorRef AllocatorGetSystemDefault() {
    return kAllocatorSystemDefault;
}

AllocatorRef AllocatorGetMalloc() {
    return kAllocatorMalloc;
}

AllocatorRef AllocatorGetNull() {
    return kAllocatorNull;
}

Void AllocatorSetCurrentDefault(
    AllocatorRef Allocator
) {
    kAllocatorCurrentDefault = Allocator;
}

AllocatorRef AllocatorGetCurrentDefault() {
    return kAllocatorCurrentDefault;
}

AllocatorRef AllocatorCreate(
    AllocatorRef Allocator,
    AllocatorCallback Callback, 
    MemoryRef Context
) {
    AllocatorRef NewAllocator = (AllocatorRef)AllocatorAllocate(Allocator, sizeof(struct _Allocator));
    NewAllocator->Allocator = Allocator;
    NewAllocator->Callback = Callback;
    NewAllocator->Context = Context;
    return NewAllocator;
}

Void AllocatorDestroy(
    AllocatorRef Allocator
) {
    _AllocatorInvokeCallback(Allocator, AllocatorModeDestroy, 0, NULL);
    assert(Allocator->Allocator);
    AllocatorDeallocate(Allocator->Allocator, Allocator);
}

MemoryRef AllocatorAllocate(
    AllocatorRef Allocator,
    Int32 Capacity
) {
    return _AllocatorInvokeCallback(Allocator, AllocatorModeAllocate, Capacity, NULL);
}

MemoryRef AllocatorReallocate(
    AllocatorRef Allocator,
    MemoryRef Memory,
    Int32 Capacity
) {
    return _AllocatorInvokeCallback(Allocator, AllocatorModeReallocate, Capacity, Memory);
}

MemoryRef AllocatorDeallocate(
    AllocatorRef Allocator,
    MemoryRef Memory
) {
    return _AllocatorInvokeCallback(Allocator, AllocatorModeDeallocate, 0, Memory);
}

MemoryRef _AllocatorMalloc(
    AllocatorMode Mode,
    Int32 Capacity,
    MemoryRef Memory,
    MemoryRef Context
) {
    switch (Mode) {
    case AllocatorModeAllocate:
        return malloc(Capacity);

    case AllocatorModeReallocate:
        return realloc(Memory, Capacity);

    case AllocatorModeDeallocate:
        free(Memory);
        return NULL;

    case AllocatorModeDestroy:
        return NULL;

    default:
        UNREACHABLE("Invalid value for mode!");
    }
}

MemoryRef _AllocatorNull(
    AllocatorMode Mode,
    Int32 Capacity,
    MemoryRef Memory,
    MemoryRef Context
) {
    return NULL;
}

AllocatorRef _AllocatorGetDefault(
    AllocatorRef Allocator
) {
    if (Allocator == kAllocatorDefault) {
        Allocator = AllocatorGetDefault();
    }

    if (Allocator == NULL) {
        return kAllocatorSystemDefault;
    }

    return Allocator;
}

MemoryRef _AllocatorInvokeCallback(
    AllocatorRef Allocator,
    AllocatorMode Mode,
    Int32 Capacity,
    MemoryRef Memory
) {
    Allocator = _AllocatorGetDefault(Allocator);
    return Allocator->Callback(Mode, Capacity, Memory, Allocator->Context);
}

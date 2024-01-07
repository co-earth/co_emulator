#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTShopItemData {
    Int32 SlotID;
    Int32 ItemID;
    Int32 ItemOption;
    Int32 MinHonorRank;
    Int32 DurationID;
    Int32 Price;
    Int32 OnlyPremium;
    Int32 WexpPrice;
    Int32 DpPrice;
};

struct _RTShopData {
    Int32 Index;
    Int32 WorldID;
    Int32 NpcID;
    Int32 ItemCount;
    struct _RTShopItemData Items[RUNTIME_SHOP_MAX_ITEM_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
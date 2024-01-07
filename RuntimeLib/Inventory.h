#pragma once

#include "Base.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

Int32 RTInventoryGetNextFreeSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory
);

Bool RTInventoryIsSlotEmpty(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
);

RTItemSlotRef RTInventoryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
);

Bool RTInventorySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	RTItemSlotRef Slot
);

Bool RTInventoryClearSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
);

Bool RTInventoryMoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef SourceInventory,
	RTCharacterInventoryInfoRef TargetInventory,
	Int32 SourceSlotIndex,
	Int32 TargetSlotIndex
);

Void RTInventoryFindItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 ItemID,
	Int32 Length,
	Int32* Count,
	RTItemSlotRef Results
);

#pragma pack(pop)

EXTERN_C_END
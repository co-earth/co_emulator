#include "Constants.h"
#include "Character.h"
#include "Inventory.h"
#include "Runtime.h"

static struct _RTCharacterInventoryInfo kInventorySnapshot = { 0 };

// TODO: Move rollback logic to here...

Int32 RTInventoryGetInsertionIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Inventory->Count; Index += 1) {
		RTItemSlotRef InventorySlot = &Inventory->Slots[Index];
		if (InventorySlot->SlotIndex > SlotIndex) {
			return Index;
		}
	}

	return Inventory->Count;
}

Int32 RTInventoryGetNextFreeSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory
) {
	for (Int32 Index = 0; Index < Inventory->Count - 1; Index += 1) {
		RTItemSlotRef InventorySlot = &Inventory->Slots[Index];
		RTItemSlotRef NextInventorySlot = &Inventory->Slots[Index + 1];
		Int32 SlotOffset = NextInventorySlot->SlotIndex - InventorySlot->SlotIndex;
		if (SlotOffset > 1) {
			return InventorySlot->SlotIndex + 1;
		}
	}

	return Inventory->Count;
}

Bool RTInventoryIsSlotEmpty(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	return RTInventoryGetSlot(Runtime, Inventory, SlotIndex) == NULL;
}

Int32 RTInventoryGetSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Inventory->Count; Index++) {
		RTItemSlotRef Slot = &Inventory->Slots[Index];
		if (Slot->SlotIndex == SlotIndex) {
			return Index;
		}
	}

	return -1;
}

RTItemSlotRef RTInventoryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	Int32 Index = RTInventoryGetSlotIndex(Runtime, Inventory, SlotIndex);
	if (Index < 0) return NULL;
	return &Inventory->Slots[Index];
}

Bool RTInventorySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	RTItemSlotRef Slot
) {
	assert(0 <= Slot->SlotIndex && Slot->SlotIndex < RUNTIME_INVENTORY_TOTAL_SIZE);

	assert(RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID));

	Int32 Index = RTInventoryGetSlotIndex(
		Runtime, 
		Inventory, 
		Slot->SlotIndex
	);
	if (Index >= 0) return false;

	Int32 InsertionIndex = RTInventoryGetInsertionIndex(Runtime, Inventory, Slot->SlotIndex);
	Int32 InsertionTailLength = Inventory->Count - InsertionIndex;
	if (InsertionTailLength > 0) {
		memmove(
			&Inventory->Slots[InsertionIndex + 1],
			&Inventory->Slots[InsertionIndex],
			sizeof(struct _RTItemSlot) * InsertionTailLength
		);
	}

	RTItemSlotRef InventorySlot = &Inventory->Slots[InsertionIndex];
	memcpy(InventorySlot, Slot, sizeof(struct _RTItemSlot));
	Inventory->Count += 1;
	return true;
}

Bool RTInventoryClearSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	assert(0 <= SlotIndex && SlotIndex < RUNTIME_INVENTORY_TOTAL_SIZE);

	Int32 InventoryIndex = RTInventoryGetSlotIndex(
		Runtime,
		Inventory,
		SlotIndex
	);
	if (InventoryIndex < 0) return false;

	Int32 TailLength = Inventory->Count - InventoryIndex - 1;
	if (TailLength > 0) {
		memmove(
			&Inventory->Slots[InventoryIndex],
			&Inventory->Slots[InventoryIndex + 1],
			sizeof(struct _RTItemSlot) * TailLength
		);
	}

	Inventory->Count -= 1;
	return true;
}

Bool RTInventoryMoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef SourceInventory,
	RTCharacterInventoryInfoRef TargetInventory,
	Int32 SourceSlotIndex,
	Int32 TargetSlotIndex
) {
	RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, SourceInventory, SourceSlotIndex);
	if (!SourceSlot) return false;

	RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, TargetInventory, TargetSlotIndex);
	if (TargetSlot && SourceSlot->Item.Serial == TargetSlot->Item.Serial) {
		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, SourceSlot->Item.ID);
		if (!ItemData || ItemData->MaxStackSize <= 1) return false;
		
		// TODO: Check if item is quest item and calculate count accordingly

		Int32 TotalStackSize = SourceSlot->ItemOptions + TargetSlot->ItemOptions;
		TargetSlot->ItemOptions = MIN(ItemData->MaxStackSize, TotalStackSize);
		SourceSlot->ItemOptions = MAX(0, TotalStackSize - TargetSlot->ItemOptions);

		if (SourceSlot->ItemOptions < 1) {
			if (!RTInventoryClearSlot(Runtime, SourceInventory, SourceSlotIndex)) return false;
		}
	}
	else if (SourceSlot && !TargetSlot) {
        struct _RTItemSlot SlotCopy = *SourceSlot;
        if (!RTInventoryClearSlot(Runtime, SourceInventory, SourceSlot->SlotIndex)) return false;

        SlotCopy.SlotIndex = TargetSlotIndex;
        if (!RTInventorySetSlot(Runtime, TargetInventory, &SlotCopy)) return false;
	} else {
        return false;
    }

    return true;
}

Void RTInventoryFindItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 ItemID,
	Int32 Length,
	Int32* Count,
	RTItemSlotRef Results
) {
	assert(Length > 0);

	*Count = 0;

	for (Int32 Index = 0; Index < Inventory->Count; Index++) {
		RTItemSlotRef InventorySlot = &Inventory->Slots[Index];
		if (InventorySlot->Item.ID == ItemID) {
			memcpy(&Results[*Count], InventorySlot, sizeof(struct _RTItemSlot));
			*Count += 1;
			
			if (*Count >= Length) {
				return;
			}
		}
	}
}

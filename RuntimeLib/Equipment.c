#include "Constants.h"
#include "Character.h"
#include "Equipment.h"

RTItemSlotRef RTEquipmentGetSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return NULL;

    for (Int32 Index = 0; Index < Equipment->Count; Index++) {
        RTItemSlotRef Slot = &Equipment->Slots[Index];
        if (Slot->SlotIndex == SlotIndex) return Slot;
    }

    return NULL;
}

Bool RTEquipmentSetSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    RTItemSlotRef Slot
) {
    if (Slot->SlotIndex < 0 || Slot->SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;
    if (Equipment->Count >= RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    RTItemSlotRef CurrentSlot = RTEquipmentGetSlot(Runtime, Equipment, Slot->SlotIndex);
    if (CurrentSlot) return false;

    CurrentSlot = &Equipment->Slots[Equipment->Count];
    memcpy(CurrentSlot, Slot, sizeof(struct _RTItemSlot));
    Equipment->Count += 1;
}

Bool RTEquipmentClearSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    for (Int32 Index = 0; Index < Equipment->Count; Index++) {
        RTItemSlotRef Slot = &Equipment->Slots[Index];
        if (Slot->SlotIndex == SlotIndex) {
            Int32 TailLength = Equipment->Count - Index - 1;
            if (TailLength > 0) {
                memmove(
                    &Equipment->Slots[Index],
                    &Equipment->Slots[Index + 1],
                    sizeof(struct _RTItemSlot) * TailLength
                );
            }

            Equipment->Count -= 1;
            return true;
        }
    }

    return false;
}

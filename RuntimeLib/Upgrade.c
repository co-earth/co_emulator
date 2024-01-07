#include "Runtime.h"
#include "Upgrade.h"

Int32 RTUpgradeLevelNormalGetCoreCount(
    RTRuntimeRef Runtime,
    RTItemSlotRef Item
) {
    RTUpgradeDataRef UpgradeData = &Runtime->UpgradeData[RUNTIME_UPGRADE_TYPE_LEVEL_NORMAL];
    RTUpgradeLevelDataRef LevelData = NULL;

    for (Int32 Index = 0; Index < UpgradeData->Count; Index++) {
        if (UpgradeData->Levels[Index].Level == Item->Item.UpgradeLevel) {
            LevelData = &UpgradeData->Levels[Index];
            break;
        }
    }

    if (!LevelData) {
        return 0;
    }

    return LevelData->CoreCount;
}

Int32 RTUpgradeLevelNormal(
    RTRuntimeRef Runtime,
    RTItemSlotRef Item,
    Int32* UpgradePoint,
    Int32* Seed
) {
    RTUpgradeDataRef UpgradeData = &Runtime->UpgradeData[RUNTIME_UPGRADE_TYPE_LEVEL_NORMAL];
    RTUpgradeLevelDataRef LevelData = NULL;

    for (Int32 Index = 0; Index < UpgradeData->Count; Index++) {
        if (UpgradeData->Levels[Index].Level == Item->Item.UpgradeLevel) {
            LevelData = &UpgradeData->Levels[Index];
            break;
        }
    }

    if (!LevelData) {
        return RUNTIME_UPGRADE_RESULT_ERROR;
    }

    Int32 TotalRate = 0;
    for (Int32 Index = 0; Index < RUNTIME_UPGRADE_RATE_TYPE_COUNT; Index++) {
        TotalRate += LevelData->Rates[Index];
    }

    TotalRate *= 100;

    Int32 UpgradePointRate = 3000 * (*UpgradePoint) / (3000 + (*UpgradePoint));
    TotalRate += UpgradePointRate;

    Int32 Value = RandomRange(Seed, 0, MAX(0, TotalRate - 1));
    Int32 CurrentRate = 0;

    CurrentRate += LevelData->Rates[RUNTIME_UPGRADE_RATE_TYPE_UPGRADE_1] * 100;
    CurrentRate += UpgradePointRate;
    if (Value < CurrentRate) {
        *UpgradePoint = 0;
        Item->Item.UpgradeLevel += 1;
        return RUNTIME_UPGRADE_RESULT_UPGRADE_1;
    }

    CurrentRate += LevelData->Rates[RUNTIME_UPGRADE_RATE_TYPE_UPGRADE_2] * 100;
    if (Value < CurrentRate) {
        *UpgradePoint = 0;
        Item->Item.UpgradeLevel += 2;
        return RUNTIME_UPGRADE_RESULT_UPGRADE_2;
    }

    CurrentRate += LevelData->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_0] * 100;
    if (Value < CurrentRate) {
        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_0;
    }

    CurrentRate += LevelData->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_1] * 100;
    if (Value < CurrentRate) {
        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        Item->Item.UpgradeLevel -= 1;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_1;
    }

    CurrentRate += LevelData->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_2] * 100;
    if (Value < CurrentRate) {
        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        Item->Item.UpgradeLevel -= 2;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_2;
    }

    CurrentRate += LevelData->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_RESET] * 100;
    if (Value < CurrentRate) {
        Int32 SafeLevel = 0;

        for (Int32 Index = 0; Index < UpgradeData->Count; Index++) {
            if (UpgradeData->Levels[Index].Level <= Item->Item.UpgradeLevel &&
                UpgradeData->Levels[Index].SafeCount < 1) {
                SafeLevel = MAX(SafeLevel, UpgradeData->Levels[Index].Level);
            }
        }

        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        Item->Item.UpgradeLevel = SafeLevel;
        return RUNTIME_UPGRADE_RESULT_DESTROYED;
    }

    FatalError("Random range calculation should always result in perfect ranged matches!");
    return RUNTIME_UPGRADE_RESULT_ERROR;
}

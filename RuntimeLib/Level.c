#include "Level.h"
#include "Runtime.h"

UInt8 RTRuntimeGetLevelByExp(
    RTRuntimeRef Runtime,
    UInt64 Exp
) {
    UInt8 Level = Runtime->Levels[0].Level;
    UInt64 RemainingExp = Exp;

    for (Int32 Index = 0; Index < Runtime->LevelCount; Index++) {
        if (RemainingExp >= Runtime->Levels[Index].Exp) {
            RemainingExp -= Runtime->Levels[Index].Exp;
            
            if (Index < Runtime->LevelCount - 1) {
                UInt8 NextLevel = Runtime->Levels[Index + 1].Level;
                assert(Level + 1 == NextLevel);
                Level = NextLevel;
            }
        }
    }

    return Level;
}

UInt64 RTRuntimeGetExpByLevel(
    RTRuntimeRef Runtime,
    UInt8 Level
) {
    UInt64 Exp = 0;

    for (Int32 Index = 0; Index < MIN(Level, Runtime->LevelCount); Index++) {
        Exp += Runtime->Levels[Index].Exp;
    }

    return Exp;
}
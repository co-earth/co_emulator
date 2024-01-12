#include "ServerDataLoader.h"

#pragma pack(push, 1)

struct _ArchiveItemData {
    Int32 ItemID;
    Char VisualID[60];
    Char ItemName[60];
    Char ItemDescription[60];
    UInt8 Padding1[20];
    UInt32 ItemType;
    UInt32 DisplayCore1;
    Int32 DisplayCore2;
    Float32 TranslationFemale[3];
    Float32 TranslationMale[3];
    Float32 RotationFemale[3];
    Float32 RotationMale[3];
    Float32 ScaleMale;
    Float32 ScaleFemale;
    UInt32 MinLevel;
    UInt32 LimitClass;
    Int32 MinHonorRank;
    UInt32 Unknown1;
    UInt8 Padding2[21];
    Int32 Unknown2;
    UInt32 SellPrice;
    UInt32 BuyPrice;

    struct {
        UInt32 Height : 2;
        UInt32 Width : 2;
    } SlotSize;

    Int32 Level;
    Int32 Options[22];
    UInt32 Property;
    UInt8 EnchantCodeLink;
    UInt8 Unknown3;
    UInt8 Grade;
    UInt8 Unknown4;
    UInt8 BattleStyleIndices[3];
    UInt8 DurationType;
    UInt8 DurationUse;
    UInt8 UniqueGrade;
    Int32 MaxHonorRank;
    UInt8 Unknown5[3];
    Int16 MaxStackSize;
    Int32 Unknown6[7];
};

struct _ArchiveMobData {
    Int32 Level;
    Int32 A;
    Float32 UnknownF;
    Int32 HPRecharge;
    Int32 H[3];
    Int32 DefaultSkillPhysicalAttackMin;
    Int32 DefaultSkillPhysicalAttackMax;
    Int32 DefaultSkillReach;
    Int32 DefaultSkillRange;
    Float32 DefaultSkillInterval;
    Int32 SpecialSkillPhysicalAttackMin;
    Int32 SpecialSkillPhysicalAttackMax;
    Int32 SpecialSkillReach;
    Int32 SpecialSkillRange;
    Float32 SpecialSkillInterval;
    Int32 AlertRange;
    Int32 LimitRangeA;
    Int32 FindCount;
    Float32 UnknownB;
    Int32 AttackPattern;
    Int32 Not_DefaultSkillDefenseSkill;
    Int32 Not_SpecialSkillDefenseSkill;
    UInt64 Exp;
    Int32 DefaultSkillStance;
    Int32 SpecialSkillStance;
    Float32 MovementSpeed;
    Float32 ChaseSpeed;
    Int32 Scale;
    Int32 DefaultSkillGroup;
    Int32 SpecialSkillGroup;
    UInt32 Property;
    Int32 D[5];
    Int32 MoveInterval;
    Int32 E[2];
    Int32 ResistCriticalRate;
    Int32 N[2];
    Int32 LimitRangeB;
    Int32 IgnoreAccuracy;
    Int32 IgnoreDamageReduction;
    Int32 IgnorePenetration;
    Int32 M[2];
    Int32 ResistSkillAmp;
    Int32 ResistCriticalDamage;
    Int32 ResistSuppression;
    Int32 O[4];
    UInt8 K[3];
    Int32 L[5];
};

#pragma pack(pop)

static const CString kBattleStyleIndexKeyWA = "2hand";
static const CString kBattleStyleIndexKeyBL = "dual";
static const CString kBattleStyleIndexKeyWI = "magic";
static const CString kBattleStyleIndexKeyFA = "magic_arrow";
static const CString kBattleStyleIndexKeyFS = "sword_shield";
static const CString kBattleStyleIndexKeyFB = "magic_sword";
static const CString kBattleStyleIndexKeyGL = "chakram";
static const CString kBattleStyleIndexKeyFG = "magic_gun";
static const CString kBattleStyleIndexKeyDM = "magic_skull";

static const CString kBattleStyleIndexKeyAltWA = "twohand";
static const CString kBattleStyleIndexKeyAltBL = "dual";
static const CString kBattleStyleIndexKeyAltWI = "magic";
static const CString kBattleStyleIndexKeyAltFA = "magic_arrow";
static const CString kBattleStyleIndexKeyAltFS = "sword_shield";
static const CString kBattleStyleIndexKeyAltFB = "magic_sword";
static const CString kBattleStyleIndexKeyAltGL = "chakram";
static const CString kBattleStyleIndexKeyAltFG = "magic_gun";
static const CString kBattleStyleIndexKeyAltDM = "magic_skull";

CString GetBattleStyleIndexKey(Int32 BattleStyleIndex) {
    switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return kBattleStyleIndexKeyWA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return kBattleStyleIndexKeyBL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return kBattleStyleIndexKeyWI;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return kBattleStyleIndexKeyFA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return kBattleStyleIndexKeyFS;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return kBattleStyleIndexKeyFB;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return kBattleStyleIndexKeyGL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return kBattleStyleIndexKeyFG;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return kBattleStyleIndexKeyDM;
    default: return NULL;
    }
}

CString GetBattleStyleIndexKeyAlt(Int32 BattleStyleIndex) {
    switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return kBattleStyleIndexKeyAltWA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return kBattleStyleIndexKeyAltBL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return kBattleStyleIndexKeyAltWI;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return kBattleStyleIndexKeyAltFA;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return kBattleStyleIndexKeyAltFS;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return kBattleStyleIndexKeyAltFB;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return kBattleStyleIndexKeyAltGL;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return kBattleStyleIndexKeyAltFG;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return kBattleStyleIndexKeyAltDM;
    default: return NULL;
    }
}

Bool ParseBattleStyleString(
    Archive* Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result
) {
    Char Buffer[20];

    if (!ParseAttributeString(Object, NodeIndex, Name, Buffer, 20)) return false;

    if (strcmp(Buffer, kBattleStyleIndexKeyWA) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyBL) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyWI) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFA) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFS) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFB) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyGL) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyFG) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG;
    }
    else if (strcmp(Buffer, kBattleStyleIndexKeyDM) == 0) {
        *Result = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM;
    }
    else {
        return false;
    }

    return true;
}

Bool ServerLoadCharacterTemplateData(
    ServerContextRef Server
) {
    Archive* Archive = &Server->RuntimeData->Rank;
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.char_init");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "init");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "style", &BattleStyleIndex)) goto error;

        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleLevelFormulaDataRef FormulaData = &Server->Runtime->BattleStyleLevelFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;
        
        if (!ParseAttributeUInt32(Archive, Iterator->Index, "hp", &FormulaData->BaseHP)) goto error;
        if (!ParseAttributeUInt32(Archive, Iterator->Index, "mp", &FormulaData->BaseMP)) goto error;
        if (!ParseAttributeUInt32(Archive, Iterator->Index, "delta_hp", &FormulaData->DeltaHP)) goto error;
        if (!ParseAttributeUInt32(Archive, Iterator->Index, "delta_mp", &FormulaData->DeltaMP)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Archive = &Server->RuntimeData->ServerCharacterInit;
    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "server_character_init");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "character_init");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "style", &BattleStyleIndex)) goto error;

        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        struct _RuntimeDataCharacterTemplate* CharacterTemplate = &Server->RuntimeData->CharacterTemplate[BattleStyleIndex - 1];
        CharacterTemplate->BattleStyleIndex = BattleStyleIndex;
        CharacterTemplate->SkillSlots.Count = 0;

        ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "skill");
        while (ChildIterator) {
            RTSkillSlotRef SkillSlot = &CharacterTemplate->SkillSlots.Skills[CharacterTemplate->SkillSlots.Count];

            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "id", &SkillSlot->ID)) goto error;
            if (!ParseAttributeUInt8(Archive, ChildIterator->Index, "level", &SkillSlot->Level)) goto error;
            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "slot", &SkillSlot->Index)) goto error;

            CharacterTemplate->SkillSlots.Count += 1;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        CharacterTemplate->QuickSlots.Count = 0;

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "quickslot");
        while (ChildIterator) {
            RTQuickSlotRef QuickSlot = &CharacterTemplate->QuickSlots.QuickSlots[CharacterTemplate->QuickSlots.Count];

            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "skill", &QuickSlot->SkillIndex)) goto error;
            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "slot", &QuickSlot->SlotIndex)) goto error;

            CharacterTemplate->QuickSlots.Count += 1;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "inventory");
        while (ChildIterator) {
            struct _RTItemSlot Slot = { 0 };

            if (!ParseAttributeUInt16(Archive, ChildIterator->Index, "slot", &Slot.SlotIndex)) goto error;
            if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "item_id", &Slot.Item.ID)) goto error;
            if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "item_option", &Slot.ItemOptions)) goto error;
            
            if (!RTInventorySetSlot(Server->Runtime, &CharacterTemplate->Inventory, &Slot)) goto error;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadBattleStyleFormulaData(
    ServerContextRef Server
) {
    Archive* Archive = &Server->RuntimeData->Rank;
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.rankup");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "class_formula");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseBattleStyleString(Archive, Iterator->Index, "class", &BattleStyleIndex)) goto error;
        
        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleClassFormulaDataRef FormulaData = &Server->Runtime->BattleStyleClassFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_atk", FormulaData->Attack, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_matk", FormulaData->MagicAttack, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_def", FormulaData->Defense, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_ar", FormulaData->AttackRate, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_dr", FormulaData->DefenseRate, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_dmgdsc", FormulaData->DamageReduction, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_hit", FormulaData->Accuracy, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_penet", FormulaData->Penetration, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_hitpenel", FormulaData->AccuracyPenalty, 2, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "class_penetpenel", FormulaData->PenetrationPenalty, 2, ',')) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    for (Int32 BattleStyleIndex = RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN; BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX; BattleStyleIndex += 1) {
        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleSkillFormulaDataRef FormulaData = &Server->Runtime->BattleStyleSkillFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;
        FormulaData->SkillRankCount = 0;
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "exp_for_point");
    while (Iterator) {
        Int32 SkillRank = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "rank", &SkillRank)) goto error;

        ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "condition");
        while (ChildIterator) {
            Int32 BattleStyleIndex = 0;
            if (!ParseBattleStyleString(Archive, ChildIterator->Index, "class", &BattleStyleIndex)) goto error;

            assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

            RTBattleStyleSkillFormulaDataRef FormulaData = &Server->Runtime->BattleStyleSkillFormulaData[BattleStyleIndex - 1];
            assert(FormulaData->BattleStyleIndex == BattleStyleIndex);
            assert(FormulaData->SkillRankCount < RUNTIME_MEMORY_MAX_SKILL_RANK_DATA_COUNT);

            RTBattleStyleSkillRankDataRef RankData = &FormulaData->SkillRanks[FormulaData->SkillRankCount];
            RankData->SkillRank = SkillRank;

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "exp", &RankData->SkillLevelExp)) goto error;

            FormulaData->SkillRankCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "mastery_levelup");
    while (Iterator) {
        Int32 Level = 0;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "level", &Level)) goto error;

        ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "condition");
        while (ChildIterator) {
            Int32 BattleStyleIndex;
            if (!ParseBattleStyleString(Archive, ChildIterator->Index, "class", &BattleStyleIndex)) goto error;
            
            assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

            RTBattleStyleRankFormulaDataRef FormulaData = &Server->Runtime->BattleStyleRankFormulaData[BattleStyleIndex - 1];
            FormulaData->BattleStyleIndex = BattleStyleIndex;
            assert(FormulaData->RankCount < RUNTIME_MEMORY_MAX_BATTLE_RANK_DATA_COUNT);

            RTBattleStyleRankDataRef RankData = &FormulaData->Ranks[FormulaData->RankCount];
            RankData->Level = Level;

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "str", &RankData->ConditionSTR)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "dex", &RankData->ConditionDEX)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "int", &RankData->ConditionINT)) goto error;

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "skill1idx", &RankData->SkillIndex[0]) ||
                !ParseAttributeInt32(Archive, ChildIterator->Index, "skill1slotidx", &RankData->SkillSlot[0])) {
                RankData->SkillIndex[0] = 0;
                RankData->SkillSlot[0] = 0;
            }

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "skill2idx", &RankData->SkillIndex[1]) ||
                !ParseAttributeInt32(Archive, ChildIterator->Index, "skill2slotidx", &RankData->SkillSlot[1])) {
                RankData->SkillIndex[1] = 0;
                RankData->SkillSlot[1] = 0;
            }

            FormulaData->RankCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.rankup.stat_formula");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "flag");
    while (Iterator) {
        Int32 BattleStyleIndex = 0;
        if (!ParseBattleStyleString(Archive, Iterator->Index, "class", &BattleStyleIndex)) goto error;

        assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

        RTBattleStyleStatsFormulaDataRef FormulaData = &Server->Runtime->BattleStyleStatsFormulaData[BattleStyleIndex - 1];
        FormulaData->BattleStyleIndex = BattleStyleIndex;

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "atk_flag", FormulaData->Attack, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "matk_flag", FormulaData->MagicAttack, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "dmgdcs_flag", FormulaData->DamageReduction, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "flee_flag", FormulaData->Evasion, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "ar_flag", FormulaData->AttackRate, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "dr_flag", FormulaData->DefenseRate, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "crpreg_flag", FormulaData->ResistCriticalRate, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "samreg_flag", FormulaData->ResistSkillAmp, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "crdreg_flag", FormulaData->ResistCriticalDamage, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "movreg_flag", FormulaData->ResistUnmovable, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "stnreg_flag", FormulaData->ResistStun, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "dwnreg_flag", FormulaData->ResistDown, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "nubreg_flag", FormulaData->ResistKnockback, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "hp_flag", FormulaData->HP, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "mp_flag", FormulaData->MP, 3, ',')) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "ignorepenet_flag", FormulaData->IgnorePenetration, 3, ',')) goto error;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "atk_code", &FormulaData->AttackSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "matk_code", &FormulaData->MagicAttackSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dmgdcs_code", &FormulaData->DamageReductionSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "flee_code", &FormulaData->EvasionSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "ar_code", &FormulaData->AttackRateSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dr_code", &FormulaData->DefenseRateSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "crpreg_code", &FormulaData->ResistCriticalRateSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "samreg_code", &FormulaData->ResistSkillAmpSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "crdreg_code", &FormulaData->ResistCriticalDamageSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "movreg_code", &FormulaData->ResistUnmovableSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "stnreg_code", &FormulaData->ResistStunSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dwnreg_code", &FormulaData->ResistDownSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "nubreg_code", &FormulaData->ResistKnockbackSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "hp_code", &FormulaData->HPSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mp_code", &FormulaData->MPSlopeID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "ignorepenet_code", &FormulaData->IgnorePenetrationSlopeID)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "parameter");
    while (Iterator) {
        assert(Server->Runtime->SlopeFormulaDataCount < RUNTIME_MEMORY_MAX_SLOPE_FORMULA_COUNT);

        RTBattleStyleSlopeFormulaDataRef FormulaData = &Server->Runtime->BattleStyleSlopeFormulaData[Server->Runtime->SlopeFormulaDataCount];
        FormulaData->SlopeCount = 0;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "code", &FormulaData->SlopeID)) goto error;

        ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "condition");
        while (ChildIterator) {
            assert(FormulaData->SlopeCount < RUNTIME_MEMORY_MAX_SLOPE_DATA_COUNT);

            RTBattleStyleSlopeDataRef SlopeData = &FormulaData->Slopes[FormulaData->SlopeCount];

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "penalty_stat", &SlopeData->Penalty)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "slope", &SlopeData->Slope)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "intercept", &SlopeData->Intercept)) goto error;

            FormulaData->SlopeCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        Server->Runtime->SlopeFormulaDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadQuestData(
    ServerContextRef Server
) {
    Archive* Archive = &Server->RuntimeData->Quest;
    RTRuntimeRef Runtime = Server->Runtime;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest.cabal_quest");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "quest");
    while (Iterator) {
        assert(Runtime->QuestDataCount < RUNTIME_MEMORY_MAX_QUEST_DATA_COUNT);

        RTQuestDataRef Quest = &Runtime->QuestData[Runtime->QuestDataCount];
        Quest->Index = Runtime->QuestDataCount;
        Quest->QuestBeginNpcIndex = -1;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "quest_id", &Quest->ID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "type", &Quest->Type)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "party", &Quest->Party)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_order", &Quest->MissionOrder)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_set", &Quest->MissionSet)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "delete_type", &Quest->DeleteType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dailycount", &Quest->DailyCount)) goto error;

        ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "condition");
        while (ChildIterator) {
            RTQuestConditionDataRef Condition = &Quest->Condition;

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "level", &Condition->Level)) goto error;
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "class", Condition->Class, 2, ':')) goto error;
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "item", Condition->Item, 3, ':')) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "maxlv", &Condition->MaxLevel)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "rank", &Condition->Rank)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "rank_type", &Condition->RankType)) goto error;
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "skill", Condition->Skill, 2, ':')) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "min_reputation_class", &Condition->MinHonorRank)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "max_reputation_class", &Condition->MaxHonorRank)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "related_qid", &Condition->RelatedQuestID)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "nation_type", &Condition->NationType)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MinOLv", &Condition->MinOverlordLevel)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MaxOLv", &Condition->MaxOverlordLevel)) goto error;

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            assert(!ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "mission");
        while (ChildIterator) {
            Int32 Type;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "type", &Type)) goto error;

            if (Type == 0) {
                assert(Quest->MissionMobCount < RUNTIME_MAX_QUEST_COUNTER_COUNT);

                RTQuestMissionDataRef Mission = &Quest->MissionMobs[Quest->MissionMobCount];
                memset(Mission, 0, sizeof(struct _RTQuestMissionData));

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Mission->Order)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "notify_order", &Mission->NotifyOrder)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "value", &Mission->Value)) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "value", Mission->Value, 2, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where1", Mission->Location1, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where2", Mission->Location2, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where3", Mission->Location3, 3, ':')) goto error;

                Quest->MissionMobCount += 1;
            }
            else if (Type == 1) {
                assert(Quest->MissionItemCount < RUNTIME_MAX_QUEST_COUNTER_COUNT);

                RTQuestMissionDataRef Mission = &Quest->MissionItems[Quest->MissionItemCount];

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Mission->Order)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "notify_order", &Mission->NotifyOrder)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "value", &Mission->Value)) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "value", Mission->Value, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where1", Mission->Location1, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where2", Mission->Location2, 3, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where3", Mission->Location3, 3, ':')) goto error;

                Quest->MissionItemCount += 1;
            }
            else if (Type == 2) {
                assert(Quest->MissionDungeonCount < RUNTIME_MAX_QUEST_COUNTER_COUNT);

                RTQuestMissionDataRef Mission = &Quest->MissionDungeons[Quest->MissionDungeonCount];

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Mission->Order)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "notify_order", &Mission->NotifyOrder)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "value", &Mission->Value)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "value", &Mission->Value[0])) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where1", Mission->Location1, 0, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where2", Mission->Location2, 0, ':')) goto error;
                if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "where3", Mission->Location3, 0, ':')) goto error;

                Quest->MissionDungeonCount += 1;
            }
            else {
                assert(0);
            }
            
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "param");
        while (ChildIterator) {
            if (!ParseAttributeInt32Array(Archive, ChildIterator->Index, "reward", Quest->Reward, 9, ',')) goto error;
            
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            assert(!ChildIterator);
        }

        ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "npcs");
        while (ChildIterator) {
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "npcset_id", &Quest->NpcSet.ID)) goto error;

            ArchiveIterator* NpcIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "quest_npc");
            while (NpcIterator) {
                assert(Quest->NpcSet.Count < RUNTIME_MEMORY_MAX_QUEST_NPC_COUNT);

                RTQuestNpcDataRef Npc = &Quest->NpcSet.Npcs[Quest->NpcSet.Count];
                Int32 NpcID[2];

                if (!ParseAttributeInt32(Archive, NpcIterator->Index, "unique_order", &Npc->Index)) goto error;
                if (!ParseAttributeInt32Array(Archive, NpcIterator->Index, "npc_id", NpcID, 2, ',')) goto error;
                
                Npc->WorldID = NpcID[0];
                Npc->NpcID = NpcID[1];

                if (!ParseAttributeInt32(Archive, NpcIterator->Index, "npc_action_order", &Npc->NpcActionOrder)) goto error;
                if (!ParseAttributeInt32(Archive, NpcIterator->Index, "action_type", &Npc->ActionType)) goto error;
                if (!ParseAttributeInt32(Archive, NpcIterator->Index, "value", &Npc->Value)) goto error;

                ParseAttributeInt32Array(Archive, NpcIterator->Index, "notify", Npc->Notification, 2, ':');

                if (Npc->ActionType == RUNTIME_QUEST_ACTION_TYPE_BEGIN) {
                    Quest->QuestBeginNpcIndex = Quest->NpcSet.Count;
                }

                Quest->NpcSet.Count += 1;
                NpcIterator = ArchiveQueryNodeIteratorNext(Archive, NpcIterator);
            }

            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            assert(!ChildIterator);
        }

        // TODO: Add dungeon

        Runtime->QuestDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest.cabal_reward");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "reward_item_set");
    while (Iterator) {
        assert(Runtime->QuestRewardItemSetDataCount < RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_SET_DATA_COUNT);

        RTQuestRewardItemSetDataRef ItemSet = &Runtime->QuestRewardItemSetData[Runtime->QuestRewardItemSetDataCount];
        ItemSet->Count = 0;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &ItemSet->ID)) goto error;
        
        ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "reward_item");
        while (ChildIterator) {
            assert(ItemSet->Count < RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_DATA_COUNT);

            RTQuestRewardItemDataRef Item = &ItemSet->Items[ItemSet->Count];
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "class", &Item->BattleStyleIndex)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "order", &Item->Index)) goto error;
            if (!ParseAttributeUInt32Array(Archive, ChildIterator->Index, "item_id", Item->ItemID, 2, ':')) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "duration", &Item->ItemDuration)) goto error;

            ItemSet->Count += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        Runtime->QuestRewardItemSetDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadWarpData(
    ServerContextRef Server
) {
    Archive* Archive = &Server->RuntimeData->Main;
    RTRuntimeRef Runtime = Server->Runtime;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.warp_point");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "warp_index");
    while (Iterator) {
        assert(Runtime->WarpIndexCount < RUNTIME_MEMORY_MAX_WARP_INDEX_COUNT);

        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[Runtime->WarpIndexCount];
        WarpIndex->Index = Runtime->WarpIndexCount;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "WorldIdx", &WarpIndex->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "x", &WarpIndex->Position.X)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "y", &WarpIndex->Position.Y)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "nation1x", &WarpIndex->Target[0].X)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "nation1y", &WarpIndex->Target[0].Y)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "nation2x", &WarpIndex->Target[1].X)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "nation2y", &WarpIndex->Target[1].Y)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "nation3x", &WarpIndex->Target[2].X)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "nation3y", &WarpIndex->Target[2].Y)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Fee", &WarpIndex->Fee)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "level", &WarpIndex->Level)) goto error;

        Runtime->WarpIndexCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.warp_npc");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* WorldIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "world");
    while (WorldIterator) {
        Int32 WorldID;
        if (!ParseAttributeInt32(Archive, WorldIterator->Index, "id", &WorldID)) goto error;

        ArchiveIterator* NpcIterator = ArchiveQueryNodeIteratorFirst(Archive, WorldIterator->Index, "npc");
        while (NpcIterator) {
            Int32 NpcID;
            if (!ParseAttributeInt32(Archive, NpcIterator->Index, "id", &NpcID)) goto error;

            ArchiveIterator* WarpIterator = ArchiveQueryNodeIteratorFirst(Archive, NpcIterator->Index, "warp_list");
            while (WarpIterator) {
                assert(Runtime->WarpCount < RUNTIME_MEMORY_MAX_WARP_COUNT);

                RTWarpRef Warp = &Runtime->Warps[Runtime->WarpCount];
                Warp->WorldID = WorldID;
                Warp->NpcID = NpcID;

                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "order", &Warp->Index)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "type", &Warp->Type)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "target_id", &Warp->TargetID)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "level", &Warp->Level)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "Fee", &Warp->Fee)) goto error;
                if (!ParseAttributeInt32(Archive, WarpIterator->Index, "quest_id", &Warp->QuestID)) goto error;
                if (!ParseAttributeUInt32Array(Archive, WarpIterator->Index, "warp_item", &Warp->WarpItemID, 2)) goto error;
    
                Runtime->WarpCount += 1;
                WarpIterator = ArchiveQueryNodeIteratorNext(Archive, WarpIterator);
            }

            NpcIterator = ArchiveQueryNodeIteratorNext(Archive, NpcIterator);
        }

        WorldIterator = ArchiveQueryNodeIteratorNext(Archive, WorldIterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadLevelData(
    ServerContextRef Server
) {
    Archive* Archive = &Server->RuntimeData->Main;
    RTRuntimeRef Runtime = Server->Runtime;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.level_up");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "con");
    while (Iterator) {
        assert(Runtime->LevelCount < RUNTIME_MEMORY_MAX_LEVEL_COUNT);

        RTLevelRef Level = &Runtime->Levels[Runtime->LevelCount];

        if (!ParseAttributeUInt8(Archive, Iterator->Index, "level", &Level->Level)) goto error;
        if (!ParseAttributeUInt64(Archive, Iterator->Index, "exp", &Level->Exp)) goto error;

        Runtime->LevelCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadItemData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Server->Runtime;

    CString FileNames[] = {
        "item.enc",
        "item2.enc",
        "item3.enc",
        "item4.enc"
    };
    
    for (Int32 FileIndex = 0; FileIndex < 4; FileIndex++) {
        Char FilePath[MAX_PATH];
        PathCombine(RuntimeDirectory, FileNames[FileIndex], FilePath);

        UInt8* Buffer = NULL;
        Int32 BufferLength = 0;

        if (!EncryptionDecryptFile(FilePath, &Buffer, &BufferLength)) {
            goto error;
        }

        struct _ArchiveItemData* ItemData = (struct _ArchiveItemData*)Buffer;
        Int32 ItemCount = BufferLength / sizeof(struct _ArchiveItemData);

        for (Int32 Index = 0; Index < ItemCount; Index++) {
            assert(Runtime->ItemDataCount < RUNTIME_MEMORY_MAX_ITEM_DATA_COUNT);

            struct _ArchiveItemData* ArchiveItemData = Buffer + sizeof(struct _ArchiveItemData) * Index;
            RTItemDataRef ItemData = &Runtime->ItemData[Runtime->ItemDataCount];
            ItemData->ItemID = ArchiveItemData->ItemID;
            ItemData->ItemType = ArchiveItemData->ItemType;
            ItemData->SellPrice = ArchiveItemData->SellPrice;
            ItemData->BuyPrice = ArchiveItemData->BuyPrice;
            ItemData->MinLevel = ArchiveItemData->MinLevel;
            memcpy(ItemData->Options, ArchiveItemData->Options, sizeof(ArchiveItemData->Options));
            ItemData->Width = ArchiveItemData->SlotSize.Width;
            ItemData->Height = ArchiveItemData->SlotSize.Height;
            ItemData->MaxStackSize = ArchiveItemData->MaxStackSize;

            Runtime->ItemDataCount += 1;
        }

        free(Buffer);
    }

    return true;

error:
    return false;
}

Bool ServerLoadMobData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Server->Runtime;

    Char MobFilePath[MAX_PATH];
    PathCombine(ServerDirectory, "mob.xml", MobFilePath);

    Archive ArchiveData;
    Archive* Archive = &ArchiveData;
    if (!ArchiveLoadFromFile(Archive, MobFilePath, false)) goto error;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "mobs");
    if (ParentIndex < 0) goto error;

    Int32 SkillGroupUsage[50] = { 0 };

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "mob");
    while (Iterator) {
        Int32 Index;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "order", &Index)) goto error;

        RTMobDataRef MobData = &Runtime->MobData[Index];
        MobData->Index = Index;
        Runtime->MobDataCount += 1;

        if (!ParseAttributeFloat32(Archive, Iterator->Index, "MoveSpeed", &MobData->MoveSpeed)) goto error;
        if (!ParseAttributeFloat32(Archive, Iterator->Index, "ChasSpeed", &MobData->ChaseSpeed)) goto error;
        if (!ParseAttributeFloat32(Archive, Iterator->Index, "Radius", &MobData->Radius)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Property", &MobData->Property)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "AttkPattern", &MobData->AttackPattern)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Aggressive", &MobData->AggressiveType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Cooperate", &MobData->Cooperate)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Escape", &MobData->Escape)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Attack", &MobData->AttackType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Scale", &MobData->Scale)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "FindCount", &MobData->FindCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "FindInterval", &MobData->FindInterval)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "MoveInterval", &MobData->MoveInterval)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "ChasInterval", &MobData->ChaseInterval)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "AlertRange", &MobData->AlertRange)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Limt0Range", &MobData->LimitRangeA)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Limt1Range", &MobData->LimitRangeB)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "LEV", &MobData->Level)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "HP", &MobData->HP)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Defense", &MobData->Defense)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "AttacksR", &MobData->AttackRate)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "DefenseR", &MobData->DefenseRate)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "HPRechagR", &MobData->HpRecharge)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "AtkSignal", &MobData->AttackSignal)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Boss", &MobData->Boss)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "canatk", &MobData->CanAttack)) goto error;
        if (!ParseAttributeUInt64(Archive, Iterator->Index, "EXP", &MobData->Exp)) goto error;
        
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PhyAttMin1", &MobData->DefaultSkill.PhysicalAttackMin)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PhyAttMax1", &MobData->DefaultSkill.PhysicalAttackMax)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Reach1", &MobData->DefaultSkill.Reach)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Range1", &MobData->DefaultSkill.Range)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Group1", &MobData->DefaultSkill.SkillGroup)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Stance1", &MobData->DefaultSkill.Stance)) goto error;
        if (!ParseAttributeUInt64(Archive, Iterator->Index, "Interval1", &MobData->DefaultSkill.Interval)) goto error;
        
        MobData->DefaultSkill.IsDefenseSkill = true;

        Int32 Radius = (MobData->Scale + 1) >> 1;
        MobData->DefaultSkill.Distance = (MobData->Scale >> 1) + MobData->DefaultSkill.Reach;
        MobData->DefaultSkill.Offset = Radius + ((MobData->DefaultSkill.Reach - Radius) >> 1);
        if (MobData->DefaultSkill.Offset < Radius) {
            MobData->DefaultSkill.Offset = Radius;
        }

        if (!ParseAttributeInt32(Archive, Iterator->Index, "PhyAttMin2", &MobData->SpecialSkill.PhysicalAttackMin)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PhyAttMax2", &MobData->SpecialSkill.PhysicalAttackMax)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Reach2", &MobData->SpecialSkill.Reach)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Range2", &MobData->SpecialSkill.Range)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Group2", &MobData->SpecialSkill.SkillGroup)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Stance2", &MobData->SpecialSkill.Stance)) goto error;
        if (!ParseAttributeUInt64(Archive, Iterator->Index, "Interval2", &MobData->SpecialSkill.Interval)) goto error;

        MobData->SpecialSkill.IsDefenseSkill = false;
        MobData->SpecialSkill.Distance = (MobData->Scale >> 1) + MobData->SpecialSkill.Reach;
        MobData->SpecialSkill.Offset = Radius + ((MobData->SpecialSkill.Reach - Radius) >> 1);
        if (MobData->SpecialSkill.Offset < Radius) {
            MobData->SpecialSkill.Offset = Radius;
        }

        assert(MobData->DefaultSkill.SkillGroup < 50);
        SkillGroupUsage[MobData->DefaultSkill.SkillGroup] += 1;
        SkillGroupUsage[MobData->SpecialSkill.SkillGroup] += 1;

        MobData->CanMove = (MobData->FindCount >= RUNTIME_MOB_FIND_COUNT_UNMOVABLE) ? 0 : 1;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    for (Int32 Index = 0; Index < 50; Index += 1) {
        if (SkillGroupUsage[Index] > 0)
            LogMessageFormat(LOG_LEVEL_WARNING, "Mob Skill Group: %d", Index);
    }

    CString FilePath = PathCombineNoAlloc(RuntimeDirectory, "mobserver.dat");
    UInt8* Memory = NULL;
    Int32 MemoryLength;
    FileRef File = FileOpen(FilePath);
    FileRead(File, &Memory, &MemoryLength);

    struct _ArchiveMobData* MobData = Memory;

    memset(SkillGroupUsage, 0, sizeof(SkillGroupUsage));

    for (Int32 Index = 0; Index < 3736; Index += 1) {
        SkillGroupUsage[MobData[Index].DefaultSkillGroup] += 1;
        SkillGroupUsage[MobData[Index].SpecialSkillGroup] += 1;
    }

    for (Int32 Index = 0; Index < 50; Index += 1) {
        if (SkillGroupUsage[Index] > 0)
            LogMessageFormat(LOG_LEVEL_WARNING, "Mob Skill Group: %d", Index);
    }

    FileClose(File);
    free(Memory);

    return true;

error:
    return false;
}

Bool ServerLoadShopData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Server->Runtime;

    Char ArchiveFilePath[MAX_PATH];
    PathCombine(ServerDirectory, "shop.xml", ArchiveFilePath);

    Archive ArchiveData;
    Archive* Archive = &ArchiveData;
    if (!ArchiveLoadFromFile(Archive, ArchiveFilePath, false)) goto error;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "shops");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "shop");
    while (Iterator) {
        assert(Runtime->ShopDataCount < RUNTIME_MEMORY_MAX_SHOP_DATA_COUNT);

        RTShopDataRef ShopData = &Runtime->ShopData[Runtime->ShopDataCount];
        ShopData->Index = Runtime->ShopDataCount;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &ShopData->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "npc_id", &ShopData->NpcID)) goto error;

        ArchiveIterator* ItemIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "item");
        while (ItemIterator) {
            assert(ShopData->ItemCount < RUNTIME_SHOP_MAX_ITEM_COUNT);

            RTShopItemDataRef ItemData = &ShopData->Items[ShopData->ItemCount];

            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "slot_id", &ItemData->SlotID)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "item_id", &ItemData->ItemID)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "option", &ItemData->ItemOption)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "reputation_class", &ItemData->MinHonorRank)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "duration_id", &ItemData->DurationID)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "price", &ItemData->Price)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "onlypremium", &ItemData->OnlyPremium)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "wexpprice", &ItemData->WexpPrice)) goto error;
            if (!ParseAttributeInt32(Archive, ItemIterator->Index, "dp", &ItemData->DpPrice)) goto error;

            ShopData->ItemCount += 1;
            ItemIterator = ArchiveQueryNodeIteratorNext(Archive, ItemIterator);
        }

        Runtime->ShopDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadUpgradeData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Server->Runtime;

    Char FilePath[MAX_PATH];
    PathCombine(ServerDirectory, "upgrade.xml", FilePath);

    Archive ArchiveData;
    Archive* Archive = &ArchiveData;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "upgrade.normal_upgrade");
    if (ParentIndex < 0) goto error;

    RTUpgradeDataRef UpgradeData = &Runtime->UpgradeData[RUNTIME_UPGRADE_TYPE_LEVEL_NORMAL];

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "level");
    while (Iterator) {
        assert(UpgradeData->Count < RUNTIME_UPGRADE_MAX_LEVEL_COUNT);

        RTUpgradeLevelDataRef LevelData = &UpgradeData->Levels[UpgradeData->Count];
        UpgradeData->Count += 1;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "level", &LevelData->Level)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "rates", LevelData->Rates, 6, ',')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "core_count", &LevelData->CoreCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "safe_count", &LevelData->SafeCount)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadWorldData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory,
    Bool LoadShops
) {
    Archive* Archive = &Server->RuntimeData->Main;
    RTRuntimeRef Runtime = Server->Runtime;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.map");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "map_index");
    while (Iterator) {
        RTWorldIndex WorldIndex = RTRuntimeCreateWorld(Runtime);
        RTWorldRef World = &Runtime->Worlds[WorldIndex];

        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &World->WorldIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp", &World->DeadWarpIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "return_warp", &World->ReturnWarpIndex)) goto error;

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    /* TODO: Parse Quest Dungeon Data
    */

    Int32 HasMapCode = 0;
    Int32 GpsOrder = 0;
    Int32 WarAllowed = 0;
    Int32 WarControl = 0;
    Char MapFileName[MAX_PATH];
    Char MapFilePath[MAX_PATH];

    for (Int32 Index = 0; Index < Runtime->WorldCount; Index++) {
        RTWorldRef World = &Runtime->Worlds[Index];

        Archive = &Server->RuntimeData->Main;
        Int32 NodeIndex = ArchiveQueryNodeWithAttribute(
            Archive,
            -1,
            "cabal.cabal_world.world",
            "id",
            Int32ToStringNoAlloc(World->WorldIndex)
        );

        if (NodeIndex < 0) goto error;

        if (!ParseAttributeUInt32(Archive, NodeIndex, "type", &World->Type)) goto error;
        if (!ParseAttributeString(Archive, NodeIndex, "map_file", MapFileName, MAX_PATH)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "m_code", &HasMapCode)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "gpsorder", &World->MapCodeIndex)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "allowedwar", &WarAllowed)) goto error;
        if (!ParseAttributeInt32(Archive, NodeIndex, "warcontrol", &WarControl)) goto error;

        if (HasMapCode) World->Flags |= RUNTIME_WORLD_FLAGS_HAS_MAPCODE;
        if (WarAllowed) World->Flags |= RUNTIME_WORLD_FLAGS_WAR_ALLOWED;
        if (WarControl) World->Flags |= RUNTIME_WORLD_FLAGS_WAR_CONTROL;

        if (strlen(MapFileName) > 0) {
            PathCombine(RuntimeDirectory, "Map", MapFilePath);
            PathCombine(MapFilePath, MapFileName, MapFilePath);
            LogMessageFormat(LOG_LEVEL_INFO, "Loading map file: %s", MapFileName);

            FileRef MapFile = FileOpen(MapFilePath);
            if (!MapFile) goto error;

            UInt8* MapData = NULL;
            Int32 MapDataLength = 0;

            if (!FileRead(MapFile, &MapData, &MapDataLength)) {
                FileClose(MapFile);
                goto error;
            }

            FileClose(MapFile);

            UInt64 MapDataOffset = 0;
            UInt32 MapFileMagic = *((UInt32*)&MapData[MapDataOffset]);
            MapDataOffset += 132;
            if (MapFileMagic == 1006) {
                MapDataOffset += 4;
            }

            UInt32 EffectCount = *((UInt32*)&MapData[MapDataOffset]);
            MapDataOffset += 4;

            for (Int32 Index = 0; Index < EffectCount; Index++) {
                UInt16 TextLength = *((UInt16*)&MapData[MapDataOffset]);
                MapDataOffset += 18;
                MapDataOffset += TextLength;
            }

            UInt32 TextureCount = *((UInt32*)&MapData[MapDataOffset]);
            MapDataOffset += 4;

            for (Int32 Index = 0; Index < TextureCount; Index++) {
                UInt32 Size = *((UInt32*)&MapData[MapDataOffset]);
                MapDataOffset += 4;
                MapDataOffset += Size;
            }

            MapDataOffset += 12;
            MapDataOffset += 257 * 257 * 4;
            Int32 Size = sizeof(World->Tiles);
            UInt32* TileData = (UInt32*)&MapData[MapDataOffset];
            memcpy(World->Tiles, TileData, sizeof(World->Tiles));
            free(MapData);
        }

        ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, NodeIndex, "world_npc");
        while (ChildIterator) {
            assert(Runtime->NpcCount < RUNTIME_MEMORY_MAX_NPC_COUNT);

            RTNpcRef Npc = &Runtime->Npcs[Runtime->NpcCount];

            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "id", &Npc->ID)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "x", &Npc->X)) goto error;
            if (!ParseAttributeInt32(Archive, ChildIterator->Index, "y", &Npc->Y)) goto error;

            Npc->WorldID = World->WorldIndex;

            Runtime->NpcCount += 1;
            ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
        }

        if (LoadShops) {
            ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, NodeIndex, "shop");
            while (ChildIterator) {
                assert(Runtime->ShopDataCount < RUNTIME_MEMORY_MAX_SHOP_DATA_COUNT);

                RTShopDataRef ShopData = &Runtime->ShopData[Runtime->ShopDataCount];
                ShopData->Index = Runtime->ShopDataCount;
                ShopData->WorldID = World->WorldIndex;

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "id", &ShopData->NpcID)) goto error;

                for (Int32 Index = 0; Index < Runtime->ShopDataCount; Index++) {
                    if (Runtime->ShopData[Index].NpcID == ShopData->NpcID) {
                        continue;
                    }
                }

                ArchiveIterator* ItemIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "item");
                while (ItemIterator) {
                    assert(ShopData->ItemCount < RUNTIME_SHOP_MAX_ITEM_COUNT);

                    RTShopItemDataRef ItemData = &ShopData->Items[ShopData->ItemCount];

                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "slot_id", &ItemData->SlotID)) goto error;
                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "item_id", &ItemData->ItemID)) goto error;
                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "option", &ItemData->ItemOption)) goto error;
                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "reputation_class", &ItemData->MinHonorRank)) goto error;
                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "duration_id", &ItemData->DurationID)) goto error;
                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "price", &ItemData->Price)) goto error;
                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "onlypremium", &ItemData->OnlyPremium)) goto error;
                    if (!ParseAttributeInt32(Archive, ItemIterator->Index, "wexpprice", &ItemData->WexpPrice)) goto error;

                    ShopData->ItemCount += 1;
                    ItemIterator = ArchiveQueryNodeIteratorNext(Archive, ItemIterator);
                }

                Runtime->ShopDataCount += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }
        }

        // Load Trainer Data
        {
            ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, NodeIndex, "trainer");
            while (ChildIterator) {
                assert(Runtime->TrainerDataCount < RUNTIME_MEMORY_MAX_TRAINER_DATA_COUNT);

                RTTrainerDataRef TrainerData = &Runtime->TrainerData[Runtime->TrainerDataCount];
                TrainerData->Index = Runtime->TrainerDataCount;
                TrainerData->WorldID = World->WorldIndex;

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "id", &TrainerData->NpcID)) goto error;

                for (Int32 Index = 0; Index < Runtime->TrainerDataCount; Index++) {
                    if (Runtime->TrainerData[Index].NpcID == TrainerData->NpcID) {
                        continue;
                    }
                }

                ArchiveIterator* SkillIterator = ArchiveQueryNodeIteratorFirst(Archive, ChildIterator->Index, "skill");
                while (SkillIterator) {
                    assert(TrainerData->SkillCount < RUNTIME_TRAINER_MAX_SKILL_COUNT);

                    RTTrainerSkillDataRef SkillData = &TrainerData->Skills[TrainerData->SkillCount];

                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "id", &SkillData->ID)) goto error;
                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "slot_id", &SkillData->SlotID)) goto error;
                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "level", &SkillData->Level)) goto error;
                    if (!ParseAttributeInt32(Archive, SkillIterator->Index, "skill_book", &SkillData->SkillBookID)) goto error;
                    
                    TrainerData->SkillCount += 1;
                    SkillIterator = ArchiveQueryNodeIteratorNext(Archive, SkillIterator);
                }

                Runtime->TrainerDataCount += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }
        }

        Char MobFilePath[MAX_PATH];
        Char MobFileName[MAX_PATH];
        sprintf(MobFileName, "world%d-mmap.xml", World->WorldIndex);
        PathCombine(ServerDirectory, MobFileName, MobFilePath);
        if (FileExists(MobFilePath)) {
            Archive = &Server->RuntimeData->Temporary;
            if (!ArchiveLoadFromFile(Archive, MobFilePath, false)) goto error;

            Int32 MobParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "world");
            if (MobParentIndex < 0) goto error;

            ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, MobParentIndex, "spawn");
            while (ChildIterator) {
                assert(World->MobTable.Count < RUNTIME_MEMORY_MAX_MOB_COUNT);

                RTMobRef Mob = &World->MobTable.Mobs[World->MobTable.Count];
                Mob->ID.EntityIndex = World->MobTable.Count + 1;
                Mob->ID.WorldIndex = World->WorldIndex;
                Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpeciesIdx", &Mob->Spawn.MobSpeciesID)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PosX", &Mob->Spawn.AreaX)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PosY", &Mob->Spawn.AreaY)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Width", &Mob->Spawn.AreaWidth)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Height", &Mob->Spawn.AreaHeight)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpwnInterval", &Mob->Spawn.SpawnInterval)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "SpawnDefault", &Mob->Spawn.SpawnDefault)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MissionGate", &Mob->Spawn.MissionGate)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Type", &Mob->Spawn.Type)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Min", &Mob->Spawn.Min)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Max", &Mob->Spawn.Max)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Authority", &Mob->Spawn.Authority)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Server_Mob", &Mob->Spawn.ServerMobID)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "Loot_Delay", &Mob->Spawn.LootDelay)) goto error;

                Mob->Data = &Runtime->MobData[Mob->Spawn.MobSpeciesID];
                Mob->IsInfiniteSpawn = true;
                Mob->IsPermanentDeath = false;
                Mob->RemainingSpawnCount = 0;

                World->MobTable.Count += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveUnload(Archive);

            Archive = &Server->RuntimeData->Main;
        }

        Char DropWorldFilePath[MAX_PATH];
        Char DropWorldFileName[MAX_PATH];
        Float32 DropWorldRates[RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT] = { 0 };
        Float32 DropWorldMinRate = 100;

        sprintf(DropWorldFileName, "world%d-terrain-world.xml", World->WorldIndex);
        PathCombine(ServerDirectory, DropWorldFileName, DropWorldFilePath);
        if (FileExists(DropWorldFilePath)) {
            Archive = &Server->RuntimeData->Temporary;
            if (!ArchiveLoadFromFile(Archive, DropWorldFilePath, false)) goto error;

            Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                assert(World->DropTable.WorldItemCount < RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT);

                RTWorldDropItemRef DropItem = &World->DropTable.WorldItems[World->DropTable.WorldItemCount];

                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->Item.ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->Item.ItemOptions)) goto error;
                if (!ParseAttributeFloat32(Archive, ChildIterator->Index, "DropRate", &DropWorldRates[World->DropTable.WorldItemCount])) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MinLv", &DropItem->MinMobLevel)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "MaxLv", &DropItem->MaxMobLevel)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "OptPoolIdx", &DropItem->Item.OptionPoolIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "DurationIdx", &DropItem->Item.DurationIndex)) goto error;

                DropWorldMinRate = MIN(DropWorldMinRate, DropWorldRates[World->DropTable.WorldItemCount]);
                World->DropTable.WorldItemCount += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveUnload(Archive);

            Archive = &Server->RuntimeData->Main;
        }

        for (Int32 Index = 0; Index < World->DropTable.WorldItemCount; Index++) {
            World->DropTable.WorldItems[Index].Item.DropRate = (Int64)(1000.0f * DropWorldRates[Index] / DropWorldMinRate);
            assert(World->DropTable.WorldItems[Index].Item.DropRate > 0);
        }

        Char DropMobFilePath[MAX_PATH];
        Char DropMobFileName[MAX_PATH]; 
        Float32 DropMobRates[RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT] = { 0 };
        Float32 DropMobMinRate = 100;

        sprintf(DropMobFileName, "world%d-terrain-mob.xml", World->WorldIndex);
        PathCombine(ServerDirectory, DropMobFileName, DropMobFilePath);
        if (FileExists(DropMobFilePath)) {
            Archive = &Server->RuntimeData->Temporary;
            if (!ArchiveLoadFromFile(Archive, DropMobFilePath, false)) goto error;

            Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                assert(World->DropTable.MobItemCount < RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT);

                RTMobDropItemRef DropItem = &World->DropTable.MobItems[World->DropTable.MobItemCount];

                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "SpeciesIdx", &DropItem->MobSpeciesID)) goto error;
                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->Item.ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->Item.ItemOptions)) goto error;
                if (!ParseAttributeFloat32(Archive, ChildIterator->Index, "DropRate", &DropMobRates[World->DropTable.MobItemCount])) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "OptPoolIdx", &DropItem->Item.OptionPoolIndex)) goto error;
                if (!ParseAttributeInt32(Archive, ChildIterator->Index, "DurationIdx", &DropItem->Item.DurationIndex)) goto error;

                DropMobMinRate = MIN(DropMobMinRate, DropMobRates[World->DropTable.MobItemCount]);
                World->DropTable.MobItemCount += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveUnload(Archive);

            Archive = &Server->RuntimeData->Main;
        }

        for (Int32 Index = 0; Index < World->DropTable.MobItemCount; Index++) {
            World->DropTable.MobItems[Index].Item.DropRate = (Int32)(1000.0f * DropMobRates[Index] / DropMobMinRate);
            assert(World->DropTable.MobItems[Index].Item.DropRate);
        }

        Char DropQuestFilePath[MAX_PATH];
        Char DropQuestFileName[MAX_PATH];
        Float32 DropQuestRates[RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT] = { 0 };
        Float32 DropQuestMinRate = 100;

        sprintf(DropQuestFileName, "world%d-terrain-quest.xml", World->WorldIndex);
        PathCombine(ServerDirectory, DropQuestFileName, DropQuestFilePath);
        if (FileExists(DropQuestFilePath)) {
            Archive = &Server->RuntimeData->Temporary;
            if (!ArchiveLoadFromFile(Archive, DropQuestFilePath, false)) goto error;

            Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "drops");
            if (ParentIndex < 0) goto error;

            ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "item");
            while (ChildIterator) {
                assert(World->DropTable.QuestItemCount < RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT);

                RTQuestDropItemRef DropItem = &World->DropTable.QuestItems[World->DropTable.QuestItemCount];

                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "SpeciesIdx", &DropItem->MobSpeciesID)) goto error;
                if (!ParseAttributeUInt32(Archive, ChildIterator->Index, "ItemKind", &DropItem->Item.ItemID.ID)) goto error;
                if (!ParseAttributeUInt64(Archive, ChildIterator->Index, "ItemOpt", &DropItem->Item.ItemOptions)) goto error;
                if (!ParseAttributeFloat32(Archive, ChildIterator->Index, "DropRate", &DropQuestRates[World->DropTable.QuestItemCount])) goto error;

                DropQuestMinRate = MIN(DropQuestMinRate, DropQuestRates[World->DropTable.QuestItemCount]);
                World->DropTable.QuestItemCount += 1;
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);
            }

            ArchiveUnload(Archive);

            Archive = &Server->RuntimeData->Main;
        }

        for (Int32 Index = 0; Index < World->DropTable.QuestItemCount; Index++) {
            World->DropTable.QuestItems[Index].Item.DropRate = (Int32)(1000.0f * DropQuestRates[Index] / DropQuestMinRate);
            assert(World->DropTable.QuestItems[Index].Item.DropRate > 0);
        }
    }

    return true;

error:
    return false;
}


Bool ServerLoadSkillData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    RTRuntimeRef Runtime = Server->Runtime;
    Archive* Archive = &Server->RuntimeData->Skill;
    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal_skill.new_skill_list");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "skill_main");
    while (Iterator) {
        assert(Runtime->CharacterSkillDataCount < RUNTIME_MEMORY_MAX_CHARACTER_SKILL_DATA_COUNT);
        RTCharacterSkillDataRef SkillData = &Runtime->CharacterSkillData[Runtime->CharacterSkillDataCount];
        memset(SkillData, 0, sizeof(struct _RTCharacterSkillData));

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &SkillData->SkillID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "type", &SkillData->SkillType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "group", &SkillData->SkillGroup)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "multi", &SkillData->Multi)) goto error;

        ArchiveIterator* AttributeIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "attribute");
        if (AttributeIterator) {
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "target", &SkillData->Target)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "max_target", &SkillData->MaxTarget)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "reach", &SkillData->Reach)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "mreach", &SkillData->MReach)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "range", &SkillData->Range)) goto error;
            if (!ParseAttributeInt32(Archive, AttributeIterator->Index, "range_type", &SkillData->RangeType)) goto error;

            ArchiveIterator* CodeIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "attribute");
            if (CodeIterator) {
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "grip_calc", &SkillData->GripCalc)) goto error;
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "anicode", &SkillData->AniCode)) goto error;
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "fxcode_on", &SkillData->FxCodeOn)) goto error;
                if (!ParseAttributeInt32(Archive, CodeIterator->Index, "fxcode_keep", &SkillData->FxCodeKeep)) goto error;
            }

            ArchiveIterator* FrameIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "frame");
            if (FrameIterator) {
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "term", &SkillData->FrameTerm)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "blendlimit", &SkillData->BlendLimit)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "hit_frame", &SkillData->HitFrame)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "firing_frame", &SkillData->FiringFrame)) goto error;
                if (!ParseAttributeInt32(Archive, FrameIterator->Index, "ani_f", &SkillData->AniFrame)) goto error;
            }

            ArchiveIterator* EffectIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "effect");
            if (EffectIterator) {
                if (!ParseAttributeInt32(Archive, EffectIterator->Index, "visual_type", &SkillData->VisualType)) goto error;
            }

            ArchiveIterator* ConditionIterator = ArchiveQueryNodeIteratorFirst(Archive, AttributeIterator->Index, "condition");
            if (ConditionIterator) {
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "twohand", &SkillData->ConditionTwoHand)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "dual", &SkillData->ConditionDual)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "onehand", &SkillData->ConditionOneHand)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "bracer", &SkillData->ConditionBracer)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "onechakram", &SkillData->ConditionOneChakram)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "twochakram", &SkillData->ConditionTwoChakram)) goto error;
                if (!ParseAttributeInt32(Archive, ConditionIterator->Index, "exclusive", &SkillData->ConditionExclusive)) goto error;
            }
        }

        ArchiveIterator* ParamIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, "skill_param");
        if (ParamIterator) {
            if (!ParseAttributeInt32(Archive, ParamIterator->Index, "exp1", &SkillData->SkillExp1)) goto error;
            if (!ParseAttributeInt32(Archive, ParamIterator->Index, "exp2", &SkillData->SkillExp2)) goto error;

            ArchiveIterator* PowerIterator = ArchiveQueryNodeIteratorFirst(Archive, ParamIterator->Index, "power");
            if (PowerIterator) {
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "samp", SkillData->SAmp, 2, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "atk", SkillData->Atk, 3, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "shit", SkillData->SHit, 2, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "spenet", SkillData->SPenet, 2, ',')) goto error;
                if (!ParseAttributeInt32Array(Archive, PowerIterator->Index, "critdmg", SkillData->CritDmg, 2, ',')) goto error;
            }

            ArchiveIterator* CostIterator = ArchiveQueryNodeIteratorFirst(Archive, ParamIterator->Index, "cost");
            if (CostIterator) {
                if (!ParseAttributeInt32(Archive, CostIterator->Index, "cooltime_id", &SkillData->CooltimeID)) goto error;
                if (!ParseAttributeInt32Array(Archive, CostIterator->Index, "mp", SkillData->Mp, 2, ',')) goto error;
                if (!ParseAttributeInt32(Archive, CostIterator->Index, "sp", &SkillData->Sp)) goto error;
            }
        }

        Runtime->CharacterSkillDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "skill_order");
    while (Iterator) {
        Int32 SkillID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &SkillID)) goto error;

        RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillID);
        assert(SkillData);

        assert(SkillData->SkillLevelCount < RUNTIME_MEMORY_MAX_SKILL_LEVEL_DATA_COUNT);

        RTSkillLevelDataRef SkillLevelData = &SkillData->SkillLevels[SkillData->SkillLevelCount];
        SkillLevelData->SkillID = SkillID;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "order_level", &SkillLevelData->Index)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "rank", &SkillLevelData->SkillRank)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "rankpoint", &SkillLevelData->SkillRankPoint)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "start_level", &SkillLevelData->StartLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "end_level", &SkillLevelData->EndLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "skill_point", &SkillLevelData->SkillPoint)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "train_price", &SkillLevelData->TrainPrice)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "untrain_price", &SkillLevelData->UntrainPrice)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "stylemastery", &SkillLevelData->StyleMastery)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "twohand", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dual", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_arrow", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "sword_shield", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_sword", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "chakram", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_gun", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG - 1])) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "magic_spirit", &SkillLevelData->BattleStyles[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM - 1])) goto error;
    
        SkillData->SkillLevelCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ServerLoadDungeonData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
) {
    Archive ArchiveData = { 0 };
    RTRuntimeRef Runtime = Server->Runtime;

    Archive* Archive = &Server->RuntimeData->Dungeon1;

    Int32 ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_quest_dungeon");
    if (ParentIndex < 0) goto error;

    ArchiveIterator* Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        assert(Runtime->DungeonDataCount < RUNTIME_MEMORY_MAX_DUNGEON_COUNT);

        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Runtime->DungeonDataCount];
        memset(DungeonData, 0, sizeof(struct _RTDungeonData));

        Int32 EntryItem[2];
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_item", EntryItem, 2, ':')) {
            memset(&DungeonData->EntryItem, 0, sizeof(RTItem));
        }

        ParseAttributeInt32(Archive, Iterator->Index, "create_type", &DungeonData->CreateType);
        ParseAttributeInt32(Archive, Iterator->Index, "enter_type", &DungeonData->EntryType);
        ParseAttributeInt32(Archive, Iterator->Index, "user_type", &DungeonData->PlayerType);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "user_count", &DungeonData->MaxPlayerCount)) {
            DungeonData->MaxPlayerCount = 1;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "ObjCount", &DungeonData->ObjectCount);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "RemoveItem", &DungeonData->RemoveItem)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_cd_level", &DungeonData->EntryConditionLevel)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_class", DungeonData->EntryConditionClass, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_warp_id", &DungeonData->EntryWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "fail_warp_npc_id", &DungeonData->FailWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp_id", &DungeonData->DeadWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData->DungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "penalty_value", &DungeonData->PenaltyValue)) goto error;

        DungeonData->PatternPartCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "pp_id",
            DungeonData->PatternPartIndices,
            RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT,
            ','
        );

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "reward", DungeonData->Reward, 8, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "success_warp_npc_id", &DungeonData->SuccessWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        Runtime->DungeonDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    // TODO: Load additional pattern part data
    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        assert(Runtime->MissionDungeonPatternPartDataCount < RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT);

        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Runtime->MissionDungeonPatternPartDataCount];
        memset(PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &PatternPartData->ID)) goto error;

        PatternPartData->MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            PatternPartData->MissionMobs,
            sizeof(struct _RTQuestUnitMobData) / sizeof(Int32),
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData->MissionNpcID);

        Runtime->MissionDungeonPatternPartDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Archive = &Server->RuntimeData->Dungeon2;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_mission_dungeon");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        assert(Runtime->DungeonDataCount < RUNTIME_MEMORY_MAX_DUNGEON_COUNT);

        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Runtime->DungeonDataCount];
        memset(DungeonData, 0, sizeof(struct _RTDungeonData));
        
        Int32 EntryItem[2];
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_item", EntryItem, 2, ':')) {
            memset(&DungeonData->EntryItem, 0, sizeof(RTItem));
        }

        ParseAttributeInt32(Archive, Iterator->Index, "create_type", &DungeonData->CreateType);
        ParseAttributeInt32(Archive, Iterator->Index, "enter_type", &DungeonData->EntryType);
        ParseAttributeInt32(Archive, Iterator->Index, "user_type", &DungeonData->PlayerType);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "user_count", &DungeonData->MaxPlayerCount)) {
            DungeonData->MaxPlayerCount = 1;
        }
        
        ParseAttributeInt32(Archive, Iterator->Index, "ObjCount", &DungeonData->ObjectCount);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "RemoveItem", &DungeonData->RemoveItem)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_cd_level", &DungeonData->EntryConditionLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "max_user", &DungeonData->MaxPlayerCount)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_class", DungeonData->EntryConditionClass, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_warp_id", &DungeonData->EntryWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "fail_warp_npc_id", &DungeonData->FailWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp_id", &DungeonData->DeadWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData->DungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "penalty_value", &DungeonData->PenaltyValue)) goto error;
        
        DungeonData->PatternPartCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "pp_id",
            DungeonData->PatternPartIndices,
            RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT,
            ','
        );

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "reward", DungeonData->Reward, 8, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "success_warp_npc_id", &DungeonData->SuccessWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "warpnpc_set", &DungeonData->WarpNpcSetID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useoddcircle_count", &DungeonData->UseOddCircleCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useveradrix_count", &DungeonData->UseVeradrixCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        Runtime->DungeonDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        assert(Runtime->MissionDungeonPatternPartDataCount < RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT);

        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Runtime->MissionDungeonPatternPartDataCount];
        memset(PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &PatternPartData->ID)) goto error;
        
        PatternPartData->MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            PatternPartData->MissionMobs,
            sizeof(struct _RTQuestUnitMobData) / sizeof(Int32),
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData->MissionNpcID);

        Runtime->MissionDungeonPatternPartDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }


    Archive = &Server->RuntimeData->Dungeon3;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "cabal.cabal_mission_dungeon");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "dungeon");
    while (Iterator) {
        assert(Runtime->DungeonDataCount < RUNTIME_MEMORY_MAX_DUNGEON_COUNT);

        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Runtime->DungeonDataCount];
        memset(DungeonData, 0, sizeof(struct _RTDungeonData));

        Int32 EntryItem[2];
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_item", EntryItem, 2, ':')) {
            memset(&DungeonData->EntryItem, 0, sizeof(RTItem));
        }

        ParseAttributeInt32(Archive, Iterator->Index, "create_type", &DungeonData->CreateType);
        ParseAttributeInt32(Archive, Iterator->Index, "enter_type", &DungeonData->EntryType);
        ParseAttributeInt32(Archive, Iterator->Index, "user_type", &DungeonData->PlayerType);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "user_count", &DungeonData->MaxPlayerCount)) {
            DungeonData->MaxPlayerCount = 1;
        }

        ParseAttributeInt32(Archive, Iterator->Index, "ObjCount", &DungeonData->ObjectCount);

        if (!ParseAttributeInt32(Archive, Iterator->Index, "RemoveItem", &DungeonData->RemoveItem)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_cd_level", &DungeonData->EntryConditionLevel)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "max_user", &DungeonData->MaxPlayerCount)) goto error;
        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "enter_cd_class", DungeonData->EntryConditionClass, 2, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "enter_warp_id", &DungeonData->EntryWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "fail_warp_npc_id", &DungeonData->FailWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dead_warp_id", &DungeonData->DeadWarpID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &DungeonData->DungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "penalty_value", &DungeonData->PenaltyValue)) goto error;

        DungeonData->PatternPartCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "pp_id",
            DungeonData->PatternPartIndices,
            RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT,
            ','
        );

        if (!ParseAttributeInt32Array(Archive, Iterator->Index, "reward", DungeonData->Reward, 8, ':')) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "success_warp_npc_id", &DungeonData->SuccessWarpNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "world_id", &DungeonData->WorldID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "next_dungeon_id", &DungeonData->NextDungeonID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "warpnpc_set", &DungeonData->WarpNpcSetID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useoddcircle_count", &DungeonData->UseOddCircleCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "dungeon_type", &DungeonData->DungeonType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "useveradrix_count", &DungeonData->UseVeradrixCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "is_elite", &DungeonData->IsElite)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "elite_dungeon_boost", &DungeonData->EliteDungeonBoost)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "mission_timeout", &DungeonData->MissionTimeout)) goto error;

        Runtime->DungeonDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "pp");
    while (Iterator) {
        assert(Runtime->MissionDungeonPatternPartDataCount < RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT);

        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Runtime->MissionDungeonPatternPartDataCount];
        memset(PatternPartData, 0, sizeof(struct _RTMissionDungeonPatternPartData));

        if (!ParseAttributeInt32(Archive, Iterator->Index, "id", &PatternPartData->ID)) goto error;

        PatternPartData->MissionMobCount = ParseAttributeInt32Array2D(
            Archive,
            Iterator->Index,
            "mission_mobs",
            PatternPartData->MissionMobs,
            sizeof(struct _RTQuestUnitMobData) / sizeof(Int32),
            RUNTIME_MAX_QUEST_COUNTER_COUNT,
            ':',
            ','
        );

        ParseAttributeInt32(Archive, Iterator->Index, "mission_npc", &PatternPartData->MissionNpcID);

        Runtime->MissionDungeonPatternPartDataCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    Char FilePath[MAX_PATH];
    PathCombine(ServerDirectory, "quest_dungeon_trigger_to_event.xml", FilePath);

    Archive = &ArchiveData;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest_dungeon_trigger_to_event");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "trigger");
    while (Iterator) {
        Int32 DungeonID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "QDungeonIdx", &DungeonID)) goto error;

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, DungeonID);
        assert(DungeonData);
        assert(DungeonData->TriggerCount < RUNTIME_MEMORY_DUNGEON_TRIGGER_DATA_COUNT);

        RTDungeonTriggerDataRef TriggerData = &DungeonData->TriggerData[DungeonData->TriggerCount];
        memset(TriggerData, 0, sizeof(struct _RTDungeonTriggerData));        
        TriggerData->DungeonID = DungeonID;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgIdx", &TriggerData->TriggerID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgType", &TriggerData->TriggerType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgNpcIdx", &TriggerData->TriggerNpcID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "EvtActGroupIdx", &TriggerData->ActionGroupID)) goto error;
        
        TriggerData->LiveMobCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "LiveStateMMapIdx",
            TriggerData->LiveMobs,
            RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT,
            ','
        );

        TriggerData->DeadMobCount = ParseAttributeInt32ArrayCounted(
            Archive,
            Iterator->Index,
            "DeadStateMMapIdx",
            TriggerData->DeadMobs,
            RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT,
            ','
        );

        DungeonData->TriggerCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    PathCombine(ServerDirectory, "quest_dungeon_act_group.xml", FilePath);

    Archive = &ArchiveData;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest_dungeon_act_group");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "act");
    while (Iterator) {
        Int32 DungeonID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "QDungeonIdx", &DungeonID)) goto error;

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, DungeonID);
        assert(DungeonData);
        assert(DungeonData->TriggerActionCount < RUNTIME_MEMORY_DUNGEON_TRIGGER_ACTION_DATA_COUNT);
        
        RTDungeonTriggerActionDataRef TriggerActionData = &DungeonData->TriggerActionData[DungeonData->TriggerActionCount];
        memset(TriggerActionData, 0, sizeof(struct _RTDungeonTriggerActionData));
        TriggerActionData->DungeonID = DungeonID;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "EvtActGroupIdx", &TriggerActionData->ActionGroupID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "TgtAction", &TriggerActionData->ActionType)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "TgtMMapIdx", &TriggerActionData->TargetID)) goto error;
        if (!ParseAttributeUInt64(Archive, Iterator->Index, "EvtDelay", &TriggerActionData->Delay)) goto error;

        DungeonData->TriggerActionCount += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    PathCombine(ServerDirectory, "quest_dungeon_mobs.xml", FilePath);

    Archive = &ArchiveData;
    if (!ArchiveLoadFromFile(Archive, FilePath, false)) goto error;

    ParentIndex = ArchiveNodeGetChildByPath(Archive, -1, "quest_dungeon_mobs");
    if (ParentIndex < 0) goto error;

    Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, "mob");
    while (Iterator) {
        Int32 PatternPartID;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PPIdx", &PatternPartID)) goto error;

        RTMissionDungeonPatternPartDataRef PatternPartData = RTRuntimeGetPatternPartByID(Runtime, PatternPartID);
        assert(PatternPartData->MobTable.Count < RUNTIME_MEMORY_MAX_MOB_COUNT);

        RTMobRef Mob = &PatternPartData->MobTable.Mobs[PatternPartData->MobTable.Count];
        Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        if (!ParseAttributeInt32(Archive, Iterator->Index, "MobIdx", &Mob->ID.EntityIndex)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PPIdx", &Mob->Spawn.PatternPartID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpeciesIdx", &Mob->Spawn.MobSpeciesID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgIdxSpawn", &Mob->Spawn.SpawnTriggerID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "TrgIdxKill", &Mob->Spawn.KillTriggerID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PosX", &Mob->Spawn.AreaX)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PosY", &Mob->Spawn.AreaY)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Width", &Mob->Spawn.AreaWidth)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Height", &Mob->Spawn.AreaHeight)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpwnInterval", &Mob->Spawn.SpawnInterval)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpwnCount", &Mob->Spawn.SpawnCount)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "SpawnDefault", &Mob->Spawn.SpawnDefault)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Grade", &Mob->Spawn.Grade)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Lv", &Mob->Spawn.Level)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "MissionGate", &Mob->Spawn.MissionGate)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "PerfectDrop", &Mob->Spawn.PerfectDrop)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Type", &Mob->Spawn.Type)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Min", &Mob->Spawn.Min)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Max", &Mob->Spawn.Max)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Authority", &Mob->Spawn.Authority)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Server_Mob", &Mob->Spawn.ServerMobID)) goto error;
        if (!ParseAttributeInt32(Archive, Iterator->Index, "Loot_Delay", &Mob->Spawn.LootDelay)) goto error;

        Mob->Data = &Runtime->MobData[Mob->Spawn.MobSpeciesID];
        Mob->IsInfiniteSpawn = true;
        Mob->IsPermanentDeath = false;
        Mob->RemainingSpawnCount = 0;

        PatternPartData->MobTable.Count += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

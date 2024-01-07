#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_ATTACK_TYPE_CRITICAL = 1,
    RUNTIME_ATTACK_TYPE_NORMAL,
    RUNTIME_ATTACK_TYPE_MISS,
    RUNTIME_ATTACK_TYPE_ABSORB,
	RUNTIME_ATTACK_TYPE_BLOCK,
};

enum {
	RUNTIME_BATTLE_SKILL_TYPE_SWORD,
	RUNTIME_BATTLE_SKILL_TYPE_MAGIC,
};

enum {
    RUNTIME_ATTRIBUTE_HP_MAX,
	RUNTIME_ATTRIBUTE_HP_CURRENT,
	RUNTIME_ATTRIBUTE_MP_MAX,
    RUNTIME_ATTRIBUTE_MP_CURRENT,
	RUNTIME_ATTRIBUTE_SP_MAX,
    RUNTIME_ATTRIBUTE_SP_CURRENT,
	RUNTIME_ATTRIBUTE_BP_MAX,
    RUNTIME_ATTRIBUTE_BP_CURRENT,
	RUNTIME_ATTRIBUTE_ATTACK,
	RUNTIME_ATTRIBUTE_MAGIC_ATTACK,
	RUNTIME_ATTRIBUTE_ATTACK_RATE,
    RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX,
	RUNTIME_ATTRIBUTE_CRITICAL_RATE,
	RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE,
	RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP,
	RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP,
	RUNTIME_ATTRIBUTE_ACCURACY,
	RUNTIME_ATTRIBUTE_PENETRATION,
	RUNTIME_ATTRIBUTE_ADD_DAMAGE,
	RUNTIME_ATTRIBUTE_MIN_DAMAGE,
	RUNTIME_ATTRIBUTE_IGNORE_EVASION,
	RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASED,
	RUNTIME_ATTRIBUTE_IGNORE_DAMAGE_REDUCTION,
	RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_RATE,
	RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_DAMAGE,
	RUNTIME_ATTRIBUTE_IGNORE_RESIST_SKILL_AMP,
	RUNTIME_ATTRIBUTE_IGNORE_RESIST_DOWN,
	RUNTIME_ATTRIBUTE_IGNORE_RESIST_KNOCK_BACK,
	RUNTIME_ATTRIBUTE_IGNORE_RESIST_STUN,
	RUNTIME_ATTRIBUTE_NORMAL_ATTACK_DAMAGE_UP,
	RUNTIME_ATTRIBUTE_CANCEL_IGNORE_PENETRATION,
	RUNTIME_ATTRIBUTE_DEFENSE,
	RUNTIME_ATTRIBUTE_DEFENSE_RATE,
	RUNTIME_ATTRIBUTE_EVASION,
	RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION,
	RUNTIME_ATTRIBUTE_RESIST_CRITICAL_RATE,
	RUNTIME_ATTRIBUTE_RESIST_CRITICAL_DAMAGE,
	RUNTIME_ATTRIBUTE_RESIST_STUN,
	RUNTIME_ATTRIBUTE_RESIST_DOWN,
	RUNTIME_ATTRIBUTE_RESIST_KNOCK_BACK,
	RUNTIME_ATTRIBUTE_RESIST_UNMOVABLE,
	RUNTIME_ATTRIBUTE_RESIST_SKILL_AMP,
	RUNTIME_ATTRIBUTE_RESIST_SUPPRESSION,
	RUNTIME_ATTRIBUTE_RESIST_SILENCE,
	RUNTIME_ATTRIBUTE_IGNORE_PENETRATION,
	RUNTIME_ATTRIBUTE_IGNORE_ACCURACY,
	RUNTIME_ATTRIBUTE_CANCEL_IGNORE_DAMAGE_REDUCTION,
	RUNTIME_ATTRIBUTE_CANCEL_IGNORE_EVASION,
	RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASED,
	RUNTIME_ATTRIBUTE_ABSORB_RATE_HP,
	RUNTIME_ATTRIBUTE_ABSORB_LIMIT_HP,
	RUNTIME_ATTRIBUTE_AUTO_HEAL_HP,
	RUNTIME_ATTRIBUTE_ABSORB_RATE_MP,
	RUNTIME_ATTRIBUTE_ABSORB_LIMIT_MP,
	RUNTIME_ATTRIBUTE_AUTO_HEAL_MP,
	RUNTIME_ATTRIBUTE_MOVEMENT_SPEED,
	RUNTIME_ATTRIBUTE_INCREASE_BATTLE_MODE_DURATION,
	RUNTIME_ATTRIBUTE_INCREASE_AURA_MODE_DURATION,
	RUNTIME_ATTRIBUTE_BOOST_RESTORATION_HP,
	RUNTIME_ATTRIBUTE_DECREASE_BP_CONSUMPTION,

	RUNTIME_ATTRIBUTE_EXP,

	RUNTIME_ATTRIBUTE_COUNT
};

struct _RTBattleStyleLevelFormulaData {
	Int32 BattleStyleIndex;
	Int32 BaseHP;
	Int32 DeltaHP;
	Int32 BaseMP;
	Int32 DeltaMP;
};

struct _RTBattleStyleClassFormulaData {
	Int32 BattleStyleIndex;
	Int32 Attack[2];
	Int32 MagicAttack[2];
	Int32 Defense[2];
	Int32 AttackRate[2];
	Int32 DefenseRate[2];
	Int32 DamageReduction[2];
	Int32 Accuracy[2];
	Int32 Penetration[2];
	Int32 AccuracyPenalty[2];
	Int32 PenetrationPenalty[2];
};

struct _RTBattleStyleSlopeData {
	Int32 Penalty;
	Int32 Slope;
	Int32 Intercept;
};

struct _RTBattleStyleSlopeFormulaData {
	Int32 SlopeID;
	Int32 SlopeCount;
	struct _RTBattleStyleSlopeData Slopes[RUNTIME_MEMORY_MAX_SLOPE_DATA_COUNT];
};

struct _RTBattleStyleStatsFormulaData {
	Int32 BattleStyleIndex;
	Int32 Attack[3];
	Int32 AttackSlopeID;
	Int32 MagicAttack[3];
	Int32 MagicAttackSlopeID;
	Int32 DamageReduction[3];
	Int32 DamageReductionSlopeID;
	Int32 Evasion[3];
	Int32 EvasionSlopeID;
	Int32 AttackRate[3];
	Int32 AttackRateSlopeID;
	Int32 DefenseRate[3];
	Int32 DefenseRateSlopeID;
	Int32 ResistCriticalRate[3];
	Int32 ResistCriticalRateSlopeID;
	Int32 ResistSkillAmp[3];
	Int32 ResistSkillAmpSlopeID;
	Int32 ResistCriticalDamage[3];
	Int32 ResistCriticalDamageSlopeID;
	Int32 ResistUnmovable[3];
	Int32 ResistUnmovableSlopeID;
	Int32 ResistStun[3];
	Int32 ResistStunSlopeID;
	Int32 ResistDown[3];
	Int32 ResistDownSlopeID;
	Int32 ResistKnockback[3];
	Int32 ResistKnockbackSlopeID;
	Int32 HP[3];
	Int32 HPSlopeID;
	Int32 MP[3];
	Int32 MPSlopeID;
	Int32 IgnorePenetration[3];
	Int32 IgnorePenetrationSlopeID;
};

struct _RTBattleStyleRankData {
	Int32 Level;
	Int32 ConditionSTR;
	Int32 ConditionDEX;
	Int32 ConditionINT;
	Int32 SkillIndex[2];
	Int32 SkillSlot[2];
};

struct _RTBattleStyleSkillRankData {
	Int32 SkillRank;
	Int32 SkillLevelExp;
};

struct _RTBattleStyleSkillFormulaData {
	Int32 BattleStyleIndex;
	Int32 SkillRankCount;
	struct _RTBattleStyleSkillRankData SkillRanks[RUNTIME_MEMORY_MAX_SKILL_RANK_DATA_COUNT];
};

struct _RTBattleStyleRankFormulaData {
	Int32 BattleStyleIndex;
	Int32 RankCount;
	struct _RTBattleStyleRankData Ranks[RUNTIME_MEMORY_MAX_BATTLE_RANK_DATA_COUNT];
};

struct _RTBattleAttributes {
	Int32 Seed;
	Timestamp AttackTimeout;
	Int32 Values[RUNTIME_ATTRIBUTE_COUNT];
};

struct _RTFinalBattleAttributes {
	Int32 Attack;
	Int32 Defense;
	Int32 CriticalDamage;
	Int32 SkillAmp;
	Int32 AddDamage;
	Int32 MinDamage;
	Int32 NormalDamageAmp;
	Int32 FinalDamageAmp;
	Int32 CriticalRate;
	Int32 MissRate;
	Int32 BlockRate;
	Int32 AccumulatedRate;
};

struct _RTBattleResult {
	UInt8 AttackType;
	Bool IsDead;
	Int32 TotalDamage;
	Int32 AppliedDamage;
	Int32 AdditionalDamage;
	Int32 Exp;
	Int32 SkillExp;
};

Void RTCalculateNormalAttackResult(
	RTRuntimeRef Runtime,
	Int32 BattleSkillType,
	Int32 AttackerLevel,
	RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
	RTBattleResultRef Result
);

Void RTCalculateSkillAttackResult(
	RTRuntimeRef Runtime,
	Int32 SkillLevel,
	RTCharacterSkillDataRef Skill,
	Int32 AttackerLevel,
	RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
	RTBattleResultRef Result
);

#pragma pack(pop)

EXTERN_C_END

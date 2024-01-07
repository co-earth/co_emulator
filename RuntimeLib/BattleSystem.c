#include "Runtime.h"
#include "Skill.h"
#include "BattleSystem.h"

// TODO: Revisit all calculation formulas to fit the original title as good as possible

Int32 CalculateLevelDifference(
	Int32 AttackerLevel,
	Int32 DefenderLevel
) {
	return AttackerLevel - DefenderLevel;
}

Int32 CalculateLevelDifferencePenalty(
	Int32 AttackerLevel,
	Int32 DefenderLevel
) {
	return (AttackerLevel - DefenderLevel) * (AttackerLevel + 30) * 1000 / (22 * AttackerLevel + 102);
}

Int32 CalculateHitRate(
	Int32 AttackRate,
	Int32 DefenseRate
) {
	if (AttackRate + DefenseRate < 1) return 100;

	return 50 + 50 * AttackRate / (AttackRate + DefenseRate);
}

static inline Int32 _CalculateFinalAttribute(
	Int32 Value,
	Int32 MaxValue,
	Int32 ResistValue,
	Int32 IgnoreResistValue
) {
	return MAX(0, MIN(Value, MaxValue) - MAX(0, ResistValue - IgnoreResistValue));
}

static inline Int32 _CalculateFinalDefense(Int32 Defense, Int32 Penetration) {
	return Defense * 10000 / (10000 + Penetration);
}

static inline Int32 _CalculateFinalMissRate(Int32 Evasion, Int32 Accuracy) {
	Int32 FinalEvasion = Evasion * 10000 / (10000 + Accuracy);
	
	return 100 - 100 * 10000 / (10000 + FinalEvasion);
}

static inline Int32 _CalculateFinalMinDamage(Int32 MinDamage, Int32 AttackRate, Int32 DefenseRate) {
	Int32 BaseRate = 100 - MinDamage;
	if (DefenseRate < 1) return BaseRate;

	Int32 AppliedAttackRate = MAX(0, AttackRate - DefenseRate);

	return MinDamage + BaseRate - BaseRate * DefenseRate / (DefenseRate + AppliedAttackRate);
}

static inline Int32 _CalculateFinalBlockRate(Int32 AttackRate, Int32 DefenseRate) {
	if (AttackRate < 1) return 0;

	Int32 AppliedDefenseRate = MAX(0, DefenseRate - AttackRate);

	return 100 - 100 * AttackRate / (AttackRate + AppliedDefenseRate);
}

Void CalculateFinalBattleAttributes(
	Int32 BattleSkillType,
	Int32 AttackerLevel,
	RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
	RTFinalBattleAttributesRef Result
) {
	assert(BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD || BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_MAGIC);

	Int32 AttackIndex = (BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_ATTACK : RUNTIME_ATTRIBUTE_MAGIC_ATTACK;
	Int32 SkillAmpIndex = (BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP;

	Result->Attack = Attacker->Values[AttackIndex];
	Result->CriticalRate = _CalculateFinalAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE],
		Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX],
		Defender->Values[RUNTIME_ATTRIBUTE_RESIST_CRITICAL_RATE],
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_RATE]
	);
	Result->CriticalDamage = _CalculateFinalAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE],
		INT32_MAX,
		Defender->Values[RUNTIME_ATTRIBUTE_RESIST_CRITICAL_DAMAGE],
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_DAMAGE]
	);
	Result->SkillAmp = _CalculateFinalAttribute(
		Attacker->Values[SkillAmpIndex],
		INT32_MAX,
		Defender->Values[RUNTIME_ATTRIBUTE_RESIST_SKILL_AMP],
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_RESIST_SKILL_AMP]
	);
	Result->Defense = _CalculateFinalDefense(
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE],
		_CalculateFinalAttribute(
			Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION],
			INT32_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_IGNORE_PENETRATION],
			Attacker->Values[RUNTIME_ATTRIBUTE_CANCEL_IGNORE_PENETRATION]
		)
	);
	Result->MissRate = _CalculateFinalMissRate(
		_CalculateFinalAttribute(
			Defender->Values[RUNTIME_ATTRIBUTE_EVASION],
			INT32_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_EVASION],
			Defender->Values[RUNTIME_ATTRIBUTE_CANCEL_IGNORE_EVASION]
		),
		_CalculateFinalAttribute(
			Attacker->Values[RUNTIME_ATTRIBUTE_ACCURACY],
			INT32_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_IGNORE_ACCURACY],
			0
		)
	);
	Result->BlockRate = _CalculateFinalBlockRate(
		Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE],
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE]
	);
	Result->AddDamage = Attacker->Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE];
	Result->AddDamage -= _CalculateFinalAttribute(
		Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION],
		INT32_MAX,
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_DAMAGE_REDUCTION],
		Defender->Values[RUNTIME_ATTRIBUTE_CANCEL_IGNORE_DAMAGE_REDUCTION]
	);
	Result->MinDamage = _CalculateFinalMinDamage(
		Attacker->Values[RUNTIME_ATTRIBUTE_MIN_DAMAGE],
		Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE],
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE]
	);

	if (BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_MAGIC) {
		Result->MinDamage = 100;
	}

	Result->NormalDamageAmp = Attacker->Values[RUNTIME_ATTRIBUTE_NORMAL_ATTACK_DAMAGE_UP];
	Result->FinalDamageAmp = Attacker->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASED];
	Result->FinalDamageAmp -= Defender->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASED];
	Result->AccumulatedRate = 100 + Result->MissRate + Result->BlockRate;
}

Void RTCalculateNormalAttackResult(
    RTRuntimeRef Runtime,
	Int32 BattleSkillType,
	Int32 AttackerLevel,
    RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
    RTBattleResultRef Result
) {
	assert(BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD || BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_MAGIC);

	struct _RTFinalBattleAttributes Attributes = { 0 };
	CalculateFinalBattleAttributes(
		BattleSkillType,
		AttackerLevel,
		Attacker,
		DefenderLevel,
		Defender,
		&Attributes
	);

//	Int32 LevelDifference = CalculateLevelDifference(AttackerLevel, DefenderLevel);
//	Int32 LevelDifferencePenalty = CalculateLevelDifferencePenalty(AttackerLevel, DefenderLevel);

	Int32 Rate = RandomRange(&Attacker->Seed, 0, Attributes.AccumulatedRate);
	if (Rate < Attributes.CriticalRate) {
		Attributes.MinDamage = 100;

		Result->AttackType = RUNTIME_ATTACK_TYPE_CRITICAL;
	}
	else if (Rate < Attributes.CriticalRate + Attributes.MissRate) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_MISS;
		return;
	}
	else if (Rate < Attributes.CriticalRate + Attributes.MissRate + Attributes.BlockRate) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_BLOCK;
		return;
	}
	else {
		Result->AttackType = RUNTIME_ATTACK_TYPE_NORMAL;
	}

	Result->TotalDamage = MAX(1, Attributes.Attack - Attributes.Defense);
	Int32 DamageRate = RandomRange(&Attacker->Seed, Attributes.MinDamage, 100);
	Result->TotalDamage = (Result->TotalDamage * DamageRate) / 100;
	Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.SkillAmp)) / 100;

	if (Result->AttackType = RUNTIME_ATTACK_TYPE_CRITICAL) {
		Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.CriticalDamage)) / 100;
	}

	if (Result->AttackType = RUNTIME_ATTACK_TYPE_NORMAL) {
		Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.NormalDamageAmp)) / 100;
	}

	Result->AdditionalDamage = Attributes.AddDamage;
	Result->TotalDamage += Attributes.AddDamage;
	Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.FinalDamageAmp)) / 100;
	Result->AppliedDamage = MIN(Result->TotalDamage, Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	Result->IsDead = (Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT] - Result->TotalDamage) <= 0;

	// TODO: Add damage absorb shield and other stuff

	Result->Exp = (Result->AppliedDamage * Defender->Values[RUNTIME_ATTRIBUTE_EXP]) / MAX(1, Defender->Values[RUNTIME_ATTRIBUTE_HP_MAX]) * Runtime->Config.ExpMultiplier;
}

Void RTCalculateSkillAttackResult(
	RTRuntimeRef Runtime,
	Int32 SkillLevel,
	RTCharacterSkillDataRef Skill,
	Int32 AttackerLevel,
	RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
	RTBattleResultRef Result
) {
	BOOL IsDefenderCharacter = false; // TODO: Add entity type to battle attributes

	// TODO: Add skill data to base attributes

	Int32 BattleSkillType = (Skill->SkillType == RUNTIME_SKILL_TYPE_MAGIC) ? RUNTIME_BATTLE_SKILL_TYPE_MAGIC : RUNTIME_BATTLE_SKILL_TYPE_SWORD;
	RTCalculateNormalAttackResult(
		Runtime,
		BattleSkillType,
		AttackerLevel,
		Attacker,
		DefenderLevel,
		Defender,
		Result
	);

	if (Result->AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result->SkillExp = Skill->SkillExp1 * Runtime->Config.SkillExpMultiplier;
	}
	else {
		Result->SkillExp = Skill->SkillExp2 * Runtime->Config.SkillExpMultiplier;
	}
}
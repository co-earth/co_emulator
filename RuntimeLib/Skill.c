#include "Skill.h"
#include "Mob.h"

Int32 RTCharacterSkillDataGetSkillType(
	RTCharacterSkillDataRef Skill,
	Bool IsPassiveSword,
	Bool IsPassiveMagic
) {
	if (IsPassiveSword && !IsPassiveMagic) return RUNTIME_SKILL_TYPE_SWORD;
	if (!IsPassiveSword && IsPassiveMagic) return RUNTIME_SKILL_TYPE_MAGIC;

	if (IsPassiveSword && IsPassiveMagic) {
		if (Skill->SkillType == RUNTIME_SKILL_TYPE_SWORD) return RUNTIME_SKILL_TYPE_MAGIC;
		if (Skill->SkillType == RUNTIME_SKILL_TYPE_MAGIC) return RUNTIME_SKILL_TYPE_SWORD;
	}

	return Skill->SkillType;
}

Int32 RTCharacterSkillDataGetSkillExp(
	RTCharacterSkillDataRef Skill,
	Int32 SkillResult
) {
	if (SkillResult == RUNTIME_SKILL_RESULT_CRITICAL) return Skill->SkillExp2;
	if (SkillResult == RUNTIME_SKILL_RESULT_NORMAL) return Skill->SkillExp1;

	return 0;
}

Int32 RTCalculateDistance(
	Int32 X1,
	Int32 Y1,
	Int32 X2,
	Int32 Y2
) {
	Int32 DeltaX = ABS(X2 - X1);
	Int32 DeltaY = ABS(Y2 - Y1);

	return (DeltaX >= DeltaY) ? DeltaX : DeltaY;
}

// TODO: The Radius is a float so we should calculate the distance in real world coords
Bool RTCheckSkillTargetDistance(
	RTCharacterSkillDataRef Skill,
	Float32 Radius,
	Int32 Scale,
	Int32 SourceX,
	Int32 SourceY,
	Int32 TargetX,
	Int32 TargetY
) {
	Int32 Permutations[9][2] = {
		{SourceX, SourceY},
		{SourceX + Radius, SourceY},
		{SourceX - Radius, SourceY},
		{SourceX, SourceY + Radius},
		{SourceX, SourceY - Radius},
		{SourceX + Radius, SourceY - Radius},
		{SourceX - Radius, SourceY + Radius},
		{SourceX + Radius, SourceY + Radius},
		{SourceX - Radius, SourceY - Radius}
	};

	Int32 ErrorTolerance = 2;
	Int32 MaxDistance = (Skill->RangeType == RUNTIME_SKILL_RANGE_TYPE_RECT) ? Skill->Range - 1 : Skill->Reach;
	MaxDistance += ErrorTolerance;

	for (Int32 Index = 0; Index < 9; Index++) {
		Int32 Distance = RTCalculateDistance(TargetX, TargetY, Permutations[Index][0], Permutations[Index][1]);
		if (Distance <= MaxDistance) return true;
	}

	return false;
}

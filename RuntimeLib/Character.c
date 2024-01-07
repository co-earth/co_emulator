#include "Character.h"
#include "Inventory.h"
#include "Runtime.h"

Void RTCharacterInitialize(
	RTRuntimeRef Runtime, 
	RTCharacterRef Character,
	RTCharacterInfoRef Info,
	RTCharacterEquipmentInfoRef EquipmentInfo,
	RTCharacterInventoryInfoRef InventoryInfo,
	RTCharacterSkillSlotInfoRef SkillSlotInfo,
	RTCharacterQuickSlotInfoRef QuickSlotInfo,
	RTCharacterQuestSlotInfoRef QuestSlotInfo,
	RTCharacterQuestFlagInfoRef QuestFlagInfo
) {
	Character->SyncMask = 0;
	Character->SyncPriority = 0;
	Character->SyncTimestamp = GetTickCount64();

	memcpy(&Character->Info, Info, sizeof(struct _RTCharacterInfo));
	memcpy(&Character->EquipmentInfo, EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
	memcpy(&Character->InventoryInfo, InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Character->SkillSlotInfo, SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
	memcpy(&Character->QuickSlotInfo, QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
	memcpy(&Character->QuestSlotInfo, QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
	memcpy(&Character->QuestFlagInfo, QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
	memset(&Character->TemporaryInventoryInfo, 0, sizeof(struct _RTCharacterInventoryInfo));
	memset(&Character->RecoveryInfo, 0, sizeof(struct _RTCharacterRecoveryInfo));

	RTCharacterInitializeAttributes(Runtime, Character);
	RTMovementInitialize(
		Runtime,
		&Character->Movement,
		Info->Position.X,
		Info->Position.Y,
		RUNTIME_MOVEMENT_SPEED_BASE,
		RUNTIME_WORLD_TILE_WALL
	);
}

Void RTCharacterInitializeAttributes(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	memset(Character->Attributes.Values, 0, sizeof(Character->Attributes.Values));

	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);

	// Battle Style Level Formula 

	RTBattleStyleLevelFormulaDataRef LevelFormula = RTRuntimeGetBattleStyleLevelFormulaData(Runtime, BattleStyleIndex);
	
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = LevelFormula->BaseHP + LevelFormula->DeltaHP * (Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level - 1) / 10;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX] = LevelFormula->BaseMP + LevelFormula->DeltaMP * (Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level - 1) / 10;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX] = 0;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX] = 0;

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MAX(0,
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] -
		Character->Info.Resource.DiffHP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = MAX(0,
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX] -
		Character->Info.Resource.DiffMP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] = MAX(0,
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX] -
		Character->Info.Resource.DiffSP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT] = MAX(0,
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX] -
		Character->Info.Resource.DiffBP
	);

	// Battle Style Class Formula 

	RTBattleStyleClassFormulaDataRef ClassFormula = RTRuntimeGetBattleStyleClassFormulaData(Runtime, BattleStyleIndex);

	struct { Int32 AttributeIndex; Int32* Values; } ClassFormulaIndices[] = {
		{ RUNTIME_ATTRIBUTE_ATTACK, ClassFormula->Attack },
		{ RUNTIME_ATTRIBUTE_MAGIC_ATTACK, ClassFormula->MagicAttack },
		{ RUNTIME_ATTRIBUTE_DEFENSE, ClassFormula->Defense },
		{ RUNTIME_ATTRIBUTE_ATTACK_RATE, ClassFormula->AttackRate },
		{ RUNTIME_ATTRIBUTE_DEFENSE_RATE, ClassFormula->DefenseRate },
		{ RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION, ClassFormula->DamageReduction },
		{ RUNTIME_ATTRIBUTE_ACCURACY, ClassFormula->Accuracy },
		{ RUNTIME_ATTRIBUTE_PENETRATION, ClassFormula->Penetration },
		// TODO: Verify if class_hitpenel, class_penetpenel are really ment to be used as ignore or as character self penalty
		{ RUNTIME_ATTRIBUTE_IGNORE_ACCURACY, ClassFormula->AccuracyPenalty },
		{ RUNTIME_ATTRIBUTE_IGNORE_PENETRATION, ClassFormula->PenetrationPenalty },
	};

	Int32 ClassFormulaIndexCount = sizeof(ClassFormulaIndices) / sizeof(ClassFormulaIndices[0]);
	for (Int32 Index = 0; Index < ClassFormulaIndexCount; Index++) {
		Character->Attributes.Values[ClassFormulaIndices[Index].AttributeIndex] += (
			Character->Info.Style.BattleRank * ClassFormulaIndices[Index].Values[0] + ClassFormulaIndices[Index].Values[1]
		);
	}

	// Battle Style Stats Formula 

	RTBattleStyleStatsFormulaDataRef StatsFormula = RTRuntimeGetBattleStyleStatsFormulaData(Runtime, BattleStyleIndex);

	struct { Int32 AttributeIndex; Int32* Values; Int32 SlopeID; } StatsFormulaIndices[] = {
		{ RUNTIME_ATTRIBUTE_ATTACK, StatsFormula->Attack, StatsFormula->AttackSlopeID },
		{ RUNTIME_ATTRIBUTE_MAGIC_ATTACK, StatsFormula->MagicAttack, StatsFormula->MagicAttackSlopeID },
		{ RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION, StatsFormula->DamageReduction, StatsFormula->DamageReductionSlopeID },
		{ RUNTIME_ATTRIBUTE_EVASION, StatsFormula->Evasion, StatsFormula->EvasionSlopeID },
		{ RUNTIME_ATTRIBUTE_ATTACK_RATE, StatsFormula->AttackRate, StatsFormula->AttackRateSlopeID },
		{ RUNTIME_ATTRIBUTE_DEFENSE_RATE, StatsFormula->DefenseRate, StatsFormula->DefenseRateSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_CRITICAL_RATE, StatsFormula->ResistCriticalRate, StatsFormula->ResistCriticalRateSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_SKILL_AMP, StatsFormula->ResistSkillAmp, StatsFormula->ResistSkillAmpSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_CRITICAL_DAMAGE, StatsFormula->ResistCriticalDamage, StatsFormula->ResistCriticalDamageSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_UNMOVABLE, StatsFormula->ResistUnmovable, StatsFormula->ResistUnmovableSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_STUN, StatsFormula->ResistStun, StatsFormula->ResistStunSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_DOWN, StatsFormula->ResistDown, StatsFormula->ResistDownSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_KNOCK_BACK, StatsFormula->ResistKnockback, StatsFormula->ResistKnockbackSlopeID },
		{ RUNTIME_ATTRIBUTE_HP_MAX, StatsFormula->HP, StatsFormula->HPSlopeID },
		{ RUNTIME_ATTRIBUTE_MP_MAX, StatsFormula->MP, StatsFormula->MPSlopeID },
		{ RUNTIME_ATTRIBUTE_IGNORE_PENETRATION, StatsFormula->IgnorePenetration, StatsFormula->IgnorePenetrationSlopeID },
	};

	Int32 StatsFormulaIndexCount = sizeof(StatsFormulaIndices) / sizeof(StatsFormulaIndices[0]);
	Int32 StatsIndices[] = {
		RUNTIME_CHARACTER_STAT_STR,
		RUNTIME_CHARACTER_STAT_DEX,
		RUNTIME_CHARACTER_STAT_INT,
	};
	Int32 StatsIndexCount = sizeof(StatsIndices) / sizeof(StatsIndices[0]);
	for (Int32 Index = 0; Index < StatsFormulaIndexCount; Index++) {
		Int32 AttributeValue = 0;

		for (Int32 StatsIndex = 0; StatsIndex < StatsIndexCount; StatsIndex++) {
			if (StatsFormulaIndices[Index].Values[StatsIndex] > 0) {
				RTBattleStyleSlopeDataRef SlopeData = RTRuntimeGetBattleStyleSlopeData(
					Runtime,
					StatsFormulaIndices[Index].SlopeID,
					Character->Info.Stat[StatsIndex]
				);

				AttributeValue += Character->Info.Stat[StatsIndex] * SlopeData->Slope + SlopeData->Intercept;
			}
		}

		Character->Attributes.Values[StatsFormulaIndices[Index].AttributeIndex] += AttributeValue / 1000;
	}

	// TODO: Skill Rank Formula
}

Bool RTCharacterIsAlive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
    return Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] > 0;
}

Bool RTCharacterIsUnmovable(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return false;
}

// TODO: Trace movement collision on RTWorld tiles!
Bool RTCharacterMovementBegin(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 WorldID,
	Int32 PositionBeginX,
	Int32 PositionBeginY,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY,
	Int32 PositionEndX,
	Int32 PositionEndY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (Character->Info.Position.WorldID != WorldID) return false;

	Int32 DeltaStartX = Character->Movement.PositionBegin.X - PositionBeginX;
	Int32 DeltaStartY = Character->Movement.PositionBegin.Y - PositionBeginY;
	Int32 DeltaCurrentX = Character->Movement.PositionCurrent.X - PositionBeginX;
	Int32 DeltaCurrentY = Character->Movement.PositionCurrent.Y - PositionBeginY;
	Int32 PositionChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 PositionChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;

	if ((Character->Movement.IsMoving) ||
		DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaCurrentX * DeltaCurrentX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaCurrentY * DeltaCurrentY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Int32 ChunkDeltaX = (PositionCurrentX >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkX;
	Int32 ChunkDeltaY = (PositionCurrentY >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkY;

	if (ChunkDeltaX < -1 || ChunkDeltaX > 1 ||
		ChunkDeltaY < -1 || ChunkDeltaY > 1) {
		return false;
	}

	// TODO: Check if PositionCurrent means the position the character stops after collision tracement

	Character->Movement.PositionBegin.X -= DeltaStartX;
	Character->Movement.PositionBegin.Y -= DeltaStartY;
	Character->Movement.PositionCurrent.X -= DeltaCurrentX;
	Character->Movement.PositionCurrent.Y -= DeltaCurrentY;
	Character->Movement.PositionEnd.X = PositionEndX;
	Character->Movement.PositionEnd.Y = PositionEndY;
	Character->Movement.Waypoints[0].X = PositionBeginX;
	Character->Movement.Waypoints[0].Y = PositionBeginY;
	Character->Movement.Waypoints[1].X = PositionCurrentX;
	Character->Movement.Waypoints[1].Y = PositionCurrentY;
	Character->Movement.WaypointCount = 2;

	RTMovementStartDeadReckoning(Runtime, &Character->Movement);

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterMovementChange(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 WorldID,
	Int32 PositionBeginX,
	Int32 PositionBeginY,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY,
	Int32 PositionEndX,
	Int32 PositionEndY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (!(Character->Movement.IsMoving)) return false;

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];

	if (PositionBeginX < 0 || PositionBeginX >= RUNTIME_WORLD_SIZE ||
		PositionBeginY < 0 || PositionBeginY >= RUNTIME_WORLD_SIZE ||
		PositionEndX < 0 || PositionEndX >= RUNTIME_WORLD_SIZE ||
		PositionEndY < 0 || PositionEndY >= RUNTIME_WORLD_SIZE ||
		PositionCurrentX < 0 || PositionCurrentX >= RUNTIME_WORLD_SIZE ||
		PositionCurrentY < 0 || PositionCurrentY >= RUNTIME_WORLD_SIZE) {
		return false;
	}

	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointA->Y;
	Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
	Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;

	Int32 DeltaStartX, DeltaStartY;
	if (AbsDeltaX >= AbsDeltaY) {
		if (AbsDeltaX == 0) {
			AbsDeltaX = 1;
		}

		Int32 OffsetX = (DeltaX > 0) ? (PositionBeginX - WaypointA->X) : (WaypointA->X - PositionBeginX);
		Int32 OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

		if (DeltaY < 0)
			OffsetY = -OffsetY;

		DeltaStartX = WaypointA->X + OffsetX * DeltaX / AbsDeltaX - PositionBeginX;
		DeltaStartY = WaypointA->Y + OffsetY - PositionBeginY;
	}
	else {
		Int32 OffsetY = (DeltaY > 0) ? (PositionBeginY - WaypointA->Y) : (WaypointA->Y - PositionBeginY);
		Int32 OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

		if (DeltaX < 0)
			OffsetX = -OffsetX;

		DeltaStartX = WaypointA->X + OffsetX - PositionBeginX;
		DeltaStartY = WaypointA->Y + OffsetY * DeltaY / AbsDeltaY - PositionBeginY;
	}

	if (DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Int32 PositionChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 PositionChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 DeltaChunkX = (PositionCurrentX >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkX;
	Int32 DeltaChunkY = (PositionCurrentY >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkY;

	if (DeltaChunkX < -1 || DeltaChunkX > 1 ||
		DeltaChunkY < -1 || DeltaChunkY > 1) {
		return false;
	}

	WaypointA->X = PositionBeginX;
	WaypointA->Y = PositionBeginY;
	WaypointB->X = PositionCurrentX;
	WaypointB->Y = PositionCurrentY;
	Character->Movement.WaypointCount = 2;
	Character->Movement.PositionBegin.X = PositionBeginX;
	Character->Movement.PositionBegin.Y = PositionBeginY;
	Character->Movement.PositionCurrent.X = PositionBeginX;
	Character->Movement.PositionCurrent.Y = PositionBeginY;
	Character->Movement.PositionEnd.X = PositionEndX;
	Character->Movement.PositionEnd.Y = PositionEndY;

	RTMovementStartDeadReckoning(Runtime, &Character->Movement);

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterMovementEnd(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PositionX,
	Int32 PositionY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (!(Character->Movement.IsMoving)) return false;

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];
	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointB->Y;
	Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
	Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;

	Int32 DeltaStartX, DeltaStartY;
	if (AbsDeltaX >= AbsDeltaY) {
		if (AbsDeltaX == 0) {
			AbsDeltaX = 1;
		}

		Int32 OffsetX = (DeltaX > 0) ? (PositionX - WaypointA->X) : (WaypointA->X - PositionX);
		Int32 OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

		if (DeltaY < 0) {
			OffsetY = -OffsetY;
		}

		DeltaStartX = WaypointA->X + OffsetX * DeltaX / AbsDeltaX - PositionX;
		DeltaStartY = WaypointA->Y + OffsetY - PositionY;
	}
	else {
		Int32 OffsetY = (DeltaY > 0) ? (PositionY - WaypointA->Y) : (WaypointA->Y - PositionY);
		Int32 OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

		if (DeltaX < 0) {
			OffsetX = -OffsetX;
		}

		DeltaStartX = WaypointA->X + OffsetX - PositionX;
		DeltaStartY = WaypointA->Y + OffsetY * DeltaY / AbsDeltaY - PositionY;
	}

	if (DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Character->Movement.PositionBegin.X = PositionX;
	Character->Movement.PositionBegin.Y = PositionY;
	Character->Movement.PositionCurrent.X = PositionX;
	Character->Movement.PositionCurrent.Y = PositionY;
	Character->Movement.PositionEnd.X = PositionX;
	Character->Movement.PositionEnd.Y = PositionY;

	RTMovementEndDeadReckoning(Runtime, &Character->Movement);

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterMovementChangeWaypoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY,
	Int32 PositionNextX,
	Int32 PositionNextY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (!(Character->Movement.IsMoving)) return false;

	// TODO: Check if given position current is matching real current position...

	Int32 DeltaX = PositionNextX - PositionCurrentX;
	Int32 DeltaY = PositionNextY - PositionCurrentY;
	Int32 AbsDeltaX = (DeltaX >= 0) ? DeltaX : -DeltaX;
	Int32 AbsDeltaY = (DeltaY >= 0) ? DeltaY : -DeltaY;

	if (AbsDeltaX < 0 || AbsDeltaX >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH ||
		AbsDeltaY < 0 || AbsDeltaY >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
		return false;
	}

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];

	WaypointA->X = PositionCurrentX;
	WaypointA->Y = PositionCurrentY;
	WaypointB->X = PositionNextX;
	WaypointB->Y = PositionNextY;
	Character->Movement.WaypointCount = 2;

	RTMovementRestartDeadReckoning(Runtime, &Character->Movement);

	Character->Movement.PositionCurrent.X = PositionCurrentX;
	Character->Movement.PositionCurrent.Y = PositionCurrentY;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterMovementChangePosition(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY
) {
	if (!RTCharacterIsAlive(Runtime, Character))
		return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) 
		return false;
	if (!(Character->Movement.IsMoving)) 
		return false;

	if (PositionCurrentX >= RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		PositionCurrentY >= RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];
	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointB->Y;
	Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
	Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;
	Int32 DeltaStartX = 0;
	Int32 DeltaStartY = 0;

	if (AbsDeltaX >= AbsDeltaY) {
		if (!AbsDeltaX) { 
			AbsDeltaX = 1;
		}

		Int32 OffsetX = (DeltaX > 0) ? (PositionCurrentX - WaypointA->X) : (WaypointA->X - PositionCurrentX);
		Int32 OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

		if (DeltaY < 0) {
			OffsetY = -OffsetY;
		}

		DeltaStartX = WaypointA->X + OffsetX * DeltaX / AbsDeltaX - PositionCurrentX;
		DeltaStartY = WaypointA->Y + OffsetY - PositionCurrentY;
	} 
	else {
		Int32 OffsetY = (DeltaY > 0) ? (PositionCurrentY - WaypointA->Y) : (WaypointA->Y - PositionCurrentY);
		Int32 OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

		if (DeltaX < 0) {
			OffsetX = -OffsetX;
		}

		DeltaStartX = WaypointA->X + OffsetX - PositionCurrentX;
		DeltaStartY = WaypointA->Y + OffsetY * DeltaY / AbsDeltaY - PositionCurrentY;
	}

	if (DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Int32 PositionChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 PositionChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 DeltaChunkX = (PositionCurrentX >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkX;
	Int32 DeltaChunkY = (PositionCurrentY >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkY;

	if (DeltaChunkX == 0 && DeltaChunkY == 0) {
		return true;
	}

	if (DeltaChunkX < -1 || DeltaChunkX > 1 || DeltaChunkY < -1 || DeltaChunkY > 1) {
		return false;
	}

	Character->Movement.PositionCurrent.X = PositionCurrentX;
	Character->Movement.PositionCurrent.Y = PositionCurrentY;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	RTRuntimeBroadcastEvent(
		Runtime,
		RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE,
		World,
		kEntityNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	return true;
}

Void RTCharacterQuestFlagClear(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);

	Character->QuestFlagInfo.Flags[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] &= ~(1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Void RTCharacterQuestFlagSet(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);

	Character->QuestFlagInfo.Flags[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] |= (1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE));

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTFLAG;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Bool RTCharacterQuestFlagIsSet(
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	assert(0 <= QuestIndex && QuestIndex < RUNTIME_CHARACTER_QUEST_FLAG_SIZE * RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);

	return (Character->QuestFlagInfo.Flags[QuestIndex / RUNTIME_CHARACTER_QUEST_FLAG_SIZE] & (1 << (QuestIndex % RUNTIME_CHARACTER_QUEST_FLAG_SIZE))) > 0;
}

Bool RTCharacterQuestBegin(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 SlotIndex
) {
 	if (SlotIndex < 0 || SlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;
	if (RTCharacterQuestFlagIsSet(Character, QuestIndex)) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
	if (Character->QuestSlotInfo.Count > SlotIndex && QuestSlot->QuestIndex > 0) return false;

	// TODO: Check if quest is already in progress!
	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;

	if (Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level < Quest->Condition.Level ||
		Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level > Quest->Condition.MaxLevel ||
		Character->Info.Honor.Rank < Quest->Condition.MinHonorRank ||
		Character->Info.Honor.Rank > Quest->Condition.MaxHonorRank ||
		Character->Info.Level[RUNTIME_CHARACTER_LEVEL_OVERLORD].Level < Quest->Condition.MinOverlordLevel ||
		Character->Info.Level[RUNTIME_CHARACTER_LEVEL_OVERLORD].Level > Quest->Condition.MaxOverlordLevel) {
		return false;
	}

	if (Quest->Condition.RelatedQuestID >= 0 && !RTCharacterQuestFlagIsSet(Character, Quest->Condition.RelatedQuestID)) {
		return false;
	}

	if (Quest->QuestBeginNpcIndex >= 0) {
		assert(Quest->QuestBeginNpcIndex < Quest->NpcSet.Count);
		RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[Quest->QuestBeginNpcIndex];
		RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
		if (!Npc) return false;

		if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
			return false;
		}

		QuestSlot->NpcActionIndex = QuestNpc->NpcActionOrder;

		if (Quest->QuestBeginNpcIndex + 1 < Quest->NpcSet.Count) {
			RTQuestNpcDataRef NextQuestNpc = &Quest->NpcSet.Npcs[Quest->QuestBeginNpcIndex + 1];
			QuestSlot->NpcActionIndex = NextQuestNpc->NpcActionOrder;
		}
	}
    
	/* TODO: Check conditions for following fields...
	Int32 Class[2];
	Int32 Skill[2];
	Int32 Item[3];
	Int32 Rank;
	Int32 RankType;
	Int32 NationType;
	*/

	QuestSlot->QuestIndex = Quest->ID;
	memset(QuestSlot->Counter, 0, sizeof(UInt8) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

	Character->QuestSlotInfo.Count += 1;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterQuestClear(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 SlotIndex,
	Int32 ItemIndex,
	Int32 InventorySlotIndex,
	UInt64* ResultExp,
	UInt32* ResultSkillExp
) {
	if (SlotIndex < 0 || SlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;
	if (QuestSlot->NpcActionIndex < 0 || QuestSlot->NpcActionIndex >= Quest->NpcSet.Count) return false;

	RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[QuestSlot->NpcActionIndex];
	RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
	if (!Npc) return false;

	if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
		return false;
	}

	if (QuestNpc->ActionType != 6) {
		return false;
	}

	Int32 CounterIndex = 0;
	for (Int32 Index = 0; Index < Quest->MissionMobCount; Index += 1) {
		if (QuestSlot->Counter[CounterIndex] < Quest->MissionMobs[Index].Value[1]) return false;

		CounterIndex += 1;
	}

	// TODO: Delete quest item from inventory!!!
	// TODO: Add item counter check!!!
	for (Int32 Index = 0; Index < Quest->MissionItemCount; Index += 1) {
		RTQuestMissionDataRef MissionData = &Quest->MissionItems[Index];

	}

	// TODO: Add dungeon counter check!!!
	for (Int32 Index = 0; Index < Quest->MissionDungeonCount; Index++) {

	}

	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);

	if (Quest->Type == RUNTIME_QUEST_TYPE_BATTLE_RANK_UP) {
		if (!RTCharacterBattleRankUp(Runtime, Character)) return false;
	}

	*ResultExp = Quest->Reward[0];
	*ResultSkillExp = Quest->Reward[5];

	Int32 RewardItemSetID = Quest->Reward[3];
	RTQuestRewardItemDataRef RewardItem = RTRuntimeGetQuestRewardItemByIndex(
		Runtime,
		RewardItemSetID,
		ItemIndex,
		BattleStyleIndex
	);
	if (RewardItem) {
		struct _RTItemSlot ItemSlot = { 0 };
		ItemSlot.SlotIndex = InventorySlotIndex;
		ItemSlot.Item.ID = RewardItem->ItemID[0];
		ItemSlot.ItemOptions = RewardItem->ItemID[1];
		ItemSlot.ItemDuration.DurationIndex = RewardItem->ItemDuration;

		if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, &ItemSlot)) {
			return false;
		}

		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
	}

	RTCharacterAddExp(Runtime, Character, Quest->Reward[0]);
	RTCharacterAddSkillExp(Runtime, Character, Quest->Reward[5]);
	RTCharacterAddHonorPoint(Runtime, Character, Quest->Reward[4]);
	RTCharacterQuestFlagSet(Character, QuestIndex);

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	RTRuntimeBroadcastEvent(
		Runtime,
		RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS,
		World,
		kEntityNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	Int32 TailLength = Character->QuestSlotInfo.Count - SlotIndex - 1;
	if (TailLength > 0) {
		memmove(
			&Character->QuestSlotInfo.QuestSlot[SlotIndex],
			&Character->QuestSlotInfo.QuestSlot[SlotIndex + 1],
			sizeof(struct _RTQuestSlot) * TailLength
		);
	}
	else {
		memset(
			&Character->QuestSlotInfo.QuestSlot[SlotIndex],
			0,
			sizeof(struct _RTQuestSlot)
		);
	}

	Character->QuestSlotInfo.Count -= 1;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterQuestCancel(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 SlotIndex
) {
	if (SlotIndex < 0 || SlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;

	Int32 TailLength = Character->QuestSlotInfo.Count - SlotIndex;
	if (TailLength > 0) {
		memmove(
			&Character->QuestSlotInfo.QuestSlot[SlotIndex],
			&Character->QuestSlotInfo.QuestSlot[SlotIndex + 1],
			sizeof(struct _RTQuestSlot) * TailLength
		);
	}
	else {
		memset(QuestSlot, 0, sizeof(struct _RTQuestSlot));
	}

	Character->QuestSlotInfo.Count -= 1;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	// TODO: Reset dungeon state!
	// TODO: Reset added and removed items state!
	return true;
}

Bool RTCharacterQuestAction(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex,
	Int32 NpcSetIndex,
	Int32 SlotIndex,
	Int32 ActionIndex,
	Int32 ActionSlotIndex
) {
	if (SlotIndex < 0 || SlotIndex >= RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT) return false;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
	if (QuestSlot->QuestIndex != QuestIndex) return false;
	if (QuestSlot->NpcActionIndex != ActionIndex) return false;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
	if (!Quest) return false;

	if (Quest->NpcSet.ID != NpcSetIndex) return false;
	if (ActionIndex < 0 || ActionIndex >= Quest->NpcSet.Count) return false;

	RTQuestNpcDataRef QuestNpc = &Quest->NpcSet.Npcs[ActionIndex];
	RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, QuestNpc->WorldID, QuestNpc->NpcID);
	if (!Npc) return false;

	// TODO: Add npc action logic!
	if (QuestNpc->ActionType == RUNTIME_QUEST_ACTION_TYPE_TRIGGER) {
		RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
		if (World->Type != RUNTIME_WORLD_TYPE_DUNGEON && World->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return false;

		RTDungeonTriggerEvent(Runtime, World, QuestNpc->Value);
	}

	/* TODO: Quest Dungeon NPC has position 0,0
	if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) {
		return false;
	}
	*/

	if (ActionIndex + 1 < Quest->NpcSet.Count) {
		RTQuestNpcDataRef NextQuestNpc = &Quest->NpcSet.Npcs[ActionIndex + 1];
		QuestSlot->NpcActionIndex = NextQuestNpc->NpcActionOrder;
	}


	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterIncrementQuestMobCounter(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 MobSpeciesID
) {
	for (Int32 SlotIndex = 0; SlotIndex < Character->QuestSlotInfo.Count; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = 0;

		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionMobCount; MissionIndex += 1) {
			RTQuestMissionDataRef Mission = &Quest->MissionMobs[MissionIndex];
			if (MobSpeciesID == Mission->Value[0]) {
				Character->QuestSlotInfo.QuestSlot[SlotIndex].Counter[QuestCounterIndex] = MIN(
					Mission->Value[1],
					Character->QuestSlotInfo.QuestSlot[SlotIndex].Counter[QuestCounterIndex] + 1
				);

				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
	if (World->Type == RUNTIME_WORLD_TYPE_DUNGEON || World->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
		RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonID);
		Int32 PatternPartID = DungeonData->PatternPartIndices[World->PatternPartIndex];
		RTMissionDungeonPatternPartDataRef PatternPart = RTRuntimeGetPatternPartByID(Runtime, PatternPartID);
		assert(PatternPart);
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUESTSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return false;
}

Bool RTCharacterHasQuestItemCounter(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItem Item,
	UInt64 ItemOptions
) {
	for (Int32 SlotIndex = 0; SlotIndex < Character->QuestSlotInfo.Count; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = Quest->MissionMobCount;

		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionItemCount; MissionIndex += 1) {
			if (QuestSlot->Counter[QuestCounterIndex] >= Quest->MissionItems[MissionIndex].Value[2]) {
				continue;
			}
				
			RTQuestMissionDataRef Mission = &Quest->MissionItems[MissionIndex];
			if (Mission->Value[0] == Item.ID && Mission->Value[1] == ItemOptions) {
				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}

Bool RTCharacterHasQuestDungeon(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 DungeonID
) {
	for (Int32 SlotIndex = 0; SlotIndex < Character->QuestSlotInfo.Count; SlotIndex += 1) {
		RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[SlotIndex];
		RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
		if (!Quest) return false;

		Int32 QuestCounterIndex = Quest->MissionMobCount + Quest->MissionItemCount;

		for (Int32 MissionIndex = 0; MissionIndex < Quest->MissionDungeonCount; MissionIndex += 1) {
			if (Quest->MissionDungeons[MissionIndex].Value[0] == DungeonID && QuestSlot->Counter[QuestCounterIndex] < 1) {
				return true;
			}

			QuestCounterIndex += 1;
		}
	}

	return false;
}

// TODO: Add check for slot count limit based on skill rank!!
RTSkillSlotRef RTCharacterAddSkillSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillID,
	Int32 Level,
	Int32 SlotIndex
) {
	if (Character->SkillSlotInfo.Count >= RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return NULL;

	RTSkillSlotRef SkillSlot = &Character->SkillSlotInfo.Skills[Character->SkillSlotInfo.Count];
	SkillSlot->ID = SkillID;
	SkillSlot->Level = Level;
	SkillSlot->Index = SlotIndex;
	Character->SkillSlotInfo.Count += 1;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return SkillSlot;
}

RTSkillSlotRef RTCharacterGetSkillSlotByIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->SkillSlotInfo.Count; Index++) {
		if (Character->SkillSlotInfo.Skills[Index].Index == SlotIndex) {
			return &Character->SkillSlotInfo.Skills[Index];
		}
	}

	return NULL;
}

Void RTCharacterRemoveSkillSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillID,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->SkillSlotInfo.Count; Index++) {
		if (Character->SkillSlotInfo.Skills[Index].Index == SlotIndex) {
			Int32 TailLength = Character->SkillSlotInfo.Count - Index - 1;
			if (TailLength > 0) {
				memmove(
					&Character->SkillSlotInfo.Skills[Index],
					&Character->SkillSlotInfo.Skills[Index + 1],
					sizeof(struct _RTSkillSlot)
				);
			}

			Character->SkillSlotInfo.Count -= 1;

			Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
			Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
		}
	}
}

Bool RTCharacterAddQuickSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex,
	Int32 SlotIndex
) {
	if (Character->QuickSlotInfo.Count >= RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return false;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, SkillIndex);
	if (!SkillSlot) return false;

	RTQuickSlotRef QuickSlot = RTCharacterGetQuickSlotByIndex(Runtime, Character, SlotIndex);
	if (QuickSlot) {
		QuickSlot->SkillIndex = SkillIndex;
		QuickSlot->SlotIndex = SlotIndex;
		return true;
	}

	QuickSlot = &Character->QuickSlotInfo.QuickSlots[Character->QuickSlotInfo.Count];
	QuickSlot->SkillIndex = SkillIndex;
	QuickSlot->SlotIndex = SlotIndex;
	Character->QuickSlotInfo.Count += 1;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUICKSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

RTQuickSlotRef RTCharacterGetQuickSlotByIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->QuickSlotInfo.Count; Index++) {
		if (Character->QuickSlotInfo.QuickSlots[Index].SlotIndex == SlotIndex) {
			return &Character->QuickSlotInfo.QuickSlots[Index];
		}
	}

	return NULL;
}

Bool RTCharacterSwapQuickSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SourceSlotIndex,
	Int32 TargetSlotIndex
) {
	RTQuickSlotRef SourceQuickSlot = RTCharacterGetQuickSlotByIndex(Runtime, Character, SourceSlotIndex);
	RTQuickSlotRef TargetQuickSlot = RTCharacterGetQuickSlotByIndex(Runtime, Character, TargetSlotIndex);

	if (!SourceQuickSlot || !TargetQuickSlot) return false;

	SourceQuickSlot->SlotIndex = TargetSlotIndex;
	TargetQuickSlot->SlotIndex = SourceSlotIndex;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUICKSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Bool RTCharacterRemoveQuickSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->QuickSlotInfo.Count; Index++) {
		if (Character->QuickSlotInfo.QuickSlots[Index].SlotIndex == SlotIndex) {
			Int32 TailLength = Character->QuickSlotInfo.Count - Index - 1;
			if (TailLength > 0) {
				memmove(
					&Character->QuickSlotInfo.QuickSlots[Index],
					&Character->QuickSlotInfo.QuickSlots[Index + 1],
					sizeof(struct _RTQuickSlot)
				);
			}

			Character->QuickSlotInfo.Count -= 1;

			Character->SyncMask |= RUNTIME_CHARACTER_SYNC_QUICKSLOT;
			Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

			return true;
		}
	}

	return false;
}

Int32 RTCharacterCalculateRequiredMP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 CoefficientA,
	Int32 CoefficientB,
	Int32 SkillLevel
) {
	Int32 RequiredMP = (CoefficientA * SkillLevel + CoefficientB) / 10;
	// TODO: Check if level table is calculating correct value
	RequiredMP += (Int32)(SkillLevel / 10) * CoefficientA / 10;
	RequiredMP += (Int32)(SkillLevel / 13) * CoefficientA / 10;
	RequiredMP += (Int32)(SkillLevel / 16) * CoefficientA / 10;
	RequiredMP += (Int32)(SkillLevel / 19) * CoefficientA / 10;
	
	// TODO: Add battle mode based MP usage delta
	RequiredMP -= Character->SkillComboLevel * RequiredMP * 2 / 10;
	return RequiredMP;
}

Bool RTCharacterBattleRankUp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
	RTBattleStyleRankDataRef RankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Info.Style.BattleRank);
	assert(RankData);

	RTBattleStyleRankDataRef NextRankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Info.Style.BattleRank + 1);
	if (!NextRankData) return false;

	if (RankData->ConditionSTR > Character->Info.Stat[RUNTIME_CHARACTER_STAT_STR] ||
		RankData->ConditionDEX > Character->Info.Stat[RUNTIME_CHARACTER_STAT_DEX] ||
		RankData->ConditionINT > Character->Info.Stat[RUNTIME_CHARACTER_STAT_INT]) {
		return false;
	}

	if (RankData->SkillSlot[0] > 0 && RankData->SkillIndex[0] > 0) {
		RTCharacterAddSkillSlot(
			Runtime,
			Character,
			RankData->SkillSlot[0],
			1,
			RankData->SkillIndex[0]
		);

		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
	}

	if (RankData->SkillSlot[1] > 0 && RankData->SkillIndex[1] > 0) {
		RTCharacterAddSkillSlot(
			Runtime,
			Character,
			RankData->SkillSlot[1],
			1,
			RankData->SkillIndex[1]
		);

		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
	}

	Character->Info.Style.BattleRank += 1;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	RTRuntimeBroadcastEvent(
		Runtime,
		RUNTIME_EVENT_CHARACTER_BATTLE_RANK_UP,
		World,
		kEntityNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	return true;
}

Void RTCharacterAddCurrency(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 CurrencyType,
	UInt64 Amount
) {
	assert(0 <= CurrencyType && CurrencyType < RUNTIME_CHARACTER_CURRENCY_COUNT);

	Character->Info.Currency[CurrencyType] += Amount;
}

Void RTCharacterAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	UInt8 CurrentLevel = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level;
	Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Exp += Exp;
	Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level = RTRuntimeGetLevelByExp(Runtime, Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Exp);

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
	
	Bool LevelDiff = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level - CurrentLevel;
    if (LevelDiff > 0) {
		RTCharacterInitializeAttributes(Runtime, Character);
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
		Character->Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += LevelDiff * 5;

        RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

        RTRuntimeBroadcastEvent(
            Runtime,
            RUNTIME_EVENT_CHARACTER_LEVEL_UP,
            World,
			kEntityNull,
			Character->ID,
            Character->Movement.PositionCurrent.X,
            Character->Movement.PositionCurrent.Y
        );
    }
}

Int32 RTCharacterAddSkillExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 SkillExp
) {
	Int32 CurrentSkillLevel = Character->Info.Skill.Level;
	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
	RTBattleStyleSkillRankDataRef SkillRankData = RTRuntimeGetBattleStyleSkillRankData(
		Runtime,
		BattleStyleIndex,
		Character->Info.Skill.Rank
	);

	Int32 SkillLevelMax = RTRuntimeDataCharacterRankUpConditionGet(
		&Runtime->Context,
		Character->Info.Skill.Rank,
		BattleStyleIndex
	);
		
	Int32 CurrentSkillExp = CurrentSkillLevel * SkillRankData->SkillLevelExp + Character->Info.Skill.Exp;
	Int32 MaxSkillExp = SkillLevelMax * SkillRankData->SkillLevelExp;
	Int32 FinalSkillExp = MIN(MaxSkillExp, CurrentSkillExp + SkillExp);
	Int32 FinalSkillLevel = FinalSkillExp / SkillRankData->SkillLevelExp;
	Int32 SkillLevelDiff = FinalSkillLevel - CurrentSkillLevel;
	Int32 ReceivedSkillExp = FinalSkillExp - CurrentSkillExp;

	Character->Info.Skill.Exp = FinalSkillExp % SkillRankData->SkillLevelExp;
	Character->Info.Skill.Level += SkillLevelDiff;
	Character->Info.Skill.Point += SkillLevelDiff;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	if (SkillLevelDiff > 0) {
		RTRuntimeBroadcastEvent(
			Runtime,
			RUNTIME_EVENT_CHARACTER_SKILL_LEVEL_UP,
			World,
			kEntityNull,
			Character->ID,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y
		);
	}

	if (Character->Info.Skill.Level >= SkillLevelMax && 
		Character->Info.Style.BattleRank > Character->Info.Skill.Rank) {
		RTBattleStyleSkillRankDataRef NextSkillRankData = RTRuntimeGetBattleStyleSkillRankData(
			Runtime,
			BattleStyleIndex,
			Character->Info.Skill.Rank + 1
		);
		
		if (NextSkillRankData) {
			Character->Info.Skill.Rank += 1;
			Character->Info.Skill.Level = 0;
		}

		RTRuntimeBroadcastEvent(
			Runtime,
			RUNTIME_EVENT_CHARACTER_SKILL_RANK_UP,
			World,
			kEntityNull,
			Character->ID,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y
		);
	}

	return ReceivedSkillExp;
}

Void RTCharacterAddHonorPoint(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 HonorPoint
) {
	Character->Info.Honor.Point += HonorPoint;

	RTDataHonorLevelFormulaRef CurrentLevelData = RTRuntimeDataHonorLevelFormulaGet(
		Runtime->Context,
		Character->Info.Honor.Rank
	);
	assert(CurrentLevelData);

	while (Character->Info.Honor.Point < CurrentLevelData->MinPoint) {
		RTDataHonorLevelFormulaRef NextLevelData = RTRuntimeDataHonorLevelFormulaGet(
			Runtime->Context,
			Character->Info.Honor.Rank - 1
		);
		if (!NextLevelData) break;

		Character->Info.Honor.Rank = NextLevelData->Rank;
		CurrentLevelData = NextLevelData;
	}

	while (Character->Info.Honor.Point > CurrentLevelData->MaxPoint) {
		RTDataHonorLevelFormulaRef NextLevelData = RTRuntimeDataHonorLevelFormulaGet(
			Runtime->Context,
			Character->Info.Honor.Rank + 1
		);
		if (!NextLevelData) break;

		Character->Info.Honor.Rank = NextLevelData->Rank;
		CurrentLevelData = NextLevelData;
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Void RTCharacterAddAbilityExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 Exp
) {
	Character->Info.Ability.Exp += Exp;

	while (Character->Info.Ability.Exp >= RUNTIME_CHARACTER_AXP_PER_LEVEL) {
		Character->Info.Ability.Exp -= RUNTIME_CHARACTER_AXP_PER_LEVEL;
		Character->Info.Ability.Point += 1;
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Bool RTCharacterAddStats(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32* Stats
) {
	Int32 RequiredPoints = Stats[RUNTIME_CHARACTER_STAT_STR] + Stats[RUNTIME_CHARACTER_STAT_DEX] + Stats[RUNTIME_CHARACTER_STAT_INT];
	if (Character->Info.Stat[RUNTIME_CHARACTER_STAT_PNT] < RequiredPoints) {
		return false;
	}

	Character->Info.Stat[RUNTIME_CHARACTER_STAT_STR] += Stats[RUNTIME_CHARACTER_STAT_STR];
	Character->Info.Stat[RUNTIME_CHARACTER_STAT_DEX] += Stats[RUNTIME_CHARACTER_STAT_DEX];
	Character->Info.Stat[RUNTIME_CHARACTER_STAT_INT] += Stats[RUNTIME_CHARACTER_STAT_INT];
	Character->Info.Stat[RUNTIME_CHARACTER_STAT_PNT] -= RequiredPoints;

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	RTCharacterInitializeAttributes(Runtime, Character);
	return true;
}

Void RTCharacterAddHP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 HP
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] += HP;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MAX(
		0,
		MIN(
			Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX],
			Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]
		)
	);

	// TODO: Send notification
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Void RTCharacterAddMP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MP
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] += MP;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = MAX(
		0,
		MIN(
			Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX],
			Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT]
		)
	);

	// TODO: Send notification
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

Void RTCharacterAddSP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SP
) {
	UNIMPLEMENTED;
}

Bool RTCharacterChangeSkillLevel(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillID,
	Int32 SlotIndex,
	Int32 CurrentSkillLevel,
	Int32 TargetSkillLevel
) {
	if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return false;
	if (SkillID < 0 || SkillID >= Runtime->CharacterSkillDataCount) return false;
	
	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, SlotIndex);
	if (!SkillSlot || SkillSlot->ID != SkillID || SkillSlot->Level != CurrentSkillLevel) return false;

	Int32 LevelDiff = TargetSkillLevel - CurrentSkillLevel;
	Int32 Increment = LevelDiff < 0 ? -1 : 1;
	Int32 RequiredSkillPointCount = 0;
	Int64 RequiredCurrencyAlz = 0;

	for (Int32 NextLevel = CurrentSkillLevel + Increment; NextLevel <= TargetSkillLevel; NextLevel += Increment) {
		RTSkillLevelDataRef SkillLevelData = RTRuntimeGetSkillLevelDataByID(Runtime, SkillID, NextLevel);
		if (!SkillLevelData) return false;

		if (Character->Info.Skill.Rank < SkillLevelData->SkillRank) return false;
		if (Character->Info.Skill.Level < SkillLevelData->SkillRankPoint) return false;
	
		RequiredSkillPointCount += SkillLevelData->SkillPoint;
		RequiredCurrencyAlz += LevelDiff < 0 ? SkillLevelData->UntrainPrice : SkillLevelData->TrainPrice;

		// TODO: Check SkillLevelData->StyleMastery

		Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
		if (SkillLevelData->BattleStyles[BattleStyleIndex - 1] <= 0) return false;
	}

	if (Character->Info.Skill.Point < RequiredSkillPointCount) return false;
	if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < RequiredCurrencyAlz) return false;

	Character->Info.Skill.Point -= RequiredSkillPointCount;
	Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= RequiredCurrencyAlz;
	SkillSlot->Level = TargetSkillLevel;

	if (TargetSkillLevel <= 0) {
		RTCharacterRemoveSkillSlot(Runtime, Character, SkillID, SlotIndex);
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}

Void RTCharacterApplyDamage(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTEntity Source,
	Int32 Damage
) {
	Character->Info.Resource.DiffHP += Damage;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MAX(0,
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] -
		Character->Info.Resource.DiffHP
	);

	// TODO: Send notification
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
}

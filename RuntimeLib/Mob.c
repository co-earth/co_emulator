#include "Mob.h"
#include "Movement.h"
#include "Runtime.h"

Void RTMobInit(
	RTRuntimeRef Runtime,
	RTMobRef Mob
) {
	assert(Mob && Mob->Data);

	Mob->Attributes.Seed = Mob->ID.Serial;
	Mob->Attributes.AttackTimeout = 0;
	memset(Mob->Attributes.Values, 0, sizeof(Mob->Attributes.Values));
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->Data->MoveSpeed;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = Mob->Data->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = Mob->Data->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] = Mob->Data->AttackRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE] = Mob->Data->Defense;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE] = Mob->Data->DefenseRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_EXP] = Mob->Data->Exp;
	Mob->HPTriggerThreshold = (Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] * 300) / 1000;
	Mob->SpecialAttack = NULL;
	Mob->SpecialAttackSkill = NULL;
	Mob->ActiveSkill = &Mob->Data->DefaultSkill;
}

Bool RTMobCanMove(RTMobRef Mob) {
	return Mob->Data->CanMove;
}

Bool RTMobCanAttack(RTMobRef Mob) {
	return Mob->Data->CanAttack;
}

Bool RTMobCanRespawn(RTMobRef Mob) {
	if (!Mob->Spawn.SpawnDefault) {
		return false;
	}

	if (Mob->IsPermanentDeath) {
		return false;
	}

	return Mob->IsInfiniteSpawn || Mob->RemainingSpawnCount > 0;
}

Bool RTMobIsAlive(RTMobRef Mob) {
	if (!Mob->IsSpawned) {
		return false;
	}

    if (Mob->IsKilled) {
        return false;
    }

	return Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] > 0;
}

Bool RTMobIsUnmovable(RTMobRef Mob) {
	return false;
}

Bool RTMobIsAggressive(RTMobRef Mob) {
	return !(Mob->Data->AggressiveType == RUNTIME_MOB_AGGRESSIVE_TYPE_PASSIVE);
}

Void RTMobUpdateActiveSkill(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob,
	Int32 TargetDistance
) {
	Mob->SpecialAttack = NULL;
	Mob->SpecialAttackSkill = NULL;

	if (Mob->Data->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_NONE) {
		Mob->ActiveSkill = &Mob->Data->DefaultSkill;
	}

	if (Mob->Data->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_HP_TRIGGER) {
		if (Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] < Mob->HPTriggerThreshold) {
			Mob->ActiveSkill = &Mob->Data->SpecialSkill;
		}
	}

	if (Mob->Data->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_SITUATIONAL) {
		assert(false && "Implementation missing!");
	}

	if (Mob->Data->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_BY_REACH) {
		if (Mob->Data->SpecialSkill.Reach >= TargetDistance) {
			Mob->ActiveSkill = &Mob->Data->SpecialSkill;
		}
		else if (Mob->Data->DefaultSkill.Reach >= TargetDistance) {
			Mob->ActiveSkill = &Mob->Data->DefaultSkill;
		}
	}

	if (Mob->Data->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_SELECTIVE) {
		RTDataMobAttackIndexRef AttackIndex = RTRuntimeDataMobAttackIndexGet(Runtime->Context, Mob->Data->Index);
		if (AttackIndex && AttackIndex->MobAttackLinkCount > 0) {
			Int32 AttackLinkIndex = RandomRange(&World->Seed, 0, AttackIndex->MobAttackLinkCount - 1);
			RTDataMobAttackLinkRef AttackLink = &AttackIndex->MobAttackLinkList[AttackLinkIndex];

			Mob->SpecialAttack = RTRuntimeDataMobAttackGet(Runtime->Context, AttackLink->MobAttackID);
			assert(Mob->SpecialAttack);

			if (Mob->SpecialAttack->SkillID > 0) {
				Mob->SpecialAttackSkill = RTRuntimeDataMobSkillGet(Runtime->Context, Mob->SpecialAttack->SkillID);
				assert(Mob->SpecialAttackSkill);
			}
		}
		else {
			Int32 Random = RandomRange(&World->Seed, 0, INT32_MAX);
			if (Random > INT32_MAX / 2) {
				Mob->ActiveSkill = &Mob->Data->SpecialSkill;
			}
			else {
				Mob->ActiveSkill = &Mob->Data->DefaultSkill;
			}
		}
	}
}

RTEntity RTMobGetMaxAggroTarget(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob
) {
	Int32 MaxAggroIndex = -1;
	Int32 Index = 0;

	while (Index < Mob->Aggro.Count) {
		RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Mob->Aggro.Entities[Index]);
		if (!Character || Character->Info.Position.WorldID != World->WorldIndex || !RTCharacterIsAlive(Runtime, Character)) {
			Int32 TailLength = Mob->Aggro.Count - Index - 1;
			if (TailLength > 0) {
				memmove(&Mob->Aggro.Entities[Index], &Mob->Aggro.Entities[Index + 1], sizeof(RTEntity));
				memmove(&Mob->Aggro.ReceivedDamage[Index], &Mob->Aggro.ReceivedDamage[Index + 1], sizeof(Int64));
			}

			Mob->Aggro.Count -= 1;
			continue;
		}

		if (MaxAggroIndex < 0) {
			MaxAggroIndex = Index;
		}
		else if (Mob->Aggro.ReceivedDamage[MaxAggroIndex] < Mob->Aggro.ReceivedDamage[Index]) {
			MaxAggroIndex = Index;
		}

		Index += 1;
	}

	if (MaxAggroIndex < 0) {
		return kEntityNull;
	}

	return Mob->Aggro.Entities[MaxAggroIndex];
}

Void RTMobCancelMovement(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob
) {
	if (Mob->Movement.IsMoving) {
		Mob->Movement.PositionEnd.X = Mob->Movement.PositionCurrent.X;
		Mob->Movement.PositionEnd.Y = Mob->Movement.PositionCurrent.Y;
		RTMovementEndDeadReckoning(Runtime, &Mob->Movement);

        RTEventData EventData = { 0 };
        EventData.MobMovementEnd.PositionCurrentX = Mob->Movement.PositionCurrent.X;
        EventData.MobMovementEnd.PositionCurrentY = Mob->Movement.PositionCurrent.Y;

		RTRuntimeBroadcastEventData(
			Runtime,
			(Mob->IsChasing) ? RUNTIME_EVENT_MOB_CHASE_END : RUNTIME_EVENT_MOB_MOVEMENT_END,
			World,
			kEntityNull,
			Mob->ID,
			Mob->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.Y,
            EventData
		);
	}
}

Void RTMobApplyDamage(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob,
	RTEntity Source,
	Int32 Damage
) {
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] -= Damage;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MAX(0, Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	
	if (Source.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
		Bool Found = false;
		for (Int32 Index = 0; Index < Mob->Aggro.Count; Index += 1) {
			if (Mob->Aggro.Entities[Index].Serial == Source.Serial) {
				Mob->Aggro.ReceivedDamage[Index] += Damage;
				Found = true;
				break;
			}
		}

		if (!Found && Mob->Aggro.Count < RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT) {
			Mob->Aggro.Entities[Mob->Aggro.Count] = Source;
			Mob->Aggro.ReceivedDamage[Mob->Aggro.Count] = Damage;
			Mob->Aggro.Count += 1;
		}

		RTMobCancelMovement(Runtime, World, Mob);
	}
}

Void RTMobAttackTarget(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob,
	RTCharacterRef Character
) {
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK] = RandomRange(
		&World->Seed, 
		Mob->ActiveSkill->PhysicalAttackMin, 
		Mob->ActiveSkill->PhysicalAttackMax
	);

	if (Mob->SpecialAttackSkill) {
		assert(false && "Not Implemented!");
	}

	// TODO: Check SkillGroup for attack patterns: single, aoe, movement, heal, buff, debuff

	struct _RTBattleResult Result = { 0 };
	RTCalculateNormalAttackResult(
		Runtime,
		RUNTIME_BATTLE_SKILL_TYPE_SWORD,
		Mob->Data->Level,
		&Mob->Attributes,
		Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level,
		&Character->Attributes,
		&Result
	);

	Mob->NextTimestamp = GetTickCount64() + Mob->ActiveSkill->Interval;

	RTEventData EventData = { 0 };
	EventData.MobAttack.IsDefaultSkill = true;
	EventData.MobAttack.ResultCount = 1;
	EventData.MobAttack.Results[0].Entity = Character->ID;
	EventData.MobAttack.Results[0].Result = Result.AttackType;
	EventData.MobAttack.Results[0].IsDead = Result.IsDead;
	EventData.MobAttack.Results[0].AppliedDamage = Result.AppliedDamage;
	EventData.MobAttack.Results[0].TargetHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];

	RTRuntimeBroadcastEventData(
		Runtime,
		RUNTIME_EVENT_MOB_ATTACK,
		World,
		Mob->ID,
		Character->ID,
		Mob->Movement.PositionCurrent.X,
		Mob->Movement.PositionCurrent.Y,
		EventData
	);
}

Void RTMobUpdate(
	RTRuntimeRef Runtime,
	RTWorldRef World,
	RTMobRef Mob
) {
	Timestamp Timestamp = GetTickCount64();
	if (Timestamp < Mob->NextTimestamp) {
		return;
	}

	if (!RTMobIsAlive(Mob) && Mob->IsSpawned) {
		Mob->IsSpawned = false;
		Mob->NextTimestamp = Timestamp + Mob->Spawn.SpawnInterval;
		return RTWorldDespawnMob(Runtime, World, Mob);
	}

	if (!RTMobIsAlive(Mob)) {
		if (RTMobCanRespawn(Mob)) {
			Mob->Aggro.Count = 0;
			memset(&Mob->Aggro.Entities, 0, sizeof(Mob->Aggro.Entities));

			Mob->IsKilled = false;
			Mob->IsSpawned = true;
			Mob->RemainingSpawnCount = MAX(0, Mob->RemainingSpawnCount - 1);
			Mob->NextTimestamp = Timestamp;
			return RTWorldSpawnMob(Runtime, World, Mob);
		}

		return;
	}

	RTMovementUpdateDeadReckoning(Runtime, &Mob->Movement);

	if (Mob->Movement.IsMoving) {
		if (Mob->Movement.PositionCurrent.X == Mob->Movement.PositionEnd.X &&
			Mob->Movement.PositionCurrent.Y == Mob->Movement.PositionEnd.Y) {
			RTMovementEndDeadReckoning(Runtime, &Mob->Movement);
            
            RTEventData EventData = { 0 };
            EventData.MobMovementEnd.PositionCurrentX = Mob->Movement.PositionCurrent.X;
            EventData.MobMovementEnd.PositionCurrentY = Mob->Movement.PositionCurrent.Y;

			RTRuntimeBroadcastEventData(
				Runtime,
				(Mob->IsChasing) ? RUNTIME_EVENT_MOB_CHASE_END : RUNTIME_EVENT_MOB_MOVEMENT_END,
				World,
				kEntityNull,
				Mob->ID,
				Mob->Movement.PositionCurrent.X,
				Mob->Movement.PositionCurrent.Y,
                EventData
			);
		}

		if (Mob->IsChasing) {
			Mob->NextTimestamp = Timestamp + Mob->Data->ChaseInterval;
		}
		else {
			Mob->NextTimestamp = Timestamp + Mob->Data->MoveInterval;
		}

		return;
	}

	RTEntity Target = RTMobGetMaxAggroTarget(Runtime, World, Mob);
	if (!Target.Serial && Mob->RemainingFindCount > 0) {
		if (RTMobIsAggressive(Mob)) {
			RTCharacterRef NearbyCharacter = NULL;
			Int32 MinDistance = Mob->Data->AlertRange;

			// TODO: Don't be insane and replace this ridiculous iteration with a spatial hashing solution...
			for (Int32 Index = 0; Index < World->CharacterCount; Index += 1) {
				RTEntity Entity = World->Characters[Index];
				RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Entity);
				assert(Character);

				Int32 LevelDifference = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Level - Mob->Data->Level;
				if (LevelDifference > RUNTIME_MOB_MAX_FIND_LEVEL_DIFFERENCE) {
					continue;
				}

				Int32 Distance = RTMovementDistance(&Mob->Movement, &Character->Movement);
				if (Distance > MinDistance) {
					continue;
				}

				Bool IsReachable = RTWorldTraceMovement(
					Runtime,
					World,
					Mob->Movement.PositionCurrent.X,
					Mob->Movement.PositionCurrent.Y,
					Character->Movement.PositionCurrent.X,
					Character->Movement.PositionCurrent.Y,
					NULL,
					NULL,
					Mob->Movement.CollisionMask,
					Mob->Movement.IgnoreMask
				);

				if (!IsReachable) {
					continue;
				}

				MinDistance = Distance;
				NearbyCharacter = Character;
			}

			if (NearbyCharacter) {
				assert(Mob->Aggro.Count < RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT);

				Mob->Aggro.Entities[Mob->Aggro.Count] = NearbyCharacter->ID;
				Mob->Aggro.ReceivedDamage[Mob->Aggro.Count] = 0;
				Mob->Aggro.Count += 1;
				Mob->RemainingFindCount = Mob->Data->FindCount;
				return;
			}
		}

		Mob->RemainingFindCount = MAX(0, Mob->RemainingFindCount - 1);
		Mob->NextTimestamp = Timestamp + Mob->Data->FindInterval;
		return;
	}

	Int32 TargetPositionX = -1;
	Int32 TargetPositionY = -1;

	if (Target.Serial && RTMobCanAttack(Mob)) {
		RTCharacterRef Character = RTRuntimeGetCharacter(Runtime, Target);
		assert(Character);

		Int32 Distance = RTMovementDistance(&Mob->Movement, &Character->Movement);
		RTMobUpdateActiveSkill(Runtime, World, Mob, Distance);

		if (Distance <= Mob->ActiveSkill->Distance) {
			RTMobAttackTarget(Runtime, World, Mob, Character);
			return;
		}
		else {
			TargetPositionX = Character->Movement.PositionCurrent.X;
			TargetPositionY = Character->Movement.PositionCurrent.Y;
		}
	}

	if (RTMobCanMove(Mob) && !RTMobIsUnmovable(Mob)) {
		if (TargetPositionX < 0 || TargetPositionY < 0) {
			TargetPositionX = RandomRange(&World->Seed, Mob->Spawn.AreaX, Mob->Spawn.AreaX + Mob->Spawn.AreaWidth);
			TargetPositionY = RandomRange(&World->Seed, Mob->Spawn.AreaY, Mob->Spawn.AreaY + Mob->Spawn.AreaHeight);
			Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->Data->MoveSpeed;
			Mob->Movement.Speed = Mob->Data->MoveSpeed;
			Mob->IsChasing = false;
		}
		else {
			Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->Data->ChaseSpeed;
			Mob->Movement.Speed = Mob->Data->ChaseSpeed;
			Mob->IsChasing = true;
		}

		Mob->Movement.PositionEnd.X = TargetPositionX;
		Mob->Movement.PositionEnd.Y = TargetPositionY;

		Bool PathFound = RTMovementFindPath(
			Runtime,
			World,
			&Mob->Movement
		);

		if (!PathFound) {
			if (Mob->Movement.WaypointCount != 2) return;

			Mob->Movement.PositionEnd.X = Mob->Movement.Waypoints[1].X;
			Mob->Movement.PositionEnd.Y = Mob->Movement.Waypoints[1].Y;
		}

		RTMovementStartDeadReckoning(Runtime, &Mob->Movement);
        
        RTEventData EventData = { 0 };
        EventData.MobMovementBegin.TickCount = Mob->Movement.TickCount;
        EventData.MobMovementBegin.PositionBeginX = Mob->Movement.PositionBegin.X;
        EventData.MobMovementBegin.PositionBeginY = Mob->Movement.PositionBegin.Y;
        EventData.MobMovementBegin.PositionEndX = Mob->Movement.PositionEnd.X;
        EventData.MobMovementBegin.PositionEndY = Mob->Movement.PositionEnd.Y;
        
		RTRuntimeBroadcastEventData(
			Runtime,
			(Mob->IsChasing) ? RUNTIME_EVENT_MOB_CHASE_BEGIN : RUNTIME_EVENT_MOB_MOVEMENT_BEGIN,
			World,
			kEntityNull,
			Mob->ID,
			Mob->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.Y,
            EventData
		);

		if (Mob->IsChasing) {
			Mob->NextTimestamp = Timestamp + Mob->Data->ChaseInterval;
		}
		else {
			Mob->NextTimestamp = Timestamp + Mob->Data->MoveInterval;
		}

		return;
	}

	Mob->RemainingFindCount = Mob->Data->FindCount;
}

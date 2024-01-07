#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_MOB) {
	if (!Character) goto error;

	// TODO: Move logic to Runtime!

	if (!RTCharacterIsAlive(Runtime, Character)) goto error;

	if (Character->Movement.IsMoving ||
		Character->Movement.IsDeadReckoning ||
		Character->Movement.PositionBegin.X != Character->Movement.PositionCurrent.X ||
		Character->Movement.PositionBegin.Y != Character->Movement.PositionCurrent.Y ||
		Character->Movement.PositionBegin.X != Character->Movement.PositionEnd.X ||
		Character->Movement.PositionBegin.Y != Character->Movement.PositionEnd.Y) {
		goto error;
	}

	if (Packet->SlotIndex < 0 || Packet->SlotIndex > RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT ||
		Packet->SkillIndex < 0 || Packet->SkillIndex >= Runtime->CharacterSkillDataCount) {
		goto error;
	}

	// TODO: Packet could eventually contains SkillIndex but no slotindex
	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Packet->SlotIndex);
	if (!SkillSlot) goto error;

	RTCharacterSkillDataRef Skill = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
	assert(Skill);

	Bool UseMoveStep = false;
	Bool UseStun = false;
	Bool UseHitCount = false;
	Bool UseDebuff = false;

	// TODO: Add skill cast time and cooldown checks

	// TODO: Calculate timing of combo skill

	if (Packet->TargetCount < 1 || (Packet->TargetCount > 1 && !Skill->Multi)) goto error;

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		Skill->Mp[0],
		Skill->Mp[1],
		SkillSlot->Level
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		// TODO: Send error notification
		return;
	}

    Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] -= RequiredMP;
    
	// TODO: Apply dash by skill if needed

	RTMovementUpdateDeadReckoning(Server->Runtime, &Character->Movement);

	Int32 CharacterPositionError = RTCalculateDistance(
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Packet->PositionCharacter.X,
		Packet->PositionCharacter.Y
	);
	if (CharacterPositionError > 1) goto error;

	Int32 SkillPositionX = Packet->PositionTarget.X;
	Int32 SkillPositionY = Packet->PositionTarget.Y;

	S2C_DATA_SKILL_TO_MOB* Response = PacketInit(S2C_DATA_SKILL_TO_MOB);
	Response->Command = S2C_SKILL_TO_MOB;
	Response->SkillIndex = Packet->SkillIndex;
	Response->CharacterHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Response->CharacterMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	Response->CharacterSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
	Response->Unknown4 = -1;
	Response->CharacterMaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
	
	Int32 TargetCount = 0;
	Int32 ReceivedSkillExp = 0;

	RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	// TODO: Add evaluation for each target
	for (Int32 Index = 0; Index < Packet->TargetCount; Index++) {
		C2S_DATA_SKILL_TO_MOB_TARGET* Target = &Packet->Data[Index];
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = PacketAppendStruct(S2C_DATA_SKILL_TO_MOB_TARGET);
		TargetResponse->Entity = Target->Entity;
		TargetResponse->EntityIDType = Target->EntityIDType;

		RTMobRef Mob = RTWorldGetMobByID(Runtime, World, Target->Entity);
		if (!Mob || Mob->IsDead) {
			TargetResponse->AttackType = RUNTIME_ATTACK_TYPE_MISS;
			continue;
		}
		/*
		Bool IsValidRange = RTCheckSkillTargetDistance(
			Skill,
			Mob->Data->Radius,
			Mob->Data->Scale,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.Y
		);
		if (!IsValidRange) {
			TargetResponse->AttackType = RUNTIME_ATTACK_TYPE_MISS;
			continue;
		}
		*/
		struct _RTBattleResult Result = { 0 };
		RTCalculateSkillAttackResult(
			Runtime,
			SkillSlot->Level,
			Skill,
			Character->Info.Level,
			&Character->Attributes,
			Mob->Data->Level,
			&Mob->Attributes,
			&Result
		);

		RTMobApplyDamage(Runtime, World, Mob, Character->ID, Result.AppliedDamage);
		RTCharacterAddExp(Runtime, Character, Result.Exp);

		ReceivedSkillExp += RTCharacterAddSkillExp(Runtime, Character, Result.SkillExp);

		TargetResponse->AttackType = Result.AttackType;
		TargetResponse->MobAppliedDamage = Result.AppliedDamage;
		TargetResponse->MobTotalDamage = Result.TotalDamage;
		TargetResponse->MobAdditionalDamage = Result.AdditionalDamage;
		TargetResponse->MobHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		TargetResponse->Unknown3 = 1;

		// TODO: This should be calculated globally inside the mob logic when it dies by a bfx effect and no active attack!
		if (Result.IsDead) {
			if (RTCharacterIncrementQuestMobCounter(Runtime, Character, Mob->Spawn.MobSpeciesID)) {
				S2C_DATA_NFY_QUEST_MOB_KILL* Notification = PacketInit(S2C_DATA_NFY_QUEST_MOB_KILL);
				Notification->Command = S2C_NFY_QUEST_MOB_KILL;
				Notification->MobSpeciesID = Mob->Spawn.MobSpeciesID;
				Notification->SkillIndex = 0;
				Notification->Unknown1 = 0;
				SocketSend(Socket, Connection, Notification);
			}

			Mob->DropOwner = Character->ID;
		}

		TargetCount += 1;
	}

	Response->AccumulatedExp = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_BASIC].Exp;
	Response->AccumulatedOxp = Character->Info.Level[RUNTIME_CHARACTER_LEVEL_OVERLORD].Exp;
	Response->ReceivedSkillExp = ReceivedSkillExp;
	Response->TargetCount = TargetCount;
	SocketSend(Socket, Connection, Response);

	if (ReceivedSkillExp > 0) {
		RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

		RTRuntimeBroadcastEvent(
			Runtime,
			RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS,
			World,
			kEntityNull,
			Client->CharacterEntity,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y
		);
	}

	S2C_DATA_NFY_SKILL_TO_MOB* Notification = PacketInit(S2C_DATA_NFY_SKILL_TO_MOB);
	Notification->Command = S2C_NFY_SKILL_TO_MOB;
	Notification->SkillIndex = Response->SkillIndex;
	Notification->TargetCount = Response->TargetCount;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->PositionSet.X = Character->Info.Position.X;
	Notification->PositionSet.Y = Character->Info.Position.Y;
	Notification->CharacterHP = Response->CharacterHP;

	for (Int32 Index = 0; Index < Response->TargetCount; Index++) {
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = &Response->Data[Index];
		S2C_DATA_NFY_SKILL_TO_MOB_TARGET* TargetNotification = PacketAppendStruct(S2C_DATA_NFY_SKILL_TO_MOB_TARGET);
		TargetNotification->Entity = TargetResponse->Entity;
		TargetNotification->EntityIDType = TargetResponse->EntityIDType;
		TargetNotification->AttackType = TargetResponse->AttackType;
		TargetNotification->MobHP = TargetResponse->MobHP;
		TargetNotification->Unknown3 = 1;
	}

	return BroadcastToWorld(
		Server,
		World,
		kEntityNull,
		Character->Info.Position.X,
		Character->Info.Position.Y,
		Notification
	);
	
error:
	return SocketDisconnect(Socket, Connection);
}
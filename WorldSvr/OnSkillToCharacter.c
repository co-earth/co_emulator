#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_CHARACTER) {
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

	// TODO: Add skill cast time and cooldown checks

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

	S2C_DATA_SKILL_TO_CHARACTER* Response = PacketInit(S2C_DATA_SKILL_TO_CHARACTER);
	Response->Command = S2C_SKILL_TO_CHARACTER;
	Response->SkillIndex = Packet->SkillIndex;

	if (Skill->SkillGroup == RUNTIME_SKILL_GROUP_MOVEMENT) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_MOVEMENT);
		if (Packet->Signature.Length != PacketLength) goto error;
		
		C2S_DATA_SKILL_GROUP_MOVEMENT* PacketData = (C2S_DATA_SKILL_GROUP_MOVEMENT*)&Packet->Data[0];

		S2C_DATA_SKILL_GROUP_MOVEMENT* ResponseData = PacketAppendStruct(S2C_DATA_SKILL_GROUP_MOVEMENT);
		ResponseData->CharacterMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];

		RTMovementUpdateDeadReckoning(Server->Runtime, &Character->Movement);

		Int32 CharacterPositionError = RTCalculateDistance(
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y,
			PacketData->PositionBegin.X,
			PacketData->PositionBegin.Y
		);
		if (CharacterPositionError > 1) goto error;
		
		Bool IsValidRange = RTCheckSkillTargetDistance(
			Skill,
			1,
			1,
			PacketData->PositionBegin.X,
			PacketData->PositionBegin.Y,
			PacketData->PositionEnd.X,
			PacketData->PositionEnd.Y
		);
		if (!IsValidRange) goto error;

		RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
		if (!World) goto error;

		Bool IsValidMovement = RTWorldTraceMovement(
			Runtime,
			World,
			PacketData->PositionBegin.X,
			PacketData->PositionBegin.Y,
			PacketData->PositionEnd.X,
			PacketData->PositionEnd.Y,
			NULL,
			NULL,
            Character->Movement.CollisionMask,
			0
		);
		if (!IsValidMovement) goto error;

		RTMovementEndDeadReckoning(Runtime, &Character->Movement);

		Character->Movement.PositionCurrent.X = PacketData->PositionEnd.X;
		Character->Movement.PositionCurrent.Y = PacketData->PositionEnd.Y;
		Character->Movement.PositionBegin.X = PacketData->PositionEnd.X;
		Character->Movement.PositionBegin.Y = PacketData->PositionEnd.Y;
		Character->Movement.PositionEnd.X = PacketData->PositionEnd.X;
		Character->Movement.PositionEnd.Y = PacketData->PositionEnd.Y;

		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

		return SocketSend(Socket, Connection, Response);
	}
	else if (Skill->SkillGroup == RUNTIME_SKILL_GROUP_ASTRAL) {
		// TODO: Activate astral weapon
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_ASTRAL);
		if (Packet->Signature.Length != PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_ASTRAL* PacketData = (C2S_DATA_SKILL_GROUP_ASTRAL*)&Packet->Data[0];

		S2C_DATA_SKILL_GROUP_ASTRAL* ResponseData = PacketAppendStruct(S2C_DATA_SKILL_GROUP_ASTRAL);
		ResponseData->Unknown1 = PacketData->Unknown1;
		ResponseData->Unknown2 = PacketData->Unknown2;

		SocketSend(Socket, Connection, Response);

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Response = PacketInit(S2C_DATA_NFY_SKILL_TO_CHARACTER);
		Response->Command = S2C_NFY_SKILL_TO_CHARACTER;
		Response->SkillIndex = Packet->SkillIndex;
		Response->CharacterIndex = Client->CharacterIndex;
		Response->Entity = Character->ID;
		Response->Position.X = Character->Movement.PositionCurrent.X;
		Response->Position.Y = Character->Movement.PositionCurrent.Y;

		ResponseData = PacketAppendStruct(S2C_DATA_SKILL_GROUP_ASTRAL);
		ResponseData->Unknown1 = PacketData->Unknown1;
		ResponseData->Unknown2 = PacketData->Unknown2;

		return SocketSend(Socket, Connection, Response);
	}
	else {
		goto error;
	}

	// TODO: Apply dash by attack skill if needed

	SocketSend(Socket, Connection, Response);

	/* TODO: Send notification S2C_DATA_SKILL_TO_CHARACTER_UPDATE
	
	S2C_DATA_SKILL_TO_MOB_UPDATE* Notification = PacketInit(S2C_DATA_SKILL_TO_MOB_UPDATE);
	Notification->Command = S2C_SKILL_TO_MOB_UPDATE;
	Notification->SkillIndex = Response->SkillIndex;
	Notification->TargetCount = Response->TargetCount;
	Notification->CharacterID = Character->ID;
	Notification->SetPositionX = Character->Info.Position.X;
	Notification->SetPositionY = Character->Info.Position.Y;
	Notification->CharacterHP = Response->CharacterHP;

	for (Int32 Index = 0; Index < Response->TargetCount; Index++) {
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = &Response->Data[Index];
		S2C_DATA_SKILL_TO_MOB_TARGET_UPDATE* TargetNotification = PacketAppendStruct(S2C_DATA_SKILL_TO_MOB_TARGET_UPDATE);
		TargetNotification->EntityID = TargetResponse->EntityID;
		TargetNotification->WorldID = TargetResponse->WorldID;
		TargetNotification->EntityType = TargetResponse->EntityType;
		TargetNotification->AttackType = TargetResponse->AttackType;
		TargetNotification->MobHP = TargetResponse->MobHP;
		TargetNotification->Unknown3 = 1;
	}

	return BroadcastToWorld(
		Server,
		World,
		Character->ID,
		Character->Info.Position.X,
		Character->Info.Position.Y,
		Notification
	);
	*/
	return;

error:
	return SocketDisconnect(Socket, Connection);
}


#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

CLIENT_PROCEDURE_BINDING(MOVEMENT_BEGIN) {
	if (!Character) goto error;
	
    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementBegin(
		Runtime,
		Character,
		Packet->WorldID,
		Packet->PositionBegin.X,
		Packet->PositionBegin.Y,
		Packet->PositionCurrent.X,
		Packet->PositionCurrent.Y,
		Packet->PositionEnd.X,
		Packet->PositionEnd.Y
	);
	if (!Success) goto error;

	S2C_DATA_MOVEMENT_BEGIN* Notification = PacketInit(S2C_DATA_MOVEMENT_BEGIN);
	Notification->Command = S2C_MOVEMENT_BEGIN;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionBegin.X = Packet->PositionBegin.X;
	Notification->PositionBegin.Y = Packet->PositionBegin.Y;
	Notification->PositionEnd.X = Packet->PositionEnd.X;
	Notification->PositionEnd.Y = Packet->PositionEnd.Y;

	return BroadcastToWorld(
		Server,
		World,
		Client->CharacterEntity,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_END) {
	if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementEnd(
		Runtime,
		Character,
		Packet->Position.X,
		Packet->Position.Y
	);
	if (!Success) goto error;

	S2C_DATA_MOVEMENT_END* Notification = PacketInit(S2C_DATA_MOVEMENT_END);
	Notification->Command = S2C_MOVEMENT_END;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->Position.X = Packet->Position.X;
	Notification->Position.Y = Packet->Position.Y;

	LogMessageFormat(
		LOG_LEVEL_INFO,
		"(%d, %d)",
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	return BroadcastToWorld(
		Server,
		World,
		Client->CharacterEntity,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_CHANGE) {
	if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementChange(
		Runtime,
		Character,
		Packet->WorldID,
		Packet->PositionBegin.X,
		Packet->PositionBegin.Y,
		Packet->PositionCurrent.X,
		Packet->PositionCurrent.Y,
		Packet->PositionEnd.X,
		Packet->PositionEnd.Y
	);
	if (!Success) goto error;

	S2C_DATA_MOVEMENT_CHANGE* Notification = PacketInit(S2C_DATA_MOVEMENT_CHANGE);
	Notification->Command = S2C_MOVEMENT_CHANGE;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionBegin.X = Packet->PositionBegin.X;
	Notification->PositionBegin.Y = Packet->PositionBegin.Y;
	Notification->PositionEnd.X = Packet->PositionEnd.X;
	Notification->PositionEnd.Y = Packet->PositionEnd.Y;

	return BroadcastToWorld(
		Server,
		World,
		Client->CharacterEntity,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_WAYPOINTS) {
	if (!Character) goto error;

	Bool Success = RTCharacterMovementChangeWaypoints(
		Runtime,
		Character,
		Packet->PositionCurrent.X,
		Packet->PositionCurrent.Y,
		Packet->PositionNext.X,
		Packet->PositionNext.Y
	);
	if (!Success) goto error;

	return;

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MOVEMENT_TILE_POSITION) {
	if (!Character) goto error;

	Bool Success = RTCharacterMovementChangePosition(
		Runtime,
		Character,
		Packet->PositionNext.X,
		Packet->PositionNext.Y
	);
	if (!Success) goto error;

	return;

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CHANGE_DIRECTION) {
	if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	S2C_DATA_CHANGE_DIRECTION* Notification = PacketInit(S2C_DATA_CHANGE_DIRECTION);
	Notification->Command = S2C_CHANGE_DIRECTION;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->Direction = Packet->Direction;

	return BroadcastToWorld(
		Server,
		World,
		Client->CharacterEntity,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(KEY_MOVEMENT_BEGIN) {
	if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementBegin(
		Runtime,
		Character,
		Character->Info.Position.WorldID,
		(Int32)Packet->PositionStartX,
		(Int32)Packet->PositionStartY,
		(Int32)Packet->PositionCurrentX,
		(Int32)Packet->PositionCurrentY,
		(Int32)Packet->PositionEndX,
		(Int32)Packet->PositionEndY
	);
	if (!Success) goto error;

	S2C_DATA_KEY_MOVEMENT_BEGIN* Notification = PacketInit(S2C_DATA_KEY_MOVEMENT_BEGIN);
	Notification->Command = S2C_KEY_MOVEMENT_BEGIN;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionStartX = Packet->PositionStartX;
	Notification->PositionStartY = Packet->PositionStartY;
	Notification->PositionEndX = Packet->PositionEndX;
	Notification->PositionEndY = Packet->PositionEndY;
	Notification->Direction = Packet->Direction;

	return BroadcastToWorld(
		Server,
		World,
		Client->CharacterEntity,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(KEY_MOVEMENT_END) {
	if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementEnd(
		Runtime,
		Character,
		(Int32)Packet->PositionCurrentX,
		(Int32)Packet->PositionCurrentY
	);
	if (!Success) goto error;

	S2C_DATA_KEY_MOVEMENT_END* Notification = PacketInit(S2C_DATA_KEY_MOVEMENT_END);
	Notification->Command = S2C_KEY_MOVEMENT_END;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->PositionCurrentX = Packet->PositionCurrentX;
	Notification->PositionCurrentY = Packet->PositionCurrentY;

	LogMessageFormat(
		LOG_LEVEL_INFO,
		"(%d, %d)",
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	return BroadcastToWorld(
		Server,
		World,
		Client->CharacterEntity,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(KEY_MOVEMENT_CHANGE) {
	if (!Character) goto error;

    RTWorldRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	Bool Success = RTCharacterMovementChange(
		Runtime,
		Character,
		Character->Info.Position.WorldID,
		(Int32)Packet->PositionStartX,
		(Int32)Packet->PositionStartY,
		(Int32)Packet->PositionCurrentX,
		(Int32)Packet->PositionCurrentY,
		(Int32)Packet->PositionEndX,
		(Int32)Packet->PositionEndY
	);
	if (!Success) goto error;

	S2C_DATA_KEY_MOVEMENT_CHANGE* Notification = PacketInit(S2C_DATA_KEY_MOVEMENT_CHANGE);
	Notification->Command = S2C_KEY_MOVEMENT_CHANGE;
	Notification->CharacterIndex = Client->CharacterIndex;
	Notification->TickCount = Character->Movement.TickCount;
	Notification->PositionStartX = Packet->PositionStartX;
	Notification->PositionStartY = Packet->PositionStartY;
	Notification->PositionEndX = Packet->PositionEndX;
	Notification->PositionEndY = Packet->PositionEndY;
	Notification->Direction = Packet->Direction;

	return BroadcastToWorld(
		Server,
		World,
		Client->CharacterEntity,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}
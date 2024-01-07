#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHANGE_CHARACTER_SKILL_LEVEL) {
	if (!Character) goto error;

	Bool Success = RTCharacterChangeSkillLevel(
		Runtime,
		Character,
		Packet->SkillIndex,
		Packet->SlotIndex,
		Packet->CurrentSkillLevel,
		Packet->TargetSkillLevel
	);
	if (!Success) goto error;

	S2C_DATA_CHANGE_CHARACTER_SKILL_LEVEL* Response = PacketInit(S2C_DATA_CHANGE_CHARACTER_SKILL_LEVEL);
    Response->Command = S2C_CHANGE_CHARACTER_SKILL_LEVEL;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

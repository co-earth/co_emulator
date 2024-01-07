#include "IPCProtocol.h"
#include "Notification.h"

Void BroadcastWorldList(
    ServerContextRef Server
) {
	IPC_DATA_AUTH_NFYWORLDLIST* Notification = PacketInit(IPC_DATA_AUTH_NFYWORLDLIST);
	Notification->Command = IPC_AUTH_NFYWORLDLIST;
	Notification->WorldCount = 0;

	for (Int32 Index = 0; Index < Server->WorldSocket->Connections.Count; Index++) {
		SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
			&Server->WorldSocket->Connections,
			Index
		);
		ClientContextRef Client = (ClientContextRef)Connection->Userdata;

		if ((Client->Flags & CLIENT_FLAGS_WORLD_INITIALIZED)) {
			IPC_DATA_AUTH_WORLD* World = PacketAppendStruct(IPC_DATA_AUTH_WORLD);
			World->WorldID = Client->World.WorldID;
			memcpy(World->WorldHost, Client->World.WorldHost, sizeof(World->WorldHost));
			World->WorldPort = Client->World.WorldPort;
			World->WorldType = Client->World.WorldType;
			World->PlayerCount = Client->World.PlayerCount;
			World->MaxPlayerCount = Client->World.MaxPlayerCount;

			Notification->WorldCount += 1;
		}
	}

	SocketSendAll(Server->AuthSocket, Notification);
}

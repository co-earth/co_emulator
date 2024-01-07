#include "Socket.h"

Void SocketInitEncryptedServer(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
) {
    PlatformLoadSocketLibrary();

    Socket->Handle = -1;
    Socket->Flags = SOCKET_FLAGS_ENCRYPTED;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = 0;
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = OnSend;
    Socket->OnReceived = OnReceived;
    Socket->OnSendRaw = NULL;
    Socket->OnReceivedRaw = NULL;
    ArrayInitializeEmpty(&Socket->Connections, sizeof(struct _SocketConnection), MAX(0, MaxConnectionCount));
    Socket->Userdata = Userdata;
}

Void SocketInitEncryptedClient(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
) {
    PlatformLoadSocketLibrary();

    Socket->Handle = -1;
    Socket->Flags = SOCKET_FLAGS_ENCRYPTED | SOCKET_FLAGS_CLIENT;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = 0;
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = OnSend;
    Socket->OnReceived = OnReceived;
    Socket->OnSendRaw = NULL;
    Socket->OnReceivedRaw = NULL;
    ArrayInitializeEmpty(&Socket->Connections, sizeof(struct _SocketConnection), MAX(0, MaxConnectionCount));
    Socket->Userdata = Userdata;
}

Void SocketInitPlainServer(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
) {
    PlatformLoadSocketLibrary();

    Socket->Handle = -1;
    Socket->Flags = SOCKET_FLAGS_PLAIN;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = 0;
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = OnSend;
    Socket->OnReceived = OnReceived;
    Socket->OnSendRaw = NULL;
    Socket->OnReceivedRaw = NULL;
    ArrayInitializeEmpty(&Socket->Connections, sizeof(struct _SocketConnection), MAX(0, MaxConnectionCount));
    Socket->Userdata = Userdata;
}

Void SocketInitRaw(
    SocketRef Socket,
    Int32 MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketRawPacketCallback OnSend,
    SocketRawPacketCallback OnReceived,
    Void* Userdata
) {
    PlatformLoadSocketLibrary();

    Socket->Handle = -1;
    Socket->Flags = 0;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = 0;
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = NULL;
    Socket->OnReceived = NULL;
    Socket->OnSendRaw = OnSend;
    Socket->OnReceivedRaw = OnReceived;
    ArrayInitializeEmpty(&Socket->Connections, sizeof(struct _SocketConnection), MAX(0, MaxConnectionCount));
    Socket->Userdata = Userdata;
}

Void SocketConnect(
    SocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
) {
    assert(!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED)));

    Socket->Timeout = Timeout;
    Socket->Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket->Handle < 0) FatalError("Socket creation failed");

    PlatformSetSocketIOBlocking(Socket->Handle, false);

    struct hostent* Server = (struct hostent*)gethostbyname(Host);
    if (!Server) FatalError("Socket creation failed");

    memset(&Socket->Address, 0, sizeof(Socket->Address));
    Socket->Address.sin_family = AF_INET;

    memmove(&Socket->Address.sin_addr.s_addr, Server->h_addr, Server->h_length);
    Socket->Address.sin_port = htons(Port);

    Socket->Flags |= SOCKET_FLAGS_CONNECTING;

    LogMessageFormat(LOG_LEVEL_INFO, "Socket connecting to port: %d", Port);

    Int32 Result = PlatformSocketConnect(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address));
    if (Result == 0) {
        // TODO: Add remote address to `Connection`!
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayAppendUninitializedElement(&Socket->Connections);
        memset(Connection, 0, sizeof(struct _SocketConnection));
        Connection->ID = Socket->NextConnectionID;
        Connection->Handle = Socket->Handle;
        Socket->NextConnectionID += 1;
        Socket->Flags &= ~SOCKET_FLAGS_CONNECTING;
        Socket->Flags |= SOCKET_FLAGS_CONNECTED;
        Socket->OnConnect(Socket, Connection);
        LogMessage(LOG_LEVEL_INFO, "Socket connection established");
    } else if (Result == -1) {
        FatalError("Socket connection failed");
    }
}

Void SocketListen(
    SocketRef Socket,
    UInt16 Port
) {
    assert(!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED)));

    Socket->Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket->Handle < 0) FatalError("Socket creation failed");

    PlatformSetSocketIOBlocking(Socket->Handle, false);

    Socket->Address.sin_family = AF_INET;
    Socket->Address.sin_addr.s_addr = htonl(INADDR_ANY);
    Socket->Address.sin_port = htons(Port);

    if ((bind(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address))) != 0)
        FatalError("Socket binding failed");

    if ((listen(Socket->Handle, Socket->MaxConnectionCount)) != 0)
        FatalError("Socket listening failed");

    Socket->Flags |= SOCKET_FLAGS_LISTENING;

    LogMessageFormat(LOG_LEVEL_INFO, "Socket started listening on port: %d", Port);
}

Void SocketSendRaw(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt8 *Data,
    Int32 Length
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    assert(Connection->WriteBufferOffset + Length <= SOCKET_MAX_PACKET_SIZE);
    memcpy(&Connection->WriteBuffer[Connection->WriteBufferOffset], Data, Length);
    Connection->WriteBufferOffset += Length;
}

Void SocketSendAllRaw(
    SocketRef Socket,
    UInt8* Data,
    Int32 Length
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    for (Int32 Index = 0; Index < Socket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(&Socket->Connections, Index);
        SocketSendRaw(Socket, Connection, Data, Length);
    }
}

Void SocketSend(
    SocketRef Socket,
    SocketConnectionRef Connection,
    PacketRef Packet
) {
    SocketSendRaw(Socket, Connection, Packet, Packet->Length);
}

Void SocketSendAll(
    SocketRef Socket,
    PacketRef Packet
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    for (Int32 Index = 0; Index < Socket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(&Socket->Connections, Index);
        SocketSend(Socket, Connection, Packet, Packet->Length);
    }
}

Bool SocketFetchReadBuffer(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED || Socket->Flags & SOCKET_FLAGS_PLAIN) {
        while (Connection->ReadBufferOffset >= sizeof(struct _PacketSignature)) {
            Int32 PacketLength = 0;

            if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                PacketLength = KeychainGetPacketLength(
                    &Connection->Keychain,
                    Connection->ReadBuffer,
                    Connection->ReadBufferOffset
                );
            }
            else {
                PacketRef Signature = (PacketRef)Connection->ReadBuffer;
                PacketLength = Signature->Length;
            }

            if (Connection->ReadBufferOffset >= PacketLength) {
                if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                    KeychainDecryptPacket(
                        &Connection->Keychain,
                        Connection->ReadBuffer,
                        PacketLength
                    );
                }

                if (Socket->OnReceived)
                    Socket->OnReceived(Socket, Connection, Connection->ReadBuffer);

                Connection->ReadBufferOffset -= PacketLength;
                if (Connection->ReadBufferOffset > 0) {
                    memmove(&Connection->ReadBuffer[0], &Connection->ReadBuffer[PacketLength], Connection->ReadBufferOffset);
                }
            }
        }
    }
    else {
        while (Connection->ReadBufferOffset > 0) {
            assert(Socket->OnReceivedRaw);
            Int32 ReadLength = Socket->OnReceivedRaw(
                Socket,
                Connection,
                Connection->ReadBuffer,
                Connection->ReadBufferOffset
            );

            if (ReadLength > 0) {
                assert(ReadLength <= Connection->ReadBufferOffset);

                Connection->ReadBufferOffset -= ReadLength;
                if (Connection->ReadBufferOffset > 0) {
                    memmove(&Connection->ReadBuffer[0], &Connection->ReadBuffer[ReadLength], Connection->ReadBufferOffset);
                }
            }
            else {
                break;
            }
        }
    }
}

Bool SocketFlushWriteBuffer(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED || Socket->Flags & SOCKET_FLAGS_PLAIN) {
        while (Connection->WriteBufferOffset > 0) {
            PacketRef Packet = (PacketRef)Connection->WriteBuffer;
            Int32 PacketLength = Packet->Length;

            if (Socket->OnSend)
                Socket->OnSend(Socket, Connection, Packet);

            if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                KeychainEncryptPacket(&Connection->Keychain, Packet, PacketLength);
            }

            if (send(Connection->Handle, (Char*)Packet, PacketLength, 0) == -1) {
                SocketDisconnect(Socket, Connection);
                break;
            }
            else {
                Connection->WriteBufferOffset -= PacketLength;
                if (Connection->WriteBufferOffset > 0) {
                    memmove(
                        &Connection->WriteBuffer[0],
                        &Connection->WriteBuffer[PacketLength],
                        Connection->WriteBufferOffset
                    );
                }
            }
        }
    }
    else {
        while (Connection->WriteBufferOffset > 0) {
            assert(Socket->OnSendRaw);
            Int32 WriteLength = Socket->OnSendRaw(
                Socket,
                Connection,
                Connection->WriteBuffer,
                Connection->WriteBufferOffset
            );

            if (WriteLength > 0) {
                if (send(Connection->Handle, (Char*)Connection->WriteBuffer, WriteLength, 0) == -1) {
                    SocketDisconnect(Socket, Connection);
                    break;
                }
                else {
                    Connection->WriteBufferOffset -= WriteLength;
                    if (Connection->WriteBufferOffset > 0) {
                        memmove(
                            &Connection->WriteBuffer[0],
                            &Connection->WriteBuffer[WriteLength],
                            Connection->WriteBufferOffset
                        );
                    }
                }
            }
            else {
                break;
            }
        }
    }
}

Void SocketUpdate(
    SocketRef Socket
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED))) return;

    if (Socket->Flags & SOCKET_FLAGS_LISTENING) {
        if (Socket->Connections.Count < Socket->MaxConnectionCount || Socket->MaxConnectionCount < 0) {
            SocketAddress ClientAddress;
            SocketHandle Client;
            if (PlatformSocketAccept(Socket->Handle, (struct sockaddr*)&ClientAddress, sizeof(ClientAddress), &Client)) {
                SocketConnectionRef Connection = (SocketConnectionRef)ArrayAppendUninitializedElement(&Socket->Connections);
                memset(Connection, 0, sizeof(struct _SocketConnection));
                Connection->ID = Socket->NextConnectionID;
                Connection->Handle = Client;
                Connection->Address = ClientAddress;
                PlatformSocketAddressToString(ClientAddress, sizeof(Connection->AddressIP), Connection->AddressIP);

                if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                    KeychainInit(&Connection->Keychain, (Socket->Flags & SOCKET_FLAGS_CLIENT) > 0);
                }

                Socket->NextConnectionID += 1;
                Socket->OnConnect(Socket, Connection);
            }
        }
    }

    if ((Socket->Flags & SOCKET_FLAGS_CONNECTING) && !(Socket->Flags & SOCKET_FLAGS_CONNECTED)) {
        if (PlatformSocketSelect(Socket->Handle, Socket->Timeout)) {
            SocketConnectionRef Connection = (SocketConnectionRef)ArrayAppendUninitializedElement(&Socket->Connections);
            memset(Connection, 0, sizeof(struct _SocketConnection));
            Connection->ID = Socket->NextConnectionID;
            Connection->Handle = Socket->Handle;

            if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                KeychainInit(&Connection->Keychain, (Socket->Flags & SOCKET_FLAGS_CLIENT) > 0);
            }

            Socket->NextConnectionID += 1;
            Socket->Flags &= ~SOCKET_FLAGS_CONNECTING;
            Socket->Flags |= SOCKET_FLAGS_CONNECTED;
            Socket->OnConnect(Socket, Connection);
            LogMessage(LOG_LEVEL_INFO, "Socket connection established");
        } else {
            PlatformSocketConnect(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address));
        }

        return;
    }

    for (Int32 Index = 0; Index < Socket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(&Socket->Connections, Index);
        SocketFlushWriteBuffer(Socket, Connection);

        Int32 ReadLength;
        Bool Success = PlatformSocketRecv(
            Connection->Handle,
            &Connection->ReadBuffer[Connection->ReadBufferOffset],
            SOCKET_MAX_PACKET_SIZE - Connection->ReadBufferOffset,
            &ReadLength
        );
        if (!Success) {
            SocketDisconnect(Socket, Connection);
            continue;
        }
        
        Connection->ReadBufferOffset += ReadLength;
        SocketFetchReadBuffer(Socket, Connection);
    }

    for (Int32 Index = Socket->Connections.Count - 1; Index >= 0; Index--) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(&Socket->Connections, Index);
        if (Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED &&
            !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED_END)) {
            Connection->Flags &= ~SOCKET_CONNECTION_FLAGS_DISCONNECTED_END;
            SocketFlushWriteBuffer(Socket, Connection);
            Socket->OnDisconnect(Socket, Connection);
            Socket->Flags &= ~SOCKET_FLAGS_CONNECTED;
            PlatformSocketClose(Connection->Handle);
            ArrayRemoveElementAtIndex(&Socket->Connections, Index);
        }
    }
}

Void SocketDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    Connection->Flags |= SOCKET_CONNECTION_FLAGS_DISCONNECTED;
}

Void SocketClose(
    SocketRef Socket
) {
    for (Int32 Index = 0; Index < Socket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(&Socket->Connections, Index);
        if (Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED &&
            !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED_END)) {
            Connection->Flags &= ~SOCKET_CONNECTION_FLAGS_DISCONNECTED_END;
            SocketFlushWriteBuffer(Socket, Connection);
            Socket->OnDisconnect(Socket, Connection);
            PlatformSocketClose(Connection->Handle);
        }
    }

    if (Socket->Handle >= 0) {
        PlatformSocketClose(Socket->Handle);
        Socket->Handle = -1;
    }

    Socket->Flags = 0;
    ArrayDeinitialize(&Socket->Connections);
}

SocketConnectionRef SocketGetConnection(
    SocketRef Socket,
    Int32 ConnectionID
) {
    for (Int32 Index = 0; Index < Socket->Connections.Count; Index++) {
        SocketConnectionRef Connection = (SocketConnectionRef)ArrayGetElementAtIndex(
            &Socket->Connections,
            Index
        );
        if (Connection->ID == ConnectionID) return Connection;
    }

    return NULL;
}

Void SocketConnectionKeychainSeed(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt32 Key,
    UInt32 Step
) {
    assert(Socket->Flags & SOCKET_FLAGS_ENCRYPTED);
    KeychainSeed(&Connection->Keychain, Key, Step);
}

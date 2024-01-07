#include "Socket.h"

static Bool kIsSocketLibraryLoaded = false;

// TODO: WSACleanup(); is missing!
Void PlatformLoadSocketLibrary() {
    if (kIsSocketLibraryLoaded) return;

    kIsSocketLibraryLoaded = true;

#ifdef _WIN32
    WSADATA WSAData = { 0 };
    Int32 Result = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (Result != 0) {
        Char MessageBuffer[50] = { 0 };

        if (!GetPlatformErrorMessage(MessageBuffer, sizeof(MessageBuffer)))
            strcpy(MessageBuffer, "Socket library loading failed!");

        FatalError(MessageBuffer);
    }
#endif
}

Bool PlatformSetSocketIOBlocking(
    SocketHandle Handle,
    Bool Blocking
) {
   if (Handle < 0) return false;

#ifdef _WIN32
   u_long Mode = Blocking ? 0 : 1;
   return (ioctlsocket(Handle, FIONBIO, &Mode) == 0) ? true : false;
#else
   int Flags = fcntl(Handle, F_GETFL, 0);
   if (Flags == -1) return false;
    Flags = Blocking ? (Flags & ~O_NONBLOCK) : (Flags | O_NONBLOCK);
   return (fcntl(Handle, F_SETFL, Flags) == 0) ? true : false;
#endif
}

Bool PlatformSocketAccept(
    SocketHandle Handle,
    const struct sockaddr* Address,
    Int32 Length,
    SocketHandle* Result
) {
    SocketHandle Client = accept(Handle, Address, &Length);
#if _WIN32
    if (Client == INVALID_SOCKET) return false;
#else
    if (Client == -1) return false;
#endif

    *Result = Client;
    return true;
}

Int32 PlatformSocketConnect(
    SocketHandle Handle,
    const struct sockaddr* Address,
    Int32 Length
) {
    Int32 Result = connect(Handle, Address, Length);
    if (Result == 0) return 0;

#if _WIN32
    return (Result == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) ? 1 : -1;
#else
    return (Result == -1 && errno == EINPROGRESS) ? 1 : -1;
#endif
}

Bool PlatformSocketSelect(
    SocketHandle Handle,
    Timestamp Timeout
) {
    fd_set Write = { 1, Handle };
    fd_set Except = { 1, Handle };
    struct timeval TimeoutInterval;
    TimeoutInterval.tv_sec = Timeout / 1000;
    TimeoutInterval.tv_usec = (Timeout % 1000) * 1000;
    Int32 Result = select(0, NULL, &Write, &Except, &TimeoutInterval);

#if _WIN32
    if (Result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) return false;
#else
    if (Result == -1 && errno != EINPROGRESS) return false;
#endif

    if (FD_ISSET(Handle, &Except)) {
        Char Message[0xFF];
        if (GetPlatformErrorMessage(Message, 0xFF)) {
            LogMessage(LOG_LEVEL_ERROR, Message);
        }

        return false;
    }

    return FD_ISSET(Handle, &Write);
}

Bool PlatformSocketRecv(
    SocketHandle Handle,
    UInt8* Buffer,
    Int32 Length,
    Int32* ReadLength
) {
    Int32 Result = recv(Handle, (Char*)Buffer, Length, 0);

#if _WIN32
    if (Result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) return false;
#else
    if (Result == -1 && errno != EINPROGRESS) return false;
#endif

    *ReadLength = (Result > 0) ? Result : 0;
    return true;
}

Void PlatformSocketClose(
    SocketHandle Handle
) {
#if _WIN32
    closesocket(Handle);
#else
    close(Handle);
#endif
}

Void PlatformSocketAddressToString(
    SocketAddress Address,
    Int32 Length,
    CString Buffer
) {
    Char* Result = inet_ntoa(Address.sin_addr);
    strcpy_s(Buffer, Length, Result);
}
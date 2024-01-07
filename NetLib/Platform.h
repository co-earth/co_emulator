#pragma once

#include "Base.h"

#ifdef _WIN32
    #include <winsock.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

#ifdef _WIN32
typedef SOCKET SocketHandle;
#else
typedef Int32 SocketHandle;
#endif

typedef struct sockaddr_in SocketAddress;

Void PlatformLoadSocketLibrary();

Bool PlatformSetSocketIOBlocking(
    SocketHandle Handle,
    Bool Blocking
);

Bool PlatformSocketAccept(
    SocketHandle Handle,
    const struct sockaddr* Address,
    Int32 Length,
    SocketHandle* Result
);

Int32 PlatformSocketConnect(
    SocketHandle Handle,
    const struct sockaddr* Address,
    Int32 Length
);

Bool PlatformSocketSelect(
    SocketHandle Handle,
    Timestamp Timeout
);

Bool PlatformSocketRecv(
    SocketHandle Handle,
    UInt8* Buffer,
    Int32 Length,
    Int32* ReadLength
);

Void PlatformSocketClose(
    SocketHandle Handle
);

Void PlatformSocketAddressToString(
    SocketAddress Address,
    Int32 Length,
    CString Buffer
);
#ifndef C2S_COMMAND(__NAME__, __VALUE__) 
#define C2S_COMMAND(__NAME__, __VALUE__)
#endif

C2S_COMMAND(C2S_CONNECT, 101)
C2S_COMMAND(C2S_VERIFYLINKS, 102)
C2S_COMMAND(C2S_AUTHACCOUNT, 103)
C2S_COMMAND(C2S_DISCONNECT, 110)
C2S_COMMAND(C2S_CHECKVERSION, 122)
C2S_COMMAND(C2S_PUBLICKEY, 2001)
C2S_COMMAND(C2S_SERVERENV, 2002)
C2S_COMMAND(C2S_AUTHENTICATE, 2006)
C2S_COMMAND(C2S_UNKNOWN3383, 3383)
C2S_COMMAND(C2S_UNKNOWN5383, 5383)

#undef C2S_COMMAND

#ifndef S2C_COMMAND(__NAME__, __VALUE__) 
#define S2C_COMMAND(__NAME__, __VALUE__)
#endif

S2C_COMMAND(S2C_CONNECT, 101)
S2C_COMMAND(S2C_VERIFYLINKS, 102)
S2C_COMMAND(S2C_AUTHACCOUNT, 103)
S2C_COMMAND(S2C_SYSTEMMESSAGE, 120)
S2C_COMMAND(S2C_SERVERLIST, 121)
S2C_COMMAND(S2C_CHECKVERSION, 122)
S2C_COMMAND(S2C_UNKNOWN124, 124)
S2C_COMMAND(S2C_URLLIST, 128)
S2C_COMMAND(S2C_PUBLICKEY, 2001)
S2C_COMMAND(S2C_SERVERENV, 2002)
S2C_COMMAND(S2C_DCTIMER, 2005)
S2C_COMMAND(S2C_AUTHENTICATE, 2006)
S2C_COMMAND(S2C_AUTHTIMER, 2009)
S2C_COMMAND(S2C_UNKNOWN3383, 3383)
S2C_COMMAND(S2C_UNKNOWN5383, 5383)

#undef S2C_COMMAND
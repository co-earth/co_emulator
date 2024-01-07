#ifndef IPC_COMMAND(__NAME__, __VALUE__) 
#define IPC_COMMAND(__NAME__, __VALUE__)
#endif

IPC_COMMAND(IPC_AUTH_REQCONNECT, 1001)
IPC_COMMAND(IPC_AUTH_ACKCONNECT, 1002)
IPC_COMMAND(IPC_AUTH_NFYWORLDLIST, 1003)
IPC_COMMAND(IPC_AUTH_VERIFYLINKS, 1004)
IPC_COMMAND(IPC_AUTH_VERIFYRESULT, 1005)
IPC_COMMAND(IPC_AUTH_REQVERIFYPASSWORD, 1006)
IPC_COMMAND(IPC_AUTH_ACKVERIFYPASSWORD, 1007)

IPC_COMMAND(IPC_WORLD_REQCONNECT, 2001)
IPC_COMMAND(IPC_WORLD_ACKCONNECT, 2002)
IPC_COMMAND(IPC_WORLD_NFYUSERLIST, 2003)
IPC_COMMAND(IPC_WORLD_VERIFYLINKS, 2004)
IPC_COMMAND(IPC_WORLD_VERIFYRESULT, 2005)
IPC_COMMAND(IPC_WORLD_UPDATE_ACCOUNT_SESSION_DATA, 2006)
IPC_COMMAND(IPC_WORLD_UPDATE_ACCOUNT_CHARACTER_DATA, 2007)
IPC_COMMAND(IPC_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA, 2008)
IPC_COMMAND(IPC_WORLD_REQGETCHARACTERS, 2009)
IPC_COMMAND(IPC_WORLD_ACKGETCHARACTERS, 2010)
IPC_COMMAND(IPC_WORLD_REQPREMIUMSERVICE, 2011)
IPC_COMMAND(IPC_WORLD_ACKPREMIUMSERVICE, 2012)
IPC_COMMAND(IPC_WORLD_REQCREATECHARACTER, 2013)
IPC_COMMAND(IPC_WORLD_ACKCREATECHARACTER, 2014)
IPC_COMMAND(IPC_WORLD_REQGETCHARACTER, 2015)
IPC_COMMAND(IPC_WORLD_ACKGETCHARACTER, 2016)
IPC_COMMAND(IPC_WORLD_REQDBSYNC, 2017)
IPC_COMMAND(IPC_WORLD_ACKDBSYNC, 2018)
IPC_COMMAND(IPC_WORLD_REQVERIFYPASSWORD, 2019)
IPC_COMMAND(IPC_WORLD_ACKVERIFYPASSWORD, 2020)

#undef IPC_COMMAND
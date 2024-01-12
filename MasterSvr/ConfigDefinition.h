#ifndef CONFIG_BEGIN(__NAMESPACE__)
#define CONFIG_BEGIN(__NAMESPACE__)
#endif

#ifndef CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__)
#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__)
#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_END(__NAMESPACE__)
#define CONFIG_END(__NAMESPACE__)
#endif

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER(UInt8, ServerID, "MasterSvr.ServerID", 1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38009)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(MasterDB)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterDB.Host", 127.0.0.1)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Username, "MasterDB.Username", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Password, "MasterDB.Password", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Database, "MasterDB.Database", master)
CONFIG_PARAMETER(UInt16, Port, "MasterDB.Port", 3307)
CONFIG_END(MasterDB)

CONFIG_BEGIN(AuthSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "AuthSvr.Host", "127.0.0.1")
CONFIG_PARAMETER(UInt16, Port, "AuthSvr.Port", 38180)
CONFIG_END(AuthSvr)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END

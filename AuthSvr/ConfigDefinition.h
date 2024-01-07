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

CONFIG_BEGIN(Auth)
CONFIG_PARAMETER(Bool, Maintenance, "AuthSvr.Maintenance", 0)
CONFIG_PARAMETER(Bool, CheckVersion, "AuthSvr.CheckVersion", 1)
CONFIG_PARAMETER(UInt32, ClientVersion, "AuthSvr.ClientVersion", 596)
CONFIG_PARAMETER(UInt32, DebugVersion, "AuthSvr.DebugVersion", 0)
CONFIG_PARAMETER(UInt32, ClientMagicKey, "AuthSvr.ClientMagicKey", 701878038)
CONFIG_PARAMETER(UInt32, ServerMagicKey, "AuthSvr.ServerMagicKey", 5834620)
CONFIG_PARAMETER(UInt16, Port, "AuthSvr.Port", 38101)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "AuthSvr.MaxConnectionCount", -1)
CONFIG_PARAMETER(Bool, AutoDisconnect, "AuthSvr.AutoDisconnect", 1)
CONFIG_PARAMETER(UInt64, AutoDisconnectDelay, "AuthSvr.AutoDisconnectDelay", 120000)
CONFIG_END(Auth)

CONFIG_BEGIN(AuthDB)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "AuthDB.Host", "127.0.0.1")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Username, "AuthDB.Username", "root")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Password, "AuthDB.Password", "root")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Database, "AuthDB.Database", "auth")
CONFIG_PARAMETER(UInt16, Port, "AuthDB.Port", 3310)
CONFIG_END(AuthDB)

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38180)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(Links)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Itemshop, "Links.Itemshop", "http://127.0.0.1:8000")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown1, "Links.Unknown1", "http://127.0.0.1:8000")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown2, "Links.Unknown2", "http://127.0.0.1:8000")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown3, "Links.Unknown3", "http://127.0.0.1:8000")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Unknown4, "Links.Unknown4", "http://127.0.0.1:8000")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Guild, "Links.Guild", "http://127.0.0.1:8000")
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, SNS, "Links.SNS", "http://127.0.0.1:8000")
CONFIG_END(Links)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END

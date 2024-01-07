#pragma once

#include <NetLib/NetLib.h>
#include "Constants.h"
#include "GameProtocol.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define CLIENT_PROTOCOL_ENUM(...) \
enum {                            \
    __VA_ARGS__                   \
};
#include "ClientProtocolDefinition.h"

#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)      \
typedef struct {                                        \
    __BODY__                                            \
} __NAME__;
#include "ClientProtocolDefinition.h"

#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __COMMAND__, __VERSION__, __BODY__)    \
typedef struct {                                                                        \
    __BODY__                                                                            \
} __NAMESPACE__ ## _DATA_ ## __NAME__;
#include "ClientProtocolDefinition.h"

enum {
#define C2S_COMMAND(__NAME__, __VALUE__) \
	C2S_ ## __NAME__ = __VALUE__,
#include "ClientCommands.h"
};

enum {
#define S2C_COMMAND(__NAME__, __VALUE__) \
	S2C_ ## __NAME__ = __VALUE__,
#include "ClientCommands.h"
};

#pragma pack(pop)

EXTERN_C_END

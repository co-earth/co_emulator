#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#define CONFIG_BEGIN(__NAMESPACE__) \
	struct _Config##__NAMESPACE__ {

#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__) \
	__TYPE__ __NAME__;

#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__) \
	__TYPE__ __NAME__[__LENGTH__];

#define CONFIG_END(__NAMESPACE__) \
	};

#include "ConfigDefinition.h"

struct _Config {
#define CONFIG_BEGIN(__NAMESPACE__) \
	struct CONCAT(_Config, __NAMESPACE__) __NAMESPACE__;

#include "ConfigDefinition.h"
};
typedef struct _Config* ConfigRef;

Bool LoadConfig(
	ConfigRef Config,
	CString Filepath
);

EXTERN_C_END
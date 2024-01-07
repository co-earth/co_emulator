#pragma once

#include "Base.h"
#include "Definitions.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTRuntimeDataContext {
#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__) \
	Int32 CONCAT(__NAME__, Count);									\
	struct CONCAT(_RTData, __NAME__) CONCAT(__NAME__, List)[__COUNT__];

#include "Macro.h"
};
typedef struct _RTRuntimeDataContext* RTRuntimeDataContextRef;

#pragma pack(pop)

RTRuntimeDataContextRef RTRuntimeDataContextCreate();

Void RTRuntimeDataContextDestroy(
	RTRuntimeDataContextRef Context
);

Bool RTRuntimeDataContextLoad(
	RTRuntimeDataContextRef Context,
	CString Directory
);

#define RUNTIME_DATA_TYPE_INDEX(__NAME__, __TYPE__, __FIELD__) \
CONCAT(RTData, __NAME__)Ref CONCAT(RTRuntimeData, __NAME__)Get( \
	RTRuntimeDataContextRef Context, \
	__TYPE__ __FIELD__ \
);

#include "Macro.h"

Int32 RTRuntimeDataCharacterRankUpConditionGet(
	RTRuntimeDataContextRef Context,
	Int32 Rank,
	Int32 BattleStyleIndex
);

EXTERN_C_END
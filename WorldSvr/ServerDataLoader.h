#pragma once

#include "Context.h"
#include "Constants.h"
#include "GameProtocol.h"

EXTERN_C_BEGIN

Bool ServerLoadCharacterTemplateData(
    ServerContextRef Server
);

Bool ServerLoadBattleStyleFormulaData(
    ServerContextRef Server
);

Bool ServerLoadQuestData(
    ServerContextRef Server
);

Bool ServerLoadWarpData(
    ServerContextRef Server
);

Bool ServerLoadLevelData(
    ServerContextRef Server
);

Bool ServerLoadItemData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadMobData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadShopData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadUpgradeData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadWorldData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory,
    Bool LoadShops
);

Bool ServerLoadSkillData(
    ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
);

Bool ServerLoadDungeonData(
	ServerContextRef Server,
    CString RuntimeDirectory,
    CString ServerDirectory
);

EXTERN_C_END
#ifndef RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__)
#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__)
#endif

#ifndef RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__, __COUNT__)
#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__, __COUNT__)
#endif

#ifndef RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__)
#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__)
#endif

#ifndef RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__)
#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__)
#endif

#ifndef RUNTIME_DATA_TYPE_END_CHILD(__NAME__, __COUNT__)
#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__, __COUNT__)
#endif

#ifndef RUNTIME_DATA_TYPE_END(__NAME__)
#define RUNTIME_DATA_TYPE_END(__NAME__)
#endif

#ifndef RUNTIME_DATA_TYPE_INDEX(__NAME__, __TYPE__, __FIELD__)
#define RUNTIME_DATA_TYPE_INDEX(__NAME__, __TYPE__, __FIELD__)
#endif

#include "Ability.h"
#include "Achievement.h"
#include "Assistant.h"
#include "BattleStyleChange.h"
#include "BossArena.h"
#include "BossResistList.h"
#include "CashShop.h"
#include "ChallengeMissionData.h"
#include "ChangeShape.h"
#include "ChaosUpgrade.h"
#include "ChatBaloon.h"
#include "CheckItemAbility.h"
#include "Collection.h"
#include "Craft.h"
#include "Destroy.h"
#include "DivineUpgrade.h"
#include "DropEffect.h"
#include "DungeonTimeLimit.h"
#include "Effector.h"
#include "Enchant.h"
#include "EnvironmentSettings.h"
#include "ExtraObject.h"
#include "ExtremeUpgrade.h"
#include "FieldWar.h"
#include "ForceCoreOption.h"
#include "ForceWing.h"
#include "Giftbox.h"
#include "GuildDungeon.h"
#include "GuildTreasure.h"
#include "HonorMedal.h"
#include "Item.h"
#include "Keymap.h"
#include "Main.h"
#include "MapInfo.h"
#include "Market.h"
#include "MasterCraft.h"
#include "MeritSystem.h"
#include "MissionBattle.h"
#include "MissionDungeon.h"
#include "Mob.h"
#include "MobSkill.h"
#include "OverlordMastery.h"
#include "Pet.h"
#include "PVPBattle.h"
#include "Quest.h"
#include "QuestDungeon.h"
#include "RandomWarp.h"
#include "Rank.h"
#include "Request.h"
#include "SecretShop.h"
#include "SetEffect.h"
#include "Skill.h"
#include "SkillEnchant.h"
#include "Title.h"
#include "Transform.h"
#include "WarAdvantage.h"

#undef RUNTIME_DATA_TYPE_BEGIN
#undef RUNTIME_DATA_TYPE_BEGIN_CHILD
#undef RUNTIME_DATA_PROPERTY
#undef RUNTIME_DATA_PROPERTY_ARRAY
#undef RUNTIME_DATA_TYPE_END_CHILD
#undef RUNTIME_DATA_TYPE_END
#undef RUNTIME_DATA_TYPE_INDEX
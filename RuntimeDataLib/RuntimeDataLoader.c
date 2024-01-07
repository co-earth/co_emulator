#include "RuntimeDataLoader.h"

RTRuntimeDataContextRef RTRuntimeDataContextCreate() {
	RTRuntimeDataContextRef Context = malloc(sizeof(struct _RTRuntimeDataContext));
	if (!Context) FatalError("Memory allocation failed!");

	memset(Context, 0, sizeof(struct _RTRuntimeDataContext));
	return Context;
}

Void RTRuntimeDataContextDestroy(
	RTRuntimeDataContextRef Context
) {
	assert(Context);
	free(Context);
}

Bool RTRuntimeDataContextLoadArchive(
    RTRuntimeDataContextRef Context,
    Archive* Archive
) {
    ArchiveIterator* Iterator = NULL;

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__) \
    { \
        CString Query = __QUERY__; \
        CString PropertyQuery = NULL; \
        Iterator = ArchiveQueryNodeIteratorByPathFirst(Archive, -1, Query); \
        while (Iterator) { \
            assert(Context->CONCAT(__NAME__, Count) < __COUNT__); \
            CONCAT(RTData, __NAME__)Ref Data = &Context->CONCAT(__NAME__, List)[Context->CONCAT(__NAME__, Count)];

#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__) \
            PropertyQuery = __QUERY__; \
            if (!CONCAT(ParseAttribute, __TYPE__)(Archive, Iterator->Index, PropertyQuery, &Data->__NAME__)) { \
                LogMessageFormat(LOG_LEVEL_ERROR, "Couldn't parse attribute %s in %s\n", PropertyQuery, Query); \
                goto error; \
            }

#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__) \
            PropertyQuery = __QUERY__; \
            if (!CONCAT(ParseAttribute, __TYPE__)Array(Archive, Iterator->Index, PropertyQuery, Data->__NAME__, __COUNT__, __SEPARATOR__)) { \
                LogMessageFormat(LOG_LEVEL_ERROR, "Couldn't parse attribute %s in %s\n", PropertyQuery, Query); \
                goto error; \
            }

#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__, __COUNT__) \
            { \
                CString Query = __QUERY__; \
                ArchiveIterator* ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, Query); \
                while (ChildIterator) { \
                    ArchiveIterator* Iterator = ChildIterator; \
                    assert(Data->CONCAT(__NAME__, Count) < __COUNT__); \
                    CONCAT(RTData, __NAME__)Ref ChildData = &Data->CONCAT(__NAME__, List)[Data->CONCAT(__NAME__, Count)]; \
                    { \
                        CONCAT(RTData, __NAME__)Ref Data = ChildData;
    
#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__) \
                    } \
                    Data->CONCAT(__NAME__, Count) += 1; \
                    ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator); \
                } \
            }

#define RUNTIME_DATA_TYPE_END(__NAME__) \
           Context->CONCAT(__NAME__, Count) += 1; \
           Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator); \
        } \
    }

#include "Macro.h"

    return true;

error:
    return false;
}

Bool RTRuntimeDataContextLoad(
	RTRuntimeDataContextRef Context,
	CString Directory
) {
    assert(Context);

    const CString RuntimeDataFileList[] = {
        "ability.enc",
        "achievement.enc",
        "assistant.enc",
        "Awaken_auramode.enc",
        "BattleStyleChange.enc",
        "boss_resistlist.enc",
        "BossArena.enc",
        "cabal.enc",
        "caz.enc",
        "ChallengeMissionData.enc",
        "change_shape.enc",
        "chaos_upgrade.enc",
        "chat_balloon.enc",
        "checkAB.enc",
        "Collection.enc",
        "cont.enc",
        "cont2.enc",
        "cont3.enc",
        "craft.enc",
        "custom.enc",
        "destroy.enc",
        "divineupgrade.enc",
        "DropEffect.enc",
        "dungeon_timelimit.enc",
        "effecter.enc",
        "Enchant.enc",
        "EnvironmentSetting.enc",
        "event_enchant.enc",
        "extra_obj.enc",
        "FieldBossRaid.enc",
        // "fieldwar.enc",
        "forcecore_option.enc",
        "ForceWing.enc",
        "guide.enc",
        "GuildDungeon.enc",
        "GuildTreasure.enc",
        "honor_medal.enc",
        "keymap.enc",
        "limit_upgrade.enc",
        "mapinfo.enc",
        "market.enc",
        "meister.enc",
        "meritsystem.enc",
        "missionbattle.enc",
        "new_assistant.enc",
        "NPCShop.enc",
        "overloadmastery.enc",
        "pet.enc",
        "pvpbattle.enc",
        "quest.enc",
        "RandomWarp.enc",
        "rank.enc",
        "request.enc",
        "secretshop.enc",
        "seteffect.enc",
        "Skill_Enhanced.enc",
        "skill.enc",
        "smob.enc",
        "SpecialBox.enc",
        "title.enc",
        "Transform.enc",
        "WarAdventage.enc",
    };
    Int32 RuntimeDataFileCount = sizeof(RuntimeDataFileList) / sizeof(RuntimeDataFileList[0]);

    Archive ArchiveData;
    Archive* Archive = &ArchiveData;
    ArchiveLoadEmpty(Archive);

    for (Int32 FileIndex = 0; FileIndex < RuntimeDataFileCount; FileIndex += 1) {
        CString FileName = RuntimeDataFileList[FileIndex];

        LogMessageFormat(LOG_LEVEL_INFO, "Loading runtime data: %s", FileName);

        Bool IgnoreErrors = true;
        Bool Success = ArchiveLoadFromFileEncryptedNoAlloc(
            Archive,
            PathCombineNoAlloc(Directory, FileName),
            IgnoreErrors
        );
        if (!Success) {
            LogMessageFormat(LOG_LEVEL_ERROR, "Error loading runtime data: %s", FileName);
            goto error;
        }

        /*
        if (!RTRuntimeDataContextLoadArchive(Context, Archive)) {
            ArchiveUnload(Archive);
            return false;
        }
        */
        RTRuntimeDataContextLoadArchive(Context, Archive);

        Bool KeepCapacity = true;
        ArchiveClear(Archive, KeepCapacity);
    }

    ArchiveUnload(Archive);

    return true;

error:
    return false;
}

#define RUNTIME_DATA_TYPE_INDEX(__NAME__, __TYPE__, __FIELD__) \
CONCAT(RTData, __NAME__)Ref CONCAT(RTRuntimeData, __NAME__)Get( \
	RTRuntimeDataContextRef Context, \
	__TYPE__ __FIELD__ \
) { \
    for (Int32 Index = 0; Index < Context->CONCAT(__NAME__, Count); Index++) { \
        CONCAT(RTData, __NAME__)Ref Data = &Context->CONCAT(__NAME__, List)[Index]; \
        if (Data->__FIELD__ == __FIELD__) { \
            return Data; \
        } \
    } \
 \
    return NULL; \
}

#include "Macro.h"

Int32 RTRuntimeDataCharacterRankUpConditionGet(
    RTRuntimeDataContextRef Context,
    Int32 Rank,
    Int32 BattleStyleIndex
) {
    for (Int32 RankIndex = 0; RankIndex < Context->CharacterRankUpConditionCount; RankIndex += 1) {
        if (Context->CharacterRankUpConditionList[RankIndex].Rank != Rank) continue;

        assert(Context->CharacterRankUpConditionList[RankIndex].ConditionCount == 1);

        switch (BattleStyleIndex) {
        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].WA;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].BL;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].WI;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FA;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FS;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FB;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].GL;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].FG;

        case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM:
            return Context->CharacterRankUpConditionList[RankIndex].ConditionList[0].DM;

        default:
            FatalError("Given BattleStyleIndex doesn't exist!");
            return 0;
        }
    }
}
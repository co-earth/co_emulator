
RUNTIME_DATA_TYPE_BEGIN(Assistant, "assistant.main.assistant_main", 128)
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, CardItem, "card_item")
	RUNTIME_DATA_PROPERTY(Int32, Category, "category")
	RUNTIME_DATA_PROPERTY(Int32, Group, "group")
	RUNTIME_DATA_PROPERTY(Int32, Level, "level")
	RUNTIME_DATA_PROPERTY(Int32, Duration, "duration")
	RUNTIME_DATA_PROPERTY(Int32, RequestLevel, "req_lv")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(AssistantAbility, "ability", 4)
		RUNTIME_DATA_PROPERTY(Int32, AttackUse, "attack_use")
		RUNTIME_DATA_PROPERTY(Int32, PassiveSingle, "passive_single")
		RUNTIME_DATA_PROPERTY(Int32, PassiveParty, "passive_party")
		RUNTIME_DATA_PROPERTY(Int32, TriggerSingle, "trigger_single")
		RUNTIME_DATA_PROPERTY(Int32, ActiveParty, "active_party")
	RUNTIME_DATA_TYPE_END_CHILD(AssistantAbility, 4)
RUNTIME_DATA_TYPE_END(Assistant)

RUNTIME_DATA_TYPE_BEGIN(AssistantAttribute, "assistant.attribute.assistant_attribute", 128)
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, MovementSpeed, "move_speed")
	RUNTIME_DATA_PROPERTY(Int32, AnimationSpeed, "ani_speed")
	RUNTIME_DATA_PROPERTY(Int32, SocialID, "social_id")
RUNTIME_DATA_TYPE_END(AssistantAttribute)

RUNTIME_DATA_TYPE_BEGIN(AssistantAttack, "assistant.attack.assistant_attack", 128)
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, BaseValue, "base_value")
	RUNTIME_DATA_PROPERTY(Int32, AttackRate, "ar")
	RUNTIME_DATA_PROPERTY(Int32, CriticalRate, "critical_rate")
	RUNTIME_DATA_PROPERTY(Int32, CriticalDamage, "critical_damage")
RUNTIME_DATA_TYPE_END(AssistantAttack)

RUNTIME_DATA_TYPE_BEGIN(AssistantBuff, "assistant.buff.assistant_buff", 128)
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, BuffGroup, "buff_group")
	RUNTIME_DATA_PROPERTY(Int32, SkillLevel, "skill_lv")
	RUNTIME_DATA_PROPERTY(Int32, SkillType, "skill_type")
	RUNTIME_DATA_PROPERTY(Int32, TriggerValue, "trigger_value")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(AssistantBuffSkillValue, "skill_value", 8)
		RUNTIME_DATA_PROPERTY(Int32, Order, "order")
		RUNTIME_DATA_PROPERTY(Int32, ForceCodeID, "forcecode_id")
		RUNTIME_DATA_PROPERTY(Int32, Value, "value")
		RUNTIME_DATA_PROPERTY(Int32, LimitValue, "limit_value")
		RUNTIME_DATA_PROPERTY(Int32, DurationTime, "duration_time")
		RUNTIME_DATA_PROPERTY(Int32, ColldownTime, "cool_time")
	RUNTIME_DATA_TYPE_END_CHILD(AssistantBuffSkillValue, 8)
RUNTIME_DATA_TYPE_END(AssistantBuff)

RUNTIME_DATA_TYPE_BEGIN(AssistantItem, "assistant.item.assistant_lv", 8)
	RUNTIME_DATA_PROPERTY(Int32, Level, "lv")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(AssistantItemIndex, "item_index", 4)
		RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_TYPE_END_CHILD(AssistantItemIndex, 4)
RUNTIME_DATA_TYPE_END(AssistantItem)
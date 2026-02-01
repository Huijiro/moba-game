#include "ability_definition.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

#include "ability_effect.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::Variant;

AbilityDefinition::AbilityDefinition() = default;

AbilityDefinition::~AbilityDefinition() = default;

void AbilityDefinition::_bind_methods() {
  // Display properties
  ClassDB::bind_method(D_METHOD("set_ability_name", "name"),
                       &AbilityDefinition::set_ability_name);
  ClassDB::bind_method(D_METHOD("get_ability_name"),
                       &AbilityDefinition::get_ability_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "ability_name"),
               "set_ability_name", "get_ability_name");

  ClassDB::bind_method(D_METHOD("set_description", "description"),
                       &AbilityDefinition::set_description);
  ClassDB::bind_method(D_METHOD("get_description"),
                       &AbilityDefinition::get_description);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description",
               "get_description");

  ClassDB::bind_method(D_METHOD("set_icon", "icon"),
                       &AbilityDefinition::set_icon);
  ClassDB::bind_method(D_METHOD("get_icon"), &AbilityDefinition::get_icon);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "icon",
                            godot::PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"),
               "set_icon", "get_icon");

  // Cost properties
  ClassDB::bind_method(D_METHOD("set_mana_cost", "cost"),
                       &AbilityDefinition::set_mana_cost);
  ClassDB::bind_method(D_METHOD("get_mana_cost"),
                       &AbilityDefinition::get_mana_cost);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mana_cost"), "set_mana_cost",
               "get_mana_cost");

  ClassDB::bind_method(D_METHOD("set_cooldown", "cooldown"),
                       &AbilityDefinition::set_cooldown);
  ClassDB::bind_method(D_METHOD("get_cooldown"),
                       &AbilityDefinition::get_cooldown);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cooldown"), "set_cooldown",
               "get_cooldown");

  // Casting properties
  ClassDB::bind_method(D_METHOD("set_cast_type", "type"),
                       &AbilityDefinition::set_cast_type);
  ClassDB::bind_method(D_METHOD("get_cast_type"),
                       &AbilityDefinition::get_cast_type);
  ADD_PROPERTY(
      PropertyInfo(Variant::INT, "cast_type", godot::PROPERTY_HINT_ENUM,
                   "Instant,CastTime,Channel"),
      "set_cast_type", "get_cast_type");

  ClassDB::bind_method(D_METHOD("set_cast_time", "time"),
                       &AbilityDefinition::set_cast_time);
  ClassDB::bind_method(D_METHOD("get_cast_time"),
                       &AbilityDefinition::get_cast_time);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cast_time"), "set_cast_time",
               "get_cast_time");

  ClassDB::bind_method(D_METHOD("set_cast_point", "point"),
                       &AbilityDefinition::set_cast_point);
  ClassDB::bind_method(D_METHOD("get_cast_point"),
                       &AbilityDefinition::get_cast_point);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cast_point"), "set_cast_point",
               "get_cast_point");

  ClassDB::bind_method(D_METHOD("set_channel_duration", "duration"),
                       &AbilityDefinition::set_channel_duration);
  ClassDB::bind_method(D_METHOD("get_channel_duration"),
                       &AbilityDefinition::get_channel_duration);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "channel_duration"),
               "set_channel_duration", "get_channel_duration");

  // Targeting properties
  ClassDB::bind_method(D_METHOD("set_targeting_type", "type"),
                       &AbilityDefinition::set_targeting_type);
  ClassDB::bind_method(D_METHOD("get_targeting_type"),
                       &AbilityDefinition::get_targeting_type);
  ADD_PROPERTY(
      PropertyInfo(Variant::INT, "targeting_type", godot::PROPERTY_HINT_ENUM,
                   "UnitTarget,PointTarget,Area,Skillshot,SelfCast"),
      "set_targeting_type", "get_targeting_type");

  ClassDB::bind_method(D_METHOD("set_range", "range"),
                       &AbilityDefinition::set_range);
  ClassDB::bind_method(D_METHOD("get_range"), &AbilityDefinition::get_range);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "range"), "set_range", "get_range");

  ClassDB::bind_method(D_METHOD("set_aoe_radius", "radius"),
                       &AbilityDefinition::set_aoe_radius);
  ClassDB::bind_method(D_METHOD("get_aoe_radius"),
                       &AbilityDefinition::get_aoe_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "aoe_radius"), "set_aoe_radius",
               "get_aoe_radius");

  // Skillshot properties
  ClassDB::bind_method(D_METHOD("set_skillshot_speed", "speed"),
                       &AbilityDefinition::set_skillshot_speed);
  ClassDB::bind_method(D_METHOD("get_skillshot_speed"),
                       &AbilityDefinition::get_skillshot_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "skillshot_speed"),
               "set_skillshot_speed", "get_skillshot_speed");

  ClassDB::bind_method(D_METHOD("set_skillshot_max_distance", "distance"),
                       &AbilityDefinition::set_skillshot_max_distance);
  ClassDB::bind_method(D_METHOD("get_skillshot_max_distance"),
                       &AbilityDefinition::get_skillshot_max_distance);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "skillshot_max_distance"),
               "set_skillshot_max_distance", "get_skillshot_max_distance");

  ClassDB::bind_method(D_METHOD("set_skillshot_hit_radius", "radius"),
                       &AbilityDefinition::set_skillshot_hit_radius);
  ClassDB::bind_method(D_METHOD("get_skillshot_hit_radius"),
                       &AbilityDefinition::get_skillshot_hit_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "skillshot_hit_radius"),
               "set_skillshot_hit_radius", "get_skillshot_hit_radius");

  // Damage property
  ClassDB::bind_method(D_METHOD("set_base_damage", "damage"),
                       &AbilityDefinition::set_base_damage);
  ClassDB::bind_method(D_METHOD("get_base_damage"),
                       &AbilityDefinition::get_base_damage);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_damage"), "set_base_damage",
               "get_base_damage");

  // Effect property
  ClassDB::bind_method(D_METHOD("set_effect", "effect"),
                       &AbilityDefinition::set_effect);
  ClassDB::bind_method(D_METHOD("get_effect"), &AbilityDefinition::get_effect);
  ADD_PROPERTY(
      PropertyInfo(Variant::OBJECT, "effect",
                   godot::PROPERTY_HINT_RESOURCE_TYPE, "AbilityEffect"),
      "set_effect", "get_effect");
}

void AbilityDefinition::set_ability_name(const String& name) {
  ability_name = name;
}

String AbilityDefinition::get_ability_name() const {
  return ability_name;
}

void AbilityDefinition::set_description(const String& desc) {
  description = desc;
}

String AbilityDefinition::get_description() const {
  return description;
}

void AbilityDefinition::set_icon(const Ref<Texture2D>& texture) {
  icon = texture;
}

Ref<Texture2D> AbilityDefinition::get_icon() const {
  return icon;
}

void AbilityDefinition::set_mana_cost(float cost) {
  mana_cost = cost >= 0.0f ? cost : 0.0f;
}

float AbilityDefinition::get_mana_cost() const {
  return mana_cost;
}

void AbilityDefinition::set_cooldown(float duration) {
  cooldown = duration >= 0.0f ? duration : 0.0f;
}

float AbilityDefinition::get_cooldown() const {
  return cooldown;
}

void AbilityDefinition::set_cast_type(int type) {
  cast_type = type;
}

int AbilityDefinition::get_cast_type() const {
  return cast_type;
}

void AbilityDefinition::set_cast_time(float duration) {
  cast_time = duration >= 0.0f ? duration : 0.0f;
}

float AbilityDefinition::get_cast_time() const {
  return cast_time;
}

void AbilityDefinition::set_cast_point(float point) {
  cast_point = point >= 0.0f && point <= 1.0f ? point : 0.4f;
}

float AbilityDefinition::get_cast_point() const {
  return cast_point;
}

void AbilityDefinition::set_channel_duration(float duration) {
  channel_duration = duration >= 0.0f ? duration : 0.0f;
}

float AbilityDefinition::get_channel_duration() const {
  return channel_duration;
}

void AbilityDefinition::set_targeting_type(int type) {
  targeting_type = type;
}

int AbilityDefinition::get_targeting_type() const {
  return targeting_type;
}

void AbilityDefinition::set_range(float r) {
  range = r >= 0.0f ? r : 0.0f;
}

float AbilityDefinition::get_range() const {
  return range;
}

void AbilityDefinition::set_aoe_radius(float radius) {
  aoe_radius = radius >= 0.0f ? radius : 0.0f;
}

float AbilityDefinition::get_aoe_radius() const {
  return aoe_radius;
}

void AbilityDefinition::set_skillshot_speed(float speed) {
  skillshot_speed = speed >= 0.0f ? speed : 30.0f;
}

float AbilityDefinition::get_skillshot_speed() const {
  return skillshot_speed;
}

void AbilityDefinition::set_skillshot_max_distance(float distance) {
  skillshot_max_distance = distance >= 0.0f ? distance : 20.0f;
}

float AbilityDefinition::get_skillshot_max_distance() const {
  return skillshot_max_distance;
}

void AbilityDefinition::set_skillshot_hit_radius(float radius) {
  skillshot_hit_radius = radius >= 0.0f ? radius : 0.5f;
}

float AbilityDefinition::get_skillshot_hit_radius() const {
  return skillshot_hit_radius;
}

void AbilityDefinition::set_base_damage(float damage) {
  base_damage = damage >= 0.0f ? damage : 0.0f;
}

float AbilityDefinition::get_base_damage() const {
  return base_damage;
}

void AbilityDefinition::set_effect(const Ref<AbilityEffect>& e) {
  effect = e;
}

Ref<AbilityEffect> AbilityDefinition::get_effect() const {
  return effect;
}

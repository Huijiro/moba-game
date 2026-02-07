#include "ability_node.hpp"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

AbilityNode::AbilityNode() = default;

AbilityNode::~AbilityNode() = default;

void AbilityNode::_bind_methods() {
  // ========== BASIC INFO ==========
  ADD_GROUP("Basic Info", "");

  ClassDB::bind_method(D_METHOD("set_ability_name", "name"),
                       &AbilityNode::set_ability_name);
  ClassDB::bind_method(D_METHOD("get_ability_name"),
                       &AbilityNode::get_ability_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "ability_name"),
               "set_ability_name", "get_ability_name");

  ClassDB::bind_method(D_METHOD("set_description", "description"),
                       &AbilityNode::set_description);
  ClassDB::bind_method(D_METHOD("get_description"),
                       &AbilityNode::get_description);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description",
               "get_description");

  ClassDB::bind_method(D_METHOD("set_icon", "icon"), &AbilityNode::set_icon);
  ClassDB::bind_method(D_METHOD("get_icon"), &AbilityNode::get_icon);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "icon",
                            godot::PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"),
               "set_icon", "get_icon");

  // ========== COST & COOLDOWN ==========
  ADD_GROUP("Cost & Cooldown", "");

  ClassDB::bind_method(D_METHOD("set_resource_pool_id", "pool_id"),
                       &AbilityNode::set_resource_pool_id);
  ClassDB::bind_method(D_METHOD("get_resource_pool_id"),
                       &AbilityNode::get_resource_pool_id);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "resource_pool_id"),
               "set_resource_pool_id", "get_resource_pool_id");

  ClassDB::bind_method(D_METHOD("set_resource_cost", "cost"),
                       &AbilityNode::set_resource_cost);
  ClassDB::bind_method(D_METHOD("get_resource_cost"),
                       &AbilityNode::get_resource_cost);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "resource_cost"),
               "set_resource_cost", "get_resource_cost");

  ClassDB::bind_method(D_METHOD("set_cooldown", "cooldown"),
                       &AbilityNode::set_cooldown);
  ClassDB::bind_method(D_METHOD("get_cooldown"), &AbilityNode::get_cooldown);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cooldown"), "set_cooldown",
               "get_cooldown");

  // ========== CASTING BEHAVIOR ==========
  ADD_GROUP("Casting Behavior", "");

  ClassDB::bind_method(D_METHOD("set_cast_type", "type"),
                       &AbilityNode::set_cast_type);
  ClassDB::bind_method(D_METHOD("get_cast_type"), &AbilityNode::get_cast_type);
  ADD_PROPERTY(
      PropertyInfo(Variant::INT, "cast_type", godot::PROPERTY_HINT_ENUM,
                   "Instant,CastTime,Channel"),
      "set_cast_type", "get_cast_type");

  ClassDB::bind_method(D_METHOD("set_cast_time", "time"),
                       &AbilityNode::set_cast_time);
  ClassDB::bind_method(D_METHOD("get_cast_time"), &AbilityNode::get_cast_time);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cast_time"), "set_cast_time",
               "get_cast_time");

  ClassDB::bind_method(D_METHOD("set_cast_point", "point"),
                       &AbilityNode::set_cast_point);
  ClassDB::bind_method(D_METHOD("get_cast_point"),
                       &AbilityNode::get_cast_point);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cast_point"), "set_cast_point",
               "get_cast_point");

  ClassDB::bind_method(D_METHOD("set_channel_duration", "duration"),
                       &AbilityNode::set_channel_duration);
  ClassDB::bind_method(D_METHOD("get_channel_duration"),
                       &AbilityNode::get_channel_duration);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "channel_duration"),
               "set_channel_duration", "get_channel_duration");

  ClassDB::bind_method(D_METHOD("set_channel_tick_interval", "interval"),
                       &AbilityNode::set_channel_tick_interval);
  ClassDB::bind_method(D_METHOD("get_channel_tick_interval"),
                       &AbilityNode::get_channel_tick_interval);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "channel_tick_interval"),
               "set_channel_tick_interval", "get_channel_tick_interval");

  // ========== TARGETING ==========
  ADD_GROUP("Targeting", "");

  ClassDB::bind_method(D_METHOD("set_targeting_type", "type"),
                       &AbilityNode::set_targeting_type);
  ClassDB::bind_method(D_METHOD("get_targeting_type"),
                       &AbilityNode::get_targeting_type);
  ADD_PROPERTY(
      PropertyInfo(Variant::INT, "targeting_type", godot::PROPERTY_HINT_ENUM,
                   "UnitTarget,PointTarget,Area,Skillshot,SelfCast"),
      "set_targeting_type", "get_targeting_type");

  ClassDB::bind_method(D_METHOD("set_range", "range"), &AbilityNode::set_range);
  ClassDB::bind_method(D_METHOD("get_range"), &AbilityNode::get_range);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "range"), "set_range", "get_range");

  ClassDB::bind_method(D_METHOD("set_aoe_radius", "radius"),
                       &AbilityNode::set_aoe_radius);
  ClassDB::bind_method(D_METHOD("get_aoe_radius"),
                       &AbilityNode::get_aoe_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "aoe_radius"), "set_aoe_radius",
               "get_aoe_radius");

  // ========== SKILLSHOT ==========
  ADD_GROUP("Skillshot", "");

  ClassDB::bind_method(D_METHOD("set_skillshot_speed", "speed"),
                       &AbilityNode::set_skillshot_speed);
  ClassDB::bind_method(D_METHOD("get_skillshot_speed"),
                       &AbilityNode::get_skillshot_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "skillshot_speed"),
               "set_skillshot_speed", "get_skillshot_speed");

  ClassDB::bind_method(D_METHOD("set_skillshot_max_distance", "distance"),
                       &AbilityNode::set_skillshot_max_distance);
  ClassDB::bind_method(D_METHOD("get_skillshot_max_distance"),
                       &AbilityNode::get_skillshot_max_distance);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "skillshot_max_distance"),
               "set_skillshot_max_distance", "get_skillshot_max_distance");

  ClassDB::bind_method(D_METHOD("set_skillshot_hit_radius", "radius"),
                       &AbilityNode::set_skillshot_hit_radius);
  ClassDB::bind_method(D_METHOD("get_skillshot_hit_radius"),
                       &AbilityNode::get_skillshot_hit_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "skillshot_hit_radius"),
               "set_skillshot_hit_radius", "get_skillshot_hit_radius");

  // ========== DAMAGE ==========
  ADD_GROUP("Damage", "");

  ClassDB::bind_method(D_METHOD("set_base_damage", "damage"),
                       &AbilityNode::set_base_damage);
  ClassDB::bind_method(D_METHOD("get_base_damage"),
                       &AbilityNode::get_base_damage);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_damage"), "set_base_damage",
               "get_base_damage");

  // ========== VFX SYSTEM ==========
  ClassDB::bind_method(D_METHOD("play_vfx", "vfx_name", "params"),
                       &AbilityNode::play_vfx, DEFVAL(godot::Dictionary()));
  ClassDB::bind_method(D_METHOD("_register_vfx"), &AbilityNode::_register_vfx);
}

bool AbilityNode::_validate_property(PropertyInfo& p_property) const {
  const String& prop_name = p_property.name;

  // Hide cast_time/cast_point if not CAST_TIME
  if (cast_type != static_cast<int>(CastType::CAST_TIME)) {
    if (prop_name == "cast_time" || prop_name == "cast_point") {
      p_property.usage = godot::PROPERTY_USAGE_NONE;
      return false;
    }
  }

  // Hide channel properties if not CHANNEL
  if (cast_type != static_cast<int>(CastType::CHANNEL)) {
    if (prop_name == "channel_duration" ||
        prop_name == "channel_tick_interval") {
      p_property.usage = godot::PROPERTY_USAGE_NONE;
      return false;
    }
  }

  // Hide range if SELF_CAST
  if (targeting_type == static_cast<int>(TargetingType::SELF_CAST)) {
    if (prop_name == "range") {
      p_property.usage = godot::PROPERTY_USAGE_NONE;
      return false;
    }
  }

  // Hide aoe_radius if not AREA/POINT_TARGET
  if (targeting_type != static_cast<int>(TargetingType::AREA) &&
      targeting_type != static_cast<int>(TargetingType::POINT_TARGET)) {
    if (prop_name == "aoe_radius") {
      p_property.usage = godot::PROPERTY_USAGE_NONE;
      return false;
    }
  }

  // Hide skillshot properties if not SKILLSHOT
  if (targeting_type != static_cast<int>(TargetingType::SKILLSHOT)) {
    if (prop_name == "skillshot_speed" ||
        prop_name == "skillshot_max_distance" ||
        prop_name == "skillshot_hit_radius") {
      p_property.usage = godot::PROPERTY_USAGE_NONE;
      return false;
    }
  }

  return true;
}

// Display properties
void AbilityNode::set_ability_name(const String& name) {
  ability_name = name;
}

String AbilityNode::get_ability_name() const {
  return ability_name;
}

void AbilityNode::set_description(const String& desc) {
  description = desc;
}

String AbilityNode::get_description() const {
  return description;
}

void AbilityNode::set_icon(const Ref<Texture2D>& texture) {
  icon = texture;
}

Ref<Texture2D> AbilityNode::get_icon() const {
  return icon;
}

// Cost properties
void AbilityNode::set_resource_pool_id(const String& pool_id) {
  resource_pool_id = pool_id;
}

String AbilityNode::get_resource_pool_id() const {
  return resource_pool_id;
}

void AbilityNode::set_resource_cost(float cost) {
  resource_cost = cost >= 0.0f ? cost : 0.0f;
}

float AbilityNode::get_resource_cost() const {
  return resource_cost;
}

void AbilityNode::set_cooldown(float duration) {
  cooldown = duration >= 0.0f ? duration : 0.0f;
}

float AbilityNode::get_cooldown() const {
  return cooldown;
}

// Casting properties
void AbilityNode::set_cast_type(int type) {
  if (cast_type != type) {
    cast_type = type;
    notify_property_list_changed();
  }
}

int AbilityNode::get_cast_type() const {
  return cast_type;
}

void AbilityNode::set_cast_time(float duration) {
  cast_time = duration >= 0.0f ? duration : 0.0f;
}

float AbilityNode::get_cast_time() const {
  return cast_time;
}

void AbilityNode::set_cast_point(float point) {
  cast_point = point >= 0.0f && point <= 1.0f ? point : 0.4f;
}

float AbilityNode::get_cast_point() const {
  return cast_point;
}

void AbilityNode::set_channel_duration(float duration) {
  channel_duration = duration >= 0.0f ? duration : 0.0f;
}

float AbilityNode::get_channel_duration() const {
  return channel_duration;
}

void AbilityNode::set_channel_tick_interval(float interval) {
  channel_tick_interval = interval >= 0.0f ? interval : 0.0f;
}

float AbilityNode::get_channel_tick_interval() const {
  return channel_tick_interval;
}

// Targeting properties
void AbilityNode::set_targeting_type(int type) {
  if (targeting_type != type) {
    targeting_type = type;
    notify_property_list_changed();
  }
}

int AbilityNode::get_targeting_type() const {
  return targeting_type;
}

void AbilityNode::set_range(float r) {
  range = r >= 0.0f ? r : 0.0f;
}

float AbilityNode::get_range() const {
  return range;
}

void AbilityNode::set_aoe_radius(float radius) {
  aoe_radius = radius >= 0.0f ? radius : 0.0f;
}

float AbilityNode::get_aoe_radius() const {
  return aoe_radius;
}

// Skillshot properties
void AbilityNode::set_skillshot_speed(float speed) {
  skillshot_speed = speed >= 0.0f ? speed : 30.0f;
}

float AbilityNode::get_skillshot_speed() const {
  return skillshot_speed;
}

void AbilityNode::set_skillshot_max_distance(float distance) {
  skillshot_max_distance = distance >= 0.0f ? distance : 20.0f;
}

float AbilityNode::get_skillshot_max_distance() const {
  return skillshot_max_distance;
}

void AbilityNode::set_skillshot_hit_radius(float radius) {
  skillshot_hit_radius = radius >= 0.0f ? radius : 0.5f;
}

float AbilityNode::get_skillshot_hit_radius() const {
  return skillshot_hit_radius;
}

// Damage property
void AbilityNode::set_base_damage(float damage) {
  base_damage = damage >= 0.0f ? damage : 0.0f;
}

float AbilityNode::get_base_damage() const {
  return base_damage;
}

// Virtual methods - default implementations
void AbilityNode::_reset() {
  // Default: no-op. Subclasses override to reset state for next cast.
}

bool AbilityNode::execute(Unit* caster, Unit* target, godot::Vector3 position) {
  // Default: no-op. Subclasses implement actual ability logic.
  // Return true to indicate successful execution.
  return true;
}

bool AbilityNode::can_execute_on_target(Unit* caster, Unit* target) const {
  // Default: always allowed. Subclasses can add validation.
  return true;
}

float AbilityNode::calculate_damage(Unit* caster, Unit* target) const {
  // Default: return base damage. Subclasses can add scaling logic.
  return base_damage;
}

bool AbilityNode::play_vfx(const String& vfx_name,
                           const godot::Dictionary& params) {
  // Find VFX child node with the given name
  Node* vfx_node = find_child(vfx_name, true, false);
  if (vfx_node == nullptr) {
    DBG_WARN("AbilityNode", "VFX not found: " + ability_name + "." + vfx_name);
    return false;
  }

  // Cast to VFXNode and play
  auto vfx = Object::cast_to<godot::Node3D>(vfx_node);
  if (vfx == nullptr) {
    DBG_WARN("AbilityNode",
             "VFX node is not Node3D: " + ability_name + "." + vfx_name);
    return false;
  }

  // Call play method with parameters
  vfx->call("play", params);
  DBG_INFO("AbilityNode", "Triggered VFX: " + ability_name + "." + vfx_name);
  return true;
}

void AbilityNode::_register_vfx() {
  // Scan children for VFXNode instances and register with manager
  // This is called when the ability is added to the scene
  // For now, we'll defer VFX discovery to when they're needed
}

#ifndef GDEXTENSION_ABILITY_DEFINITION_H
#define GDEXTENSION_ABILITY_DEFINITION_H

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/templates/list.hpp>

#include "ability_effect.hpp"
#include "ability_types.hpp"

using godot::Dictionary;
using godot::PropertyInfo;
using godot::Ref;
using godot::Resource;
using godot::String;
using godot::Texture2D;
using godot::TypedArray;

/// Resource-based ability definition - save as .tres files in editor
/// Contains all the stats and configuration for an ability
///
/// Documentation: See docs/ability-system/01_ADDING_ABILITIES.md
/// - Complete step-by-step guide for creating abilities
/// - Cast type reference (INSTANT, CAST_TIME, CHANNEL)
/// - Targeting type reference (UNIT_TARGET, POINT_TARGET, AREA)
/// - Frost Bolt example walkthrough
/// - Troubleshooting common issues
///
/// Example files:
/// - GodotGame/resources/abilities/Q_Instant_Strike.tres
/// - GodotGame/resources/abilities/Frost_Bolt.tres
class AbilityDefinition : public Resource {
  GDCLASS(AbilityDefinition, Resource)

 protected:
  static void _bind_methods();
  void _get_property_list(godot::List<PropertyInfo>* r_list) const;

  // Display information
  String ability_name = "Ability";
  String description = "";
  Ref<Texture2D> icon = nullptr;

  // Cost - can use any resource pool by specifying pool name and amount
  String resource_pool_id =
      "default";  // Which resource pool to use (e.g., "mana", "energy")
  float resource_cost = 0.0f;  // Amount to deduct from resource pool
  float cooldown = 1.0f;

  // Casting behavior
  int cast_type = static_cast<int>(CastType::INSTANT);
  float cast_time = 0.0f;   // Duration before ability executes
  float cast_point = 0.4f;  // When in cast time (0.0-1.0) the ability fires
  float channel_duration = 0.0f;  // For CHANNEL type abilities
  float channel_tick_interval =
      0.0f;  // Time between damage ticks (0.0 = no ticks)

  // Targeting
  int targeting_type = static_cast<int>(TargetingType::SELF_CAST);
  float range = 0.0f;
  float aoe_radius = 0.0f;

  // Skillshot-specific properties
  float skillshot_speed = 30.0f;         // Projectile travel speed
  float skillshot_max_distance = 20.0f;  // Max range before detonation
  float skillshot_hit_radius = 0.5f;     // Collision detection radius

  // Damage
  float base_damage = 0.0f;

  // Effect instance - handles all gameplay impact
  Ref<AbilityEffect> effect = nullptr;

 public:
  AbilityDefinition();
  ~AbilityDefinition();

  // Display properties
  void set_ability_name(const String& name);
  String get_ability_name() const;

  void set_description(const String& desc);
  String get_description() const;

  void set_icon(const Ref<Texture2D>& texture);
  Ref<Texture2D> get_icon() const;

  // Cost properties
  void set_resource_pool_id(const String& pool_id);
  String get_resource_pool_id() const;

  void set_resource_cost(float cost);
  float get_resource_cost() const;

  // Legacy mana cost (for backwards compatibility)
  void set_mana_cost(float cost);
  float get_mana_cost() const;

  void set_cooldown(float duration);
  float get_cooldown() const;

  // Casting properties
  void set_cast_type(int type);
  int get_cast_type() const;

  void set_cast_time(float duration);
  float get_cast_time() const;

  void set_cast_point(float point);
  float get_cast_point() const;

  void set_channel_duration(float duration);
  float get_channel_duration() const;

  void set_channel_tick_interval(float interval);
  float get_channel_tick_interval() const;

  // Targeting properties
  void set_targeting_type(int type);
  int get_targeting_type() const;

  void set_range(float r);
  float get_range() const;

  void set_aoe_radius(float radius);
  float get_aoe_radius() const;

  // Skillshot properties
  void set_skillshot_speed(float speed);
  float get_skillshot_speed() const;

  void set_skillshot_max_distance(float distance);
  float get_skillshot_max_distance() const;

  void set_skillshot_hit_radius(float radius);
  float get_skillshot_hit_radius() const;

  // Damage property
  void set_base_damage(float damage);
  float get_base_damage() const;

  // Effect property
  void set_effect(const Ref<AbilityEffect>& e);
  Ref<AbilityEffect> get_effect() const;
};

#endif  // GDEXTENSION_ABILITY_DEFINITION_H

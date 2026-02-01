#ifndef GDEXTENSION_ABILITY_DEFINITION_H
#define GDEXTENSION_ABILITY_DEFINITION_H

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/texture2d.hpp>

#include "ability_effect.hpp"
#include "ability_types.hpp"

using godot::Ref;
using godot::Resource;
using godot::String;
using godot::Texture2D;

// Resource-based ability definition - save as .tres files in editor
// Contains all the stats and configuration for an ability
class AbilityDefinition : public Resource {
  GDCLASS(AbilityDefinition, Resource)

 protected:
  static void _bind_methods();

  // Display information
  String ability_name = "Ability";
  String description = "";
  Ref<Texture2D> icon = nullptr;

  // Cost
  float mana_cost = 0.0f;
  float cooldown = 1.0f;

  // Casting behavior
  int cast_type = static_cast<int>(CastType::INSTANT);
  float cast_time = 0.0f;   // Duration before ability executes
  float cast_point = 0.4f;  // When in cast time (0.0-1.0) the ability fires
  float channel_duration = 0.0f;  // For CHANNEL type abilities

  // Targeting
  int targeting_type = static_cast<int>(TargetingType::SELF_CAST);
  float range = 0.0f;
  float aoe_radius = 0.0f;

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

  // Targeting properties
  void set_targeting_type(int type);
  int get_targeting_type() const;

  void set_range(float r);
  float get_range() const;

  void set_aoe_radius(float radius);
  float get_aoe_radius() const;

  // Damage property
  void set_base_damage(float damage);
  float get_base_damage() const;

  // Effect property
  void set_effect(const Ref<AbilityEffect>& e);
  Ref<AbilityEffect> get_effect() const;
};

#endif  // GDEXTENSION_ABILITY_DEFINITION_H

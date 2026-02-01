#ifndef GDEXTENSION_UNIT_DEFINITION_H
#define GDEXTENSION_UNIT_DEFINITION_H

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

using godot::Array;
using godot::Ref;
using godot::Resource;
using godot::String;

class AbilityDefinition;

/// Unit definition resource - contains any unit's data and 4 ability slots
/// Works for: Heroes, Minions, Jungle Camps, Bosses, NPCs, etc.
/// Save as .tres file and assign to Units in the editor
///
/// Documentation: See docs/ability-system/02_MODIFYING_UNITS.md
/// - How to create new unit definitions
/// - Modifying existing units
/// - Using units in scenes
///
/// Example files:
/// - GodotGame/resources/units/TestHero.tres
/// - GodotGame/resources/units/MageHero.tres
class UnitDefinition : public Resource {
  GDCLASS(UnitDefinition, Resource)

 protected:
  static void _bind_methods();

  // Unit identity
  String unit_name = "Unit";
  int unit_id = 0;
  String description = "";
  String unit_type = "hero";  // "hero", "minion", "jungle_camp", "boss", etc.

  // Unit abilities (configurable count, typically 4-6: Q, W, E, R, D, F)
  int ability_count = 4;  // Number of ability slots for this unit
  Array abilities;        // Array of AbilityDefinition references

  // Future: Stats for scaling
  // int base_health = 100;
  // int base_mana = 100;

 public:
  UnitDefinition();
  ~UnitDefinition();

  // Unit identity properties
  void set_unit_name(const String& name);
  String get_unit_name() const;

  void set_unit_id(int id);
  int get_unit_id() const;

  void set_description(const String& desc);
  String get_description() const;

  void set_unit_type(const String& type);
  String get_unit_type() const;

  // Ability count management
  void set_ability_count(int count);
  int get_ability_count() const;

  // Ability management
  void set_ability(int slot, const Ref<AbilityDefinition>& ability);
  Ref<AbilityDefinition> get_ability(int slot) const;
  bool has_ability(int slot) const;

  // Get all abilities as array
  Array get_abilities() const;
  void set_abilities(const Array& new_abilities);
};

#endif  // GDEXTENSION_UNIT_DEFINITION_H

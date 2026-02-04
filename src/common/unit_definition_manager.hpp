#ifndef GDEXTENSION_UNIT_DEFINITION_MANAGER_H
#define GDEXTENSION_UNIT_DEFINITION_MANAGER_H

#include <map>

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>

using godot::Node;
using godot::Ref;
using godot::String;

class UnitDefinition;

// Unit definition manager - central registry for all unit definitions
// Can register units at startup and look them up by ID or name
// Supports: Heroes, Minions, Jungle Camps, Bosses, NPCs, etc.
class UnitDefinitionManager : public Node {
  GDCLASS(UnitDefinitionManager, Node)

 protected:
  static void _bind_methods();

  // Static registry of units (loaded from resources)
  static std::map<int, Ref<UnitDefinition>> unit_registry;
  static UnitDefinitionManager* singleton;

 public:
  void _ready() override;
  UnitDefinitionManager();
  ~UnitDefinitionManager();

  // Singleton access
  static UnitDefinitionManager* get_singleton();

  // Unit registration and lookup
  static void register_unit(const Ref<UnitDefinition>& unit_def);
  static Ref<UnitDefinition> get_unit_by_id(int unit_id);
  static Ref<UnitDefinition> get_unit_by_name(const String& name);
  static bool has_unit(int unit_id);

  // Debugging/Development
  static void list_all_units();
};

#endif  // GDEXTENSION_UNIT_DEFINITION_MANAGER_H

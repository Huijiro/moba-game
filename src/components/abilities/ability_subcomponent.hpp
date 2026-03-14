#ifndef GDEXTENSION_ABILITY_SUBCOMPONENT_H
#define GDEXTENSION_ABILITY_SUBCOMPONENT_H

#include <godot_cpp/classes/node.hpp>

using godot::Node;

class AbilityNode;
class LabelRegistry;

/// Base class for ability child components.
/// Mirrors UnitComponent: validates parentage and exposes get_ability().
class AbilitySubcomponent : public Node {
  GDCLASS(AbilitySubcomponent, Node)

 protected:
  static void _bind_methods();

  AbilityNode* owner_ability = nullptr;

 public:
  AbilitySubcomponent();
  ~AbilitySubcomponent();

  void _ready() override;

  AbilityNode* get_ability() const;

  /// Optional: Register debug label data. Override in subclasses to expose
  /// debug info.
  virtual void register_debug_labels(LabelRegistry* registry) {
    // Default: do nothing
  }
};

#endif  // GDEXTENSION_ABILITY_SUBCOMPONENT_H

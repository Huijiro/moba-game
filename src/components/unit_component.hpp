#ifndef GDEXTENSION_UNIT_COMPONENT_H
#define GDEXTENSION_UNIT_COMPONENT_H

#include <godot_cpp/classes/node.hpp>

using godot::Node;

class Unit;
class LabelRegistry;

class UnitComponent : public Node {
  GDCLASS(UnitComponent, Node)

 protected:
  static void _bind_methods();

  Unit* owner_unit = nullptr;

 public:
  UnitComponent();
  ~UnitComponent();

  void _ready() override;

  Unit* get_unit() const;

  /// Optional: Register debug label data. Override in subclasses to expose
  /// debug info.
  virtual void register_debug_labels(LabelRegistry* registry) {
    // Default: do nothing
  }
};

#endif  // GDEXTENSION_UNIT_COMPONENT_H

#ifndef GDEXTENSION_UNIT_COMPONENT_H
#define GDEXTENSION_UNIT_COMPONENT_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string_name.hpp>

using godot::Callable;
using godot::Node;
using godot::StringName;

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

  /// Helper to connect to dynamic signals on the Unit
  /// Uses Unit's signal multiplexer instead of Godot's pre-registered signals
  void connect_unit_signal(const StringName& signal_name,
                           const Callable& callable);

  /// Optional: Register debug label data. Override in subclasses to expose
  /// debug info.
  virtual void register_debug_labels(LabelRegistry* registry) {
    // Default: do nothing
  }
};

#endif  // GDEXTENSION_UNIT_COMPONENT_H

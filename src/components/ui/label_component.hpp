#ifndef GDEXTENSION_LABEL_COMPONENT_H
#define GDEXTENSION_LABEL_COMPONENT_H

#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include "label_registry.hpp"

using godot::Label3D;
using godot::Vector3;

class Unit;

/// Label3D component for displaying unit debug information
/// Manages its own rendering and updates, attached directly to Unit nodes
class LabelComponent : public Label3D {
  GDCLASS(LabelComponent, Label3D)

 protected:
  static void _bind_methods();

 public:
  LabelComponent();
  ~LabelComponent() = default;

  void _ready() override;
  void _process(double delta) override;

  // Properties
  void set_update_rate(float rate);
  float get_update_rate() const;

  void set_label_offset(const Vector3& offset);
  Vector3 get_label_offset() const;

 private:
  // Update label content and positioning
  void _update_label_content();
  void _update_label_transform();

  Unit* owner_unit = nullptr;
  LabelRegistry registry;

  // Configuration
  float update_rate = 10.0f;  // Updates per second
  Vector3 label_offset = Vector3(0, 2.5f, 0);

  // Timing
  double accumulated_time = 0.0;
};

#endif  // GDEXTENSION_LABEL_COMPONENT_H

#ifndef GDEXTENSION_LABEL_COMPONENT_H
#define GDEXTENSION_LABEL_COMPONENT_H

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include "label_registry.hpp"

using godot::Camera3D;
using godot::Label;
using godot::Node;
using godot::Vector2;
using godot::Vector3;

class Unit;

/// 2D label component for displaying unit debug information
/// Tracks unit position on screen and displays as 2D label
class LabelComponent : public Node {
  GDCLASS(LabelComponent, Node)

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
  void _update_label_position();

  Unit* owner_unit = nullptr;
  Camera3D* camera = nullptr;
  Label* label_2d = nullptr;
  LabelRegistry registry;

  // Configuration
  float update_rate = 10.0f;                   // Updates per second
  Vector3 world_offset = Vector3(0, 2.5f, 0);  // World space offset from unit

  // Timing
  double accumulated_time = 0.0;
};

#endif  // GDEXTENSION_LABEL_COMPONENT_H

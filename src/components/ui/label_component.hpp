#ifndef GDEXTENSION_LABEL_COMPONENT_H
#define GDEXTENSION_LABEL_COMPONENT_H

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include "../unit_component.hpp"
#include "label_registry.hpp"

using godot::Camera3D;
using godot::Label3D;
using godot::Vector3;

/// Component for displaying unit debug labels above units
/// Shows registered debug information from other components
class LabelComponent : public UnitComponent {
  GDCLASS(LabelComponent, UnitComponent)

 protected:
  static void _bind_methods();

 public:
  void _ready() override;
  void _process(double delta) override;

 private:
  // Update label content and positioning
  void _update_label_content();
  void _update_label_transform();
  Label3D* label_3d = nullptr;
  LabelRegistry registry;

  // Configuration
  float update_rate = 10.0f;  // Updates per second
  Vector3 label_offset = Vector3(0, 2.5f, 0);
  int font_size = 32;
  bool enabled = true;

  // Timing
  double accumulated_time = 0.0;

 public:
  LabelComponent();
  ~LabelComponent() = default;

  // Properties
  void set_update_rate(float rate);
  float get_update_rate() const;

  void set_label_offset(const Vector3& offset);
  Vector3 get_label_offset() const;

  void set_font_size(int size);
  int get_font_size() const;

  void set_enabled(bool enable);
  bool is_enabled() const;
};

#endif  // GDEXTENSION_LABEL_COMPONENT_H

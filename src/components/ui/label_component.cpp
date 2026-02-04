#include "label_component.hpp"

#include <godot_cpp/classes/base_material3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>

#include "../../core/unit.hpp"

using godot::BaseMaterial3D;
using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::String;
using godot::Variant;

LabelComponent::LabelComponent() {}

void LabelComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_update_rate", "rate"),
                       &LabelComponent::set_update_rate);
  ClassDB::bind_method(D_METHOD("get_update_rate"),
                       &LabelComponent::get_update_rate);

  ClassDB::bind_method(D_METHOD("set_label_offset", "offset"),
                       &LabelComponent::set_label_offset);
  ClassDB::bind_method(D_METHOD("get_label_offset"),
                       &LabelComponent::get_label_offset);

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "update_rate"), "set_update_rate",
               "get_update_rate");
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "label_offset"),
               "set_label_offset", "get_label_offset");
}

void LabelComponent::_ready() {
  // Find parent Unit
  auto parent = get_parent();
  owner_unit = godot::Object::cast_to<Unit>(parent);

  // Configure this label
  Label3D::set_text(String(""));
  Label3D::set_billboard_mode(BaseMaterial3D::BILLBOARD_FIXED_Y);
}

void LabelComponent::_process(double delta) {
  if (!owner_unit) {
    return;
  }

  accumulated_time += delta;
  float update_interval = 1.0f / update_rate;

  if (accumulated_time >= update_interval) {
    accumulated_time -= update_interval;
    _update_label_content();
    _update_label_transform();
  }
}

void LabelComponent::_update_label_content() {
  if (!owner_unit) {
    return;
  }

  // Clear registry and collect debug info from all components
  registry.clear();
  owner_unit->register_all_debug_labels(&registry);

  // Update this label's text
  Label3D::set_text(registry.get_formatted_text());
}

void LabelComponent::_update_label_transform() {
  if (!owner_unit) {
    return;
  }

  // Position label above unit
  Vector3 target_position = owner_unit->get_global_position() + label_offset;
  Label3D::set_global_position(target_position);

  // Make label face camera
  auto viewport = get_viewport();
  if (viewport) {
    auto camera = viewport->get_camera_3d();
    if (camera) {
      Vector3 camera_pos = camera->get_global_position();
      Vector3 label_pos = Label3D::get_global_position();

      // Calculate direction from label to camera
      Vector3 look_dir = (camera_pos - label_pos).normalized();

      // Keep Y-axis fixed (upright), but rotate on XZ plane to face camera
      Vector3 target_forward = look_dir;
      target_forward.y = 0;
      target_forward = target_forward.normalized();

      // If we have a valid forward direction, rotate to face it
      if (target_forward.length() > 0.001f) {
        Label3D::look_at(label_pos + target_forward, Vector3(0, 1, 0));
      }
    }
  }
}

void LabelComponent::set_update_rate(float rate) {
  update_rate = rate > 0.1f ? rate : 0.1f;
}

float LabelComponent::get_update_rate() const {
  return update_rate;
}

void LabelComponent::set_label_offset(const Vector3& offset) {
  label_offset = offset;
}

Vector3 LabelComponent::get_label_offset() const {
  return label_offset;
}

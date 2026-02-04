#include "label_component.hpp"

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>

#include "../../core/unit.hpp"

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
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "world_offset"),
               "set_label_offset", "get_label_offset");
}

void LabelComponent::_ready() {
  // Only run in runtime, not in editor
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Find parent Unit
  auto parent = get_parent();
  owner_unit = godot::Object::cast_to<Unit>(parent);

  if (!owner_unit) {
    return;
  }

  // Get camera from viewport
  auto viewport = get_viewport();
  if (viewport) {
    camera = viewport->get_camera_3d();
  }

  // Create 2D label
  label_2d = memnew(Label);
  add_child(label_2d);

  // Configure label
  label_2d->set_text(String(""));
  label_2d->set_anchor_and_offset(godot::SIDE_LEFT,
                                  godot::Control::ANCHOR_BEGIN, 0);
  label_2d->set_anchor_and_offset(godot::SIDE_TOP, godot::Control::ANCHOR_BEGIN,
                                  0);
}

void LabelComponent::_process(double delta) {
  // Only run in runtime, not in editor
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (!owner_unit || !label_2d || !camera) {
    return;
  }

  accumulated_time += delta;
  float update_interval = 1.0f / update_rate;

  if (accumulated_time >= update_interval) {
    accumulated_time -= update_interval;
    _update_label_content();
    _update_label_position();
  }
}

void LabelComponent::_update_label_content() {
  if (!owner_unit || !label_2d) {
    return;
  }

  // Clear registry and collect debug info from all components
  registry.clear();
  owner_unit->register_all_debug_labels(&registry);

  // Update label text
  label_2d->set_text(registry.get_formatted_text());
}

void LabelComponent::_update_label_position() {
  if (!owner_unit || !label_2d || !camera) {
    return;
  }

  // Get world position of label (above unit)
  Vector3 world_pos = owner_unit->get_global_position() + world_offset;

  // Convert world position to screen position
  Vector2 screen_pos = camera->unproject_position(world_pos);

  // Set label position on screen
  label_2d->set_global_position(screen_pos);

  // Center the label on the screen position
  Vector2 label_size = label_2d->get_size();
  label_2d->set_global_position(screen_pos -
                                label_size / 2);  // Center on world position
}

void LabelComponent::set_update_rate(float rate) {
  update_rate = rate > 0.1f ? rate : 0.1f;
}

float LabelComponent::get_update_rate() const {
  return update_rate;
}

void LabelComponent::set_label_offset(const Vector3& offset) {
  world_offset = offset;
}

Vector3 LabelComponent::get_label_offset() const {
  return world_offset;
}

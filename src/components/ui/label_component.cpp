#include "label_component.hpp"

#include <godot_cpp/classes/base_material3d.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"

using godot::BaseMaterial3D;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Font;
using godot::PropertyInfo;
using godot::Ref;
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

  ClassDB::bind_method(D_METHOD("set_font_size", "size"),
                       &LabelComponent::set_font_size);
  ClassDB::bind_method(D_METHOD("get_font_size"),
                       &LabelComponent::get_font_size);

  ClassDB::bind_method(D_METHOD("set_enabled", "enabled"),
                       &LabelComponent::set_enabled);
  ClassDB::bind_method(D_METHOD("is_enabled"), &LabelComponent::is_enabled);

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "update_rate"), "set_update_rate",
               "get_update_rate");
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "label_offset"),
               "set_label_offset", "get_label_offset");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "font_size"), "set_font_size",
               "get_font_size");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled",
               "is_enabled");
}

void LabelComponent::_ready() {
  // Call parent _ready to setup owner_unit
  UnitComponent::_ready();

  // Create Label3D child node
  label_3d = memnew(Label3D);
  add_child(label_3d);

  // Position relative to component (which is on the unit)
  label_3d->set_position(label_offset);

  // Configure label
  label_3d->set_text(String("[Test] label=active"));
  label_3d->set_font_size(font_size);

  // Set billboard mode to always face camera (keep Y-axis fixed)
  label_3d->set_billboard_mode(BaseMaterial3D::BILLBOARD_FIXED_Y);

  // Ensure the label is visible
  label_3d->set_visible(true);

  DBG_INFO("LabelComponent", "Ready - created label");
}

void LabelComponent::_process(double delta) {
  if (!enabled || !label_3d) {
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
  if (!label_3d || !get_unit()) {
    DBG_WARN("LabelComponent", "Cannot update - missing label_3d or unit");
    return;
  }

  // Clear registry and collect debug info from all components
  registry.clear();
  get_unit()->register_all_debug_labels(&registry);

  // Update label text
  String formatted_text = registry.get_formatted_text();
  label_3d->set_text(formatted_text);

  if (formatted_text.is_empty()) {
    DBG_DEBUG("LabelComponent", "No debug labels registered");
  }
}

void LabelComponent::_update_label_transform() {
  if (!label_3d || !get_unit()) {
    return;
  }

  // Position label above unit
  Vector3 target_position = get_unit()->get_global_position() + label_offset;
  label_3d->set_global_position(target_position);
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

void LabelComponent::set_font_size(int size) {
  font_size = size > 8 ? size : 8;
  if (label_3d) {
    label_3d->set_font_size(font_size);
  }
}

int LabelComponent::get_font_size() const {
  return font_size;
}

void LabelComponent::set_enabled(bool enable) {
  enabled = enable;
  if (label_3d) {
    label_3d->set_visible(enabled);
  }
}

bool LabelComponent::is_enabled() const {
  return enabled;
}

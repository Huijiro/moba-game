#include "data_progress_bar.hpp"

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::Label;
using godot::String;

DataProgressBar::DataProgressBar() = default;

DataProgressBar::~DataProgressBar() = default;

void DataProgressBar::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_value_changed", "value"),
                       &DataProgressBar::_on_value_changed);
}

void DataProgressBar::_ready() {
  // Disable built-in percentage display
  set_show_percentage(false);

  // Create a child Label to display "current/max" text
  Label* label = memnew(Label);
  add_child(label);

  // Center the label
  label->set_anchors_preset(godot::Control::PRESET_CENTER);
  label->set_text("0/0");

  // Connect to value_changed signal to update custom text
  connect("value_changed",
          godot::Callable(this, godot::StringName("_on_value_changed")));

  // Initialize text with current values
  _on_value_changed(get_value());
}

void DataProgressBar::_on_value_changed(double value) {
  // Find the label child and update it
  if (get_child_count() > 0) {
    Label* label = Object::cast_to<Label>(get_child(0));
    if (label) {
      String text = String::num((int)value) + "/" + String::num((int)get_max());
      label->set_text(text);
    }
  }
}

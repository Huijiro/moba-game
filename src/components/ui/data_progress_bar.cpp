#include "data_progress_bar.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::String;

DataProgressBar::DataProgressBar() {
  // Disable show_percentage - we'll use a child Label instead
  set_show_percentage(false);
}

DataProgressBar::~DataProgressBar() = default;

void DataProgressBar::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_value_changed", "value"),
                       &DataProgressBar::_on_value_changed);
}

void DataProgressBar::_ready() {
  // Create a child Label to display "current/max" text
  text_label = memnew(godot::Label);
  add_child(text_label);

  // Center the label
  text_label->set_anchors_preset(godot::Control::PRESET_CENTER);
  text_label->set_text("0/0");

  // Connect to value_changed signal
  connect("value_changed",
          godot::Callable(this, godot::StringName("_on_value_changed")));

  // Initialize with current values
  _on_value_changed(get_value());
}

void DataProgressBar::_on_value_changed(double value) {
  if (text_label) {
    String text = String::num((int)value) + "/" + String::num((int)get_max());
    text_label->set_text(text);
  }
}

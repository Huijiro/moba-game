#include "data_progress_bar.hpp"

#include <godot_cpp/core/class_db.hpp>

using godot::ClassDB;

DataProgressBar::DataProgressBar() {
  // Disable show_percentage by default
  // Users can re-enable it in the editor if desired
  set_show_percentage(false);
}

DataProgressBar::~DataProgressBar() = default;

void DataProgressBar::_bind_methods() {
  // No public methods to bind
}

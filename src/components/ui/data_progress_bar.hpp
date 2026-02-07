#ifndef GDEXTENSION_DATA_PROGRESS_BAR_H
#define GDEXTENSION_DATA_PROGRESS_BAR_H

#include <godot_cpp/classes/progress_bar.hpp>

/// DataProgressBar - custom ProgressBar that displays "current/max" format
///
/// Extends ProgressBar to show text in "current/max" format instead of
/// percentage. The bar value represents current, and max represents the
/// maximum.
///
/// Example display: "100/200"
///
/// The component automatically:
/// 1. Converts bar value and max to integer text
/// 2. Updates label text whenever value or max changes
class DataProgressBar : public godot::ProgressBar {
  GDCLASS(DataProgressBar, godot::ProgressBar)

 protected:
  static void _bind_methods();

  void _on_value_changed(double value);

 public:
  DataProgressBar();
  ~DataProgressBar();

  void _ready() override;
};

#endif  // GDEXTENSION_DATA_PROGRESS_BAR_H

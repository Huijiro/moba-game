#ifndef GDEXTENSION_DATA_PROGRESS_BAR_H
#define GDEXTENSION_DATA_PROGRESS_BAR_H

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/progress_bar.hpp>

using godot::Label;
using godot::ProgressBar;

/// DataProgressBar - ProgressBar with a child Label for custom text display
///
/// Extends ProgressBar to display "current/max" format text in a child Label.
/// Disables show_percentage and manages a child Label for custom text display.
class DataProgressBar : public ProgressBar {
  GDCLASS(DataProgressBar, ProgressBar)

 protected:
  static void _bind_methods();

  Label* text_label = nullptr;
  void _on_value_changed(double value);

 public:
  DataProgressBar();
  ~DataProgressBar();

  void _ready() override;
};

#endif  // GDEXTENSION_DATA_PROGRESS_BAR_H

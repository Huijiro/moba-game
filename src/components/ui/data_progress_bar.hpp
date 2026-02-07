#ifndef GDEXTENSION_DATA_PROGRESS_BAR_H
#define GDEXTENSION_DATA_PROGRESS_BAR_H

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/progress_bar.hpp>

using godot::Label;
using godot::ProgressBar;

/// DataProgressBar - custom ProgressBar that displays "current/max" format
///
/// Extends ProgressBar to show text in "current/max" format instead of
/// percentage. Manages a child Label that displays the custom text while
/// ProgressBar handles the visual bar and layout.
///
/// Example display: "100/200" centered on the progress bar
class DataProgressBar : public ProgressBar {
  GDCLASS(DataProgressBar, ProgressBar)

 private:
  static void _bind_methods();

  Label* text_label = nullptr;

  void _on_value_changed(double value);

 public:
  DataProgressBar();
  ~DataProgressBar();

  void _ready() override;
};

#endif  // GDEXTENSION_DATA_PROGRESS_BAR_H

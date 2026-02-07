#ifndef GDEXTENSION_DATA_PROGRESS_BAR_H
#define GDEXTENSION_DATA_PROGRESS_BAR_H

#include <godot_cpp/classes/progress_bar.hpp>

/// DataProgressBar - ProgressBar that displays "current/max" format instead of
/// percentage
///
/// Extends ProgressBar to override the text rendering to show "current/max"
/// format instead of percentage while keeping all other ProgressBar
/// functionality intact.
class DataProgressBar : public godot::ProgressBar {
  GDCLASS(DataProgressBar, godot::ProgressBar)

 private:
  static void _bind_methods();

 protected:
  void _notification(int p_what);

 public:
  DataProgressBar();
  ~DataProgressBar();
};

#endif  // GDEXTENSION_DATA_PROGRESS_BAR_H

#ifndef GDEXTENSION_DATA_PROGRESS_BAR_H
#define GDEXTENSION_DATA_PROGRESS_BAR_H

#include <godot_cpp/classes/progress_bar.hpp>

/// DataProgressBar - ProgressBar that displays "current/max" format instead of
/// percentage
///
/// This is just a ProgressBar with show_percentage disabled by default.
/// Users can add a child Label with custom formatting if desired, or configure
/// the theme to display custom text. The bar will maintain proper sizing
/// without the show_percentage constraint.
class DataProgressBar : public godot::ProgressBar {
  GDCLASS(DataProgressBar, godot::ProgressBar)

 private:
  static void _bind_methods();

 public:
  DataProgressBar();
  ~DataProgressBar();
};

#endif  // GDEXTENSION_DATA_PROGRESS_BAR_H

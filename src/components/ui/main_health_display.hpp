#ifndef GDEXTENSION_MAIN_HEALTH_DISPLAY_H
#define GDEXTENSION_MAIN_HEALTH_DISPLAY_H

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/progress_bar.hpp>

using godot::Label;
using godot::PanelContainer;
using godot::ProgressBar;

class HealthComponent;

/// MainHealthDisplay - displays main unit's health in the main UI
///
/// This component creates and manages:
/// - A ProgressBar for the health bar visualization
/// - A Label for "current/max" text display
///
/// The component automatically:
/// 1. Finds the main Unit from MatchManager
/// 2. Gets HealthComponent from that unit
/// 3. Connects to health_changed signal
/// 4. Updates bar value and label text in real-time
class MainHealthDisplay : public PanelContainer {
  GDCLASS(MainHealthDisplay, PanelContainer)

 protected:
  static void _bind_methods();

  ProgressBar* health_bar = nullptr;
  Label* health_label = nullptr;
  HealthComponent* health_component = nullptr;

  void _on_health_changed(float current, float max);

 public:
  MainHealthDisplay();
  ~MainHealthDisplay();

  void _ready() override;
};

#endif  // GDEXTENSION_MAIN_HEALTH_DISPLAY_H

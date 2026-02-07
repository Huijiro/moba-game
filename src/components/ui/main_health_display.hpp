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
/// This component shows:
/// - Health bar (current/max health) with built-in percentage display
///
/// Properties:
/// - health_bar_path: NodePath to ProgressBar child (default: "ProgressBar")
///
/// The component automatically:
/// 1. Finds the main Unit from MatchManager
/// 2. Gets HealthComponent from that unit
/// 3. Connects to health_changed signal
/// 4. Updates bar value in real-time
class MainHealthDisplay : public PanelContainer {
  GDCLASS(MainHealthDisplay, PanelContainer)

 protected:
  static void _bind_methods();

  godot::NodePath health_bar_path = "ProgressBar";

  ProgressBar* health_bar = nullptr;
  HealthComponent* health_component = nullptr;

  // Signal handler
  void _on_health_changed(float current, float max);

 public:
  MainHealthDisplay();
  ~MainHealthDisplay();

  void _ready() override;
  godot::Vector2 _get_minimum_size() const override;

  // Properties
  void set_health_bar_path(godot::NodePath path);
  godot::NodePath get_health_bar_path() const;
};

#endif  // GDEXTENSION_MAIN_HEALTH_DISPLAY_H

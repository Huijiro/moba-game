#ifndef GDEXTENSION_MAIN_HEALTH_DISPLAY_H
#define GDEXTENSION_MAIN_HEALTH_DISPLAY_H

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/progress_bar.hpp>

using godot::Label;
using godot::ProgressBar;

class HealthComponent;

/// MainHealthDisplay - displays main unit's health in the main UI
///
/// This component shows:
/// - Health bar (current/max health)
/// - Health text label (e.g., "100/100")
///
/// Properties:
/// - health_bar_path: NodePath to ProgressBar child (default: "HealthBar")
/// - health_label_path: NodePath to Label child (default: "HealthLabel")
///
/// The component automatically:
/// 1. Finds the main Unit from MatchManager
/// 2. Gets HealthComponent from that unit
/// 3. Connects to health_changed signal
/// 4. Updates bar value and label text in real-time
class MainHealthDisplay : public godot::Control {
  GDCLASS(MainHealthDisplay, godot::Control)

 protected:
  static void _bind_methods();

  godot::NodePath health_bar_path = "HealthBar";
  godot::NodePath health_label_path = "HealthLabel";

  ProgressBar* health_bar = nullptr;
  Label* health_label = nullptr;
  HealthComponent* health_component = nullptr;

  // Signal handler
  void _on_health_changed(float current, float max);

 public:
  MainHealthDisplay();
  ~MainHealthDisplay();

  void _ready() override;

  // Properties
  void set_health_bar_path(godot::NodePath path);
  godot::NodePath get_health_bar_path() const;

  void set_health_label_path(godot::NodePath path);
  godot::NodePath get_health_label_path() const;
};

#endif  // GDEXTENSION_MAIN_HEALTH_DISPLAY_H

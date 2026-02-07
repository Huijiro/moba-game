#ifndef GDEXTENSION_COOLDOWN_DISPLAY_COMPONENT_H
#define GDEXTENSION_COOLDOWN_DISPLAY_COMPONENT_H

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/label.hpp>
#include <vector>

using godot::CanvasLayer;
using godot::Label;
using godot::String;
using godot::StringName;

class Unit;
class AbilityComponent;
class MatchManager;

/// CooldownDisplayComponent - displays ability cooldowns in a fixed HUD
/// position
///
/// This component is a signal orchestrator with NO appearance logic.
/// All visual configuration happens in the editor scene.
///
/// This component:
/// 1. Gets the main Unit from MatchManager
/// 2. Finds child Label nodes for ability slots
/// 3. Listens to ability_cooldown_started signal to begin tracking
/// 4. Maintains local cooldown timers and updates labels
/// 5. Listens to cooldown_changed signal for forced updates
///
/// Child nodes required (configurable in editor):
/// - Any number of Label nodes (one per ability slot to display)
///   Example: QLabel, WLabel, ELabel, RLabel
///
/// The labels are matched to ability slots by order in the array.
/// Each label shows remaining cooldown time:
/// - >= 1s: "3s", "2s", "1s"
/// - < 1s: "0.9s", "0.5s", "0.1s"
/// - <= 0s: "" (empty, no text)
class CooldownDisplayComponent : public CanvasLayer {
  GDCLASS(CooldownDisplayComponent, CanvasLayer)

 protected:
  static void _bind_methods();

  // Configuration
  godot::Array ability_slot_label_paths;  // Array of NodePaths to labels

  // Cooldown tracking
  struct CooldownTimer {
    int slot = -1;
    float duration = 0.0f;
    float elapsed = 0.0f;
    bool active = false;
  };
  std::vector<CooldownTimer> cooldown_timers;

  // References
  MatchManager* match_manager = nullptr;
  Unit* main_unit = nullptr;
  AbilityComponent* ability_component = nullptr;
  std::vector<Label*> cooldown_labels;

  // Signal handlers
  void _on_cooldown_started(int slot, float duration);
  void _on_cooldown_changed(int slot);

 public:
  CooldownDisplayComponent();
  ~CooldownDisplayComponent();

  void _ready() override;
  void _process(double delta) override;

  // Properties
  void set_ability_slot_label_paths(const godot::Array& paths);
  godot::Array get_ability_slot_label_paths() const;

 private:
  // Helper methods
  String _format_cooldown(float remaining) const;
  void _update_label(int slot_index, float remaining);
};

#endif  // GDEXTENSION_COOLDOWN_DISPLAY_COMPONENT_H

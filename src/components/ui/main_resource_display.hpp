#ifndef GDEXTENSION_MAIN_RESOURCE_DISPLAY_H
#define GDEXTENSION_MAIN_RESOURCE_DISPLAY_H

#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/variant/string_name.hpp>

using godot::Label;
using godot::PanelContainer;
using godot::ProgressBar;
using godot::StringName;

class ResourcePoolComponent;

/// MainResourceDisplay - displays main unit's resource pool in the main UI
///
/// This component shows:
/// - Resource bar (current/max resource) with "current/max" text display
///
/// Properties:
/// - pool_id: Which resource pool to display (e.g., "mana") (default: "mana")
/// - resource_bar_path: NodePath to DataProgressBar child (default:
/// "ProgressBar")
///
/// The component automatically:
/// 1. Finds the main Unit from MatchManager
/// 2. Gets ResourcePoolComponent with matching pool_id
/// 3. Connects to value_changed signal
/// 4. Updates bar value in real-time (DataProgressBar handles text)
class MainResourceDisplay : public PanelContainer {
  GDCLASS(MainResourceDisplay, PanelContainer)

 protected:
  static void _bind_methods();

  StringName pool_id = "mana";
  godot::NodePath resource_bar_path = "ProgressBar";

  ProgressBar* resource_bar = nullptr;
  ResourcePoolComponent* resource_pool = nullptr;

  // Signal handler
  void _on_resource_changed(float current, float max);

 public:
  MainResourceDisplay();
  ~MainResourceDisplay();

  void _ready() override;
  godot::Vector2 _get_minimum_size() const override;

  // Properties
  void set_pool_id(StringName id);
  StringName get_pool_id() const;

  void set_resource_bar_path(godot::NodePath path);
  godot::NodePath get_resource_bar_path() const;
};

#endif  // GDEXTENSION_MAIN_RESOURCE_DISPLAY_H

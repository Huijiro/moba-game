#ifndef GDEXTENSION_MAIN_RESOURCE_DISPLAY_H
#define GDEXTENSION_MAIN_RESOURCE_DISPLAY_H

#include <godot_cpp/classes/container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/variant/string_name.hpp>

using godot::Container;
using godot::Label;
using godot::ProgressBar;
using godot::StringName;

class ResourcePoolComponent;

/// MainResourceDisplay - displays main unit's resource pool in the main UI
///
/// This component shows:
/// - Resource bar (current/max resource)
/// - Resource text label (e.g., "50/100")
///
/// Properties:
/// - pool_id: Which resource pool to display (e.g., "mana") (default: "mana")
/// - resource_bar_path: NodePath to ProgressBar child (default: "ResourceBar")
/// - resource_label_path: NodePath to Label child (default: "ResourceLabel")
///
/// The component automatically:
/// 1. Finds the main Unit from MatchManager
/// 2. Gets ResourcePoolComponent with matching pool_id
/// 3. Connects to value_changed signal
/// 4. Updates bar value and label text in real-time
class MainResourceDisplay : public Container {
  GDCLASS(MainResourceDisplay, Container)

 protected:
  static void _bind_methods();

  StringName pool_id = "mana";
  godot::NodePath resource_bar_path = "ResourceBar";
  godot::NodePath resource_label_path = "ResourceLabel";

  ProgressBar* resource_bar = nullptr;
  Label* resource_label = nullptr;
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

  void set_resource_label_path(godot::NodePath path);
  godot::NodePath get_resource_label_path() const;
};

#endif  // GDEXTENSION_MAIN_RESOURCE_DISPLAY_H

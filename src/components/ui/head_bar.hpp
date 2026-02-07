#ifndef GDEXTENSION_HEAD_BAR_H
#define GDEXTENSION_HEAD_BAR_H

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <map>
#include <vector>

using godot::Camera3D;
using godot::Control;
using godot::Label;
using godot::ProgressBar;
using godot::StringName;
using godot::Vector2;
using godot::Vector3;

class Unit;
class HealthComponent;
class ResourcePoolComponent;

/// HeadBar component - displays unit name, health, and resource pools above a
/// unit
///
/// This component is a signal orchestrator with NO appearance logic.
/// All visual configuration happens in the editor scene.
///
/// Child nodes required:
/// - UnitNameLabel (Label) - Shows unit name
/// - HealthBar (ProgressBar) - Shows health bar
/// - Resource* (ProgressBar) - One per resource pool (e.g., ResourceMana,
/// ResourceEnergy)
///
/// The component:
/// 1. Finds child nodes by configured NodePaths
/// 2. Connects to parent component signals
/// 3. Updates ProgressBar values only
/// 4. Dynamically discovers ResourcePoolComponents and matches them to
/// Resource* children
class HeadBar : public Control {
  GDCLASS(HeadBar, Control)

 protected:
  static void _bind_methods();

  // NodePaths to children (configurable in editor)
  godot::NodePath unit_name_label_path = "UnitNameLabel";
  godot::NodePath health_bar_path = "HealthBar";

  // References to parent components
  Unit* owner_unit = nullptr;
  HealthComponent* health_component = nullptr;
  std::vector<ResourcePoolComponent*> resource_pools;

  // References to child nodes
  Label* unit_name_label = nullptr;
  ProgressBar* health_bar = nullptr;
  std::vector<ProgressBar*> resource_bars;
  std::vector<StringName> resource_pool_ids;

  // Camera for world-to-screen positioning
  Camera3D* camera = nullptr;

  // Signal handlers
  void _on_health_changed(float current, float max);
  void _on_resource_changed(StringName pool_id, float current, float max);
  void _reparent_to_game_ui(godot::Object* game_ui);

 public:
  HeadBar();
  ~HeadBar();

  void _ready() override;
  void _process(double delta) override;

  // Properties
  void set_unit_name_label_path(const godot::NodePath& path);
  godot::NodePath get_unit_name_label_path() const;

  void set_health_bar_path(const godot::NodePath& path);
  godot::NodePath get_health_bar_path() const;
};

#endif  // GDEXTENSION_HEAD_BAR_H

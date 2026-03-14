#ifndef CLICK_MARKER_HANDLER_HPP
#define CLICK_MARKER_HANDLER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

/// Visual feedback for move clicks — spawns a marker at the click position
/// and fades it out over time.
///
/// Listens to the controlled unit's move_requested signal.
class ClickMarkerHandler : public godot::Node {
  GDCLASS(ClickMarkerHandler, godot::Node);

 public:
  ClickMarkerHandler();
  ~ClickMarkerHandler();

  void _ready() override;
  void _process(double delta) override;

  void set_click_indicator_scene(const godot::Ref<godot::PackedScene>& scene);
  godot::Ref<godot::PackedScene> get_click_indicator_scene() const;

  void set_fade_duration(float duration);
  float get_fade_duration() const;

  /// Show a marker at the given world position.
  void show_at(const godot::Vector3& position);

 protected:
  static void _bind_methods();

 private:
  godot::Ref<godot::PackedScene> click_indicator_scene;
  float fade_duration = 2.0f;

  godot::Node3D* marker = nullptr;
  godot::Ref<godot::StandardMaterial3D> marker_material;
  float fade_timer = 0.0f;
  bool active = false;

  void _on_move_requested(const godot::Vector3& position);
};

#endif  // CLICK_MARKER_HANDLER_HPP

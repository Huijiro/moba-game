#ifndef HOVER_HIGHLIGHT_HPP
#define HOVER_HIGHLIGHT_HPP

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/variant/color.hpp>

class Unit;
class CursorWorldQuery;

/// Highlights enemy units when the cursor hovers over them.
///
/// Each frame, reads the cursor query result and applies/removes a
/// glow overlay mesh on the hovered unit.
class HoverHighlight : public godot::Node {
  GDCLASS(HoverHighlight, godot::Node);

 public:
  HoverHighlight();
  ~HoverHighlight();

  void _ready() override;
  void _process(double delta) override;

  void set_glow_color(const godot::Color& color);
  godot::Color get_glow_color() const;

  void set_controlled_unit(Unit* unit);
  Unit* get_controlled_unit() const;

 protected:
  static void _bind_methods();

 private:
  CursorWorldQuery* cursor_query = nullptr;
  Unit* controlled_unit = nullptr;
  Unit* hovered_unit = nullptr;
  godot::MeshInstance3D* glow_overlay = nullptr;
  godot::Color glow_color = godot::Color(0.3f, 0.8f, 1.0f, 0.3f);

  void _apply_glow(Unit* unit);
  void _remove_glow();
};

#endif  // HOVER_HIGHLIGHT_HPP

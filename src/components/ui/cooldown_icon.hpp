#ifndef GDEXTENSION_COOLDOWN_ICON_H
#define GDEXTENSION_COOLDOWN_ICON_H

#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/vector2.hpp>

using godot::StringName;
using godot::TextureRect;
using godot::Vector2;

class AbilityComponent;

/// CooldownIcon - displays an ability icon with a visual cooldown swipe overlay
///
/// This component shows:
/// - Ability icon centered
/// - Visual progress overlay during cooldown (animated swipe from bottom)
/// - Cooldown text in the corner (e.g., "2.5s")
///
/// Properties:
/// - ability_slot: Which ability slot to display (0-3 for Q/W/E/R)
/// - icon_size: Size of the icon in pixels (default: 50)
///
/// The component automatically:
/// 1. Finds the AbilityComponent on the main unit via MatchManager
/// 2. Gets the ability icon from that slot
/// 3. Draws a visual cooldown overlay during cooldown
/// 4. Updates in real-time via ability cooldown signals
class CooldownIcon : public TextureRect {
  GDCLASS(CooldownIcon, TextureRect)

 protected:
  static void _bind_methods();

  int ability_slot = 0;
  Vector2 icon_size = Vector2(50, 50);

  AbilityComponent* ability_component = nullptr;

  // Cooldown tracking
  float cooldown_duration = 0.0f;
  float cooldown_elapsed = 0.0f;
  bool on_cooldown = false;

  // Signal handlers
  void _on_cooldown_started(int slot, float duration);
  void _on_cooldown_changed(int slot);

  // Drawing
  void _draw_cooldown_overlay();

  // Debug
  void _log_debug_position();

 public:
  CooldownIcon();
  ~CooldownIcon();

  void _ready() override;
  void _process(double delta) override;
  void _draw() override;

  // Properties
  void set_ability_slot(int slot);
  int get_ability_slot() const;

  void set_icon_size(Vector2 size);
  Vector2 get_icon_size() const;
};

#endif  // GDEXTENSION_COOLDOWN_ICON_H

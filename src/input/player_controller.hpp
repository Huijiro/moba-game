#ifndef PLAYER_CONTROLLER_HPP
#define PLAYER_CONTROLLER_HPP

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

class Unit;
class CursorWorldQuery;
class AbilityTargetingHandler;
class ClickMarkerHandler;

/// Translates player input into unit commands.
///
/// Reads cursor state from CursorWorldQuery (sibling node).
/// Issues commands: move_requested, attack_requested, stop_requested,
/// cast_ability. Delegates click-to-cast targeting to AbilityTargetingHandler.
///
/// This node is the only one that reads raw input events.
/// A NetworkController or AIController could issue the same relay signals.
class PlayerController : public godot::Node {
  GDCLASS(PlayerController, godot::Node);

 public:
  PlayerController();
  ~PlayerController();

  void _ready() override;
  void _input(const godot::Ref<godot::InputEvent>& event) override;

  void set_controlled_unit(Unit* unit);
  Unit* get_controlled_unit() const;

  void set_move_action(const godot::String& action);
  godot::String get_move_action() const;

  void set_cast_action(const godot::String& action);
  godot::String get_cast_action() const;

 protected:
  static void _bind_methods();

 private:
  Unit* controlled_unit = nullptr;
  CursorWorldQuery* cursor_query = nullptr;
  AbilityTargetingHandler* targeting_handler = nullptr;
  ClickMarkerHandler* click_marker = nullptr;

  godot::String move_action = "game_move";
  godot::String cast_action = "game_cast";

  // Keybinds: action name → ability slot
  godot::Dictionary keybind_map;

  void _handle_ability_key(const godot::String& action_name);
  void _handle_move_click();
  void _handle_stop();
  void _init_default_keybinds();
};

#endif  // PLAYER_CONTROLLER_HPP

#ifndef ABILITY_TARGETING_HANDLER_HPP
#define ABILITY_TARGETING_HANDLER_HPP

#include <godot_cpp/classes/node.hpp>

class Unit;
class AbilityComponent;
class CursorWorldQuery;

/// Manages the click-to-cast targeting state machine.
///
/// When the player presses an ability key in CLICK_TO_CAST mode,
/// PlayerController calls start(slot). This node then waits for a
/// left-click, validates targeting type (unit vs point), and issues
/// the cast_ability relay when the player clicks a valid target.
class AbilityTargetingHandler : public godot::Node {
  GDCLASS(AbilityTargetingHandler, godot::Node);

 public:
  AbilityTargetingHandler();
  ~AbilityTargetingHandler();

  /// Enter targeting mode for the given ability slot.
  void start(int ability_slot, Unit* caster);

  /// Process a left-click while in targeting mode.
  void handle_click(CursorWorldQuery* cursor);

  /// Cancel targeting mode.
  void cancel();

  /// Is currently waiting for a target click?
  bool is_active() const;

  /// Which slot is being targeted (-1 if none).
  int get_active_slot() const;

 protected:
  static void _bind_methods();

 private:
  int active_slot = -1;
  bool awaiting_unit_target = false;
  Unit* caster = nullptr;

  AbilityComponent* _find_ability_component(Unit* unit) const;
};

#endif  // ABILITY_TARGETING_HANDLER_HPP

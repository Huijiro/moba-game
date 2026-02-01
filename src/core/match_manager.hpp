#ifndef GDEXTENSION_MATCH_MANAGER_H
#define GDEXTENSION_MATCH_MANAGER_H

#include <godot_cpp/classes/node.hpp>

using godot::Node;

class InputManager;
class MOBACamera;
class Unit;

class MatchManager : public Node {
  GDCLASS(MatchManager, Node)

 protected:
  static void _bind_methods();

 public:
  MatchManager();
  ~MatchManager();

  void _ready() override;

  void set_main_unit(Unit* unit);
  Unit* get_main_unit() const;

  void set_player_controller(InputManager* controller);
  InputManager* get_player_controller() const;

  void set_moba_camera(MOBACamera* camera);
  MOBACamera* get_moba_camera() const;

  // Game settings - channel interrupt behavior
  void set_channel_requires_stop_command_only(bool value);
  bool get_channel_requires_stop_command_only() const;

 private:
  Unit* main_unit = nullptr;
  InputManager* player_controller = nullptr;
  MOBACamera* moba_camera = nullptr;

  // Game settings
  // If true: channels only stop with S (stop) command
  // If false: channels also stop on ability key release
  bool channel_requires_stop_command_only = true;
};

#endif  // GDEXTENSION_MATCH_MANAGER_H

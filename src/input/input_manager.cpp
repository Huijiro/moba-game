#include "input_manager.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include "../common/unit_signals.hpp"
#include "../components/abilities/ability_component.hpp"
#include "../components/abilities/ability_node.hpp"
#include "../components/interaction/interactable.hpp"
#include "../core/game_settings.hpp"
#include "../core/unit.hpp"
#include "../debug/debug_macros.hpp"
#include "../debug/visual_debugger.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Dictionary;
using godot::Engine;
using godot::InputEventMouseButton;
using godot::MOUSE_BUTTON_RIGHT;
using godot::Node;
using godot::Node3D;
using godot::PhysicsRayQueryParameters3D;
using godot::PropertyInfo;
using godot::String;
using godot::UtilityFunctions;
using godot::Variant;
using godot::Vector2;

InputManager::InputManager() = default;

InputManager::~InputManager() {
  if (click_marker != nullptr) {
    click_marker->queue_free();
    click_marker = nullptr;
  }
}

void InputManager::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_controlled_unit", "unit"),
                       &InputManager::set_controlled_unit);
  ClassDB::bind_method(D_METHOD("get_controlled_unit"),
                       &InputManager::get_controlled_unit);

  ClassDB::bind_method(D_METHOD("set_camera", "camera"),
                       &InputManager::set_camera);
  ClassDB::bind_method(D_METHOD("get_camera"), &InputManager::get_camera);

  ClassDB::bind_method(D_METHOD("set_raycast_distance", "distance"),
                       &InputManager::set_raycast_distance);
  ClassDB::bind_method(D_METHOD("get_raycast_distance"),
                       &InputManager::get_raycast_distance);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "camera",
                            godot::PROPERTY_HINT_NODE_TYPE, "Camera3D"),
               "set_camera", "get_camera");

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "raycast_distance"),
               "set_raycast_distance", "get_raycast_distance");

  ClassDB::bind_method(D_METHOD("set_click_indicator_scene", "scene"),
                       &InputManager::set_click_indicator_scene);
  ClassDB::bind_method(D_METHOD("get_click_indicator_scene"),
                       &InputManager::get_click_indicator_scene);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "click_indicator_scene",
                            godot::PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"),
               "set_click_indicator_scene", "get_click_indicator_scene");

  // Input action configuration
  ClassDB::bind_method(D_METHOD("set_move_action", "action"),
                       &InputManager::set_move_action);
  ClassDB::bind_method(D_METHOD("get_move_action"),
                       &InputManager::get_move_action);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "move_action"), "set_move_action",
               "get_move_action");

  ClassDB::bind_method(D_METHOD("set_cast_action", "action"),
                       &InputManager::set_cast_action);
  ClassDB::bind_method(D_METHOD("get_cast_action"),
                       &InputManager::get_cast_action);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "cast_action"), "set_cast_action",
               "get_cast_action");

  // Keybind methods
  ClassDB::bind_method(D_METHOD("bind_ability_to_key", "key", "ability_slot"),
                       &InputManager::bind_ability_to_key);
  ClassDB::bind_method(D_METHOD("unbind_key", "key"),
                       &InputManager::unbind_key);
  ClassDB::bind_method(D_METHOD("get_bound_ability", "key"),
                       &InputManager::get_bound_ability);
}

void InputManager::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Auto-find camera if not set
  if (camera == nullptr) {
    camera = Object::cast_to<Camera3D>(get_viewport()->get_camera_3d());
  }

  // Auto-find Unit if not set - search parent
  if (controlled_unit == nullptr) {
    Node* parent = get_parent();
    if (parent != nullptr) {
      controlled_unit = Object::cast_to<Unit>(parent);
    }
  }

  // Initialize default keybinds
  _init_default_keybinds();
}

void InputManager::_input(const Ref<InputEvent>& event) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (controlled_unit == nullptr) {
    return;
  }

  // Check for stop command (S key)
  if (event->is_action_pressed(godot::StringName("game_stop"))) {
    _handle_stop_command();
    get_viewport()->set_input_as_handled();
    return;
  }

  // Check for keyboard input (ability slots 1-6)
  // Support up to 6 ability slots via game_ability_1 through game_ability_6
  // actions
  for (int i = 1; i <= 6; i++) {
    String action_name =
        String("game_ability_") + String::num(i, 0);  // Force integer format
    StringName action_sname = godot::StringName(action_name);

    // Check for ability key press
    if (event->is_action_pressed(action_sname)) {
      // Cancel any existing targeting state when pressing a new ability key
      _cancel_targeting();
      _handle_ability_input(action_name);
      break;  // Only process one ability key per input event
    }
  }

  // Check for mouse button click
  auto mouse_event = Object::cast_to<InputEventMouseButton>(event.ptr());
  if (mouse_event == nullptr) {
    return;
  }

  if (!mouse_event->is_pressed()) {
    return;
  }

  // Check which action was triggered
  bool is_cast_action =
      event->is_action_pressed(godot::StringName(cast_action));
  bool is_move_action =
      event->is_action_pressed(godot::StringName(move_action));

  // If waiting for ability target, handle click for ability
  if (awaiting_target_slot >= 0 && is_cast_action) {
    Vector3 click_position;
    godot::Object* clicked_object = nullptr;
    if (_try_raycast(click_position, clicked_object)) {
      if (is_awaiting_unit_target) {
        // Unit-target ability: only cast if valid target clicked
        if (clicked_object != nullptr) {
          controlled_unit->relay(cast_ability_unit_target, awaiting_target_slot,
                                 clicked_object);
          awaiting_target_slot = -1;
        } else {
          // No target clicked - stay in targeting mode
          DBG_INFO("InputManager", "No valid target. Click on a unit.");
          return;
        }
      } else {
        // Position-target or skillshot ability: cast at clicked position
        controlled_unit->relay(cast_ability_point_target, awaiting_target_slot,
                               click_position);
        // Clear targeting mode
        awaiting_target_slot = -1;
      }

      DBG_INFO("InputManager", "Cast ability at target");
      get_viewport()->set_input_as_handled();
      return;
    }
    return;
  }

  // Movement/attack handling - only if we're NOT using cast action
  // This prevents clicking to cast an ability from also triggering a
  // movement/attack
  if (!is_move_action || is_cast_action) {
    return;
  }

  // Cancel any existing targeting state when attempting movement/attack
  _cancel_targeting();

  Vector3 click_position;
  godot::Object* clicked_object = nullptr;
  if (_try_raycast(click_position, clicked_object)) {
    if (auto clicked_unit = Object::cast_to<Unit>(clicked_object)) {
      if (clicked_unit == controlled_unit) {
        // Ignore right-clicks on the main unit itself.
        get_viewport()->set_input_as_handled();
        return;
      }

      // Allies: do nothing.
      if (clicked_unit->get_faction_id() == controlled_unit->get_faction_id()) {
        get_viewport()->set_input_as_handled();
        return;
      }

      // Enemies: relay ATTACK order
      controlled_unit->relay(attack_requested, clicked_unit,
                             clicked_unit->get_global_position());
      DBG_INFO("InputManager",
               "Issued ATTACK order on: " + String(clicked_unit->get_name()));
      get_viewport()->set_input_as_handled();
      return;
    }

    if (auto clicked_interactable =
            Object::cast_to<Interactable>(clicked_object)) {
      DBG_INFO("InputManager", "Clicked Interactable: " +
                                   String(clicked_interactable->get_name()));
      get_viewport()->set_input_as_handled();
      return;
    }

    // Default: treat as terrain/world click.
    controlled_unit->relay(move_requested, click_position);
    _show_click_marker(click_position);
    get_viewport()->set_input_as_handled();
  }
}

void InputManager::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  _update_click_marker(delta);
  _update_hover_glow();

  // Visual debugging: Draw aiming line and AoE circle while targeting
  if (awaiting_target_slot >= 0 && controlled_unit != nullptr &&
      camera != nullptr) {
    VisualDebugger* debugger = VisualDebugger::get_singleton();
    if (debugger != nullptr && debugger->is_debug_enabled()) {
      // Get mouse position and raycast to ground
      Vector3 mouse_pos;
      godot::Object* dummy = nullptr;
      if (_try_raycast(mouse_pos, dummy)) {
        // Visual debugging of ability aiming would require querying the
        // ability's AoE radius. Since we're now fully signal-based, we'd need
        // to add a signal for this. For now, we skip the visual aiming preview.
      }
    }
  }
}

void InputManager::set_controlled_unit(Unit* unit) {
  controlled_unit = unit;
}

Unit* InputManager::get_controlled_unit() const {
  return controlled_unit;
}

void InputManager::set_camera(Camera3D* cam) {
  camera = cam;
}

Camera3D* InputManager::get_camera() const {
  return camera;
}

void InputManager::set_raycast_distance(float distance) {
  raycast_distance = distance;
}

float InputManager::get_raycast_distance() const {
  return raycast_distance;
}

void InputManager::set_click_indicator_scene(const Ref<PackedScene>& scene) {
  click_indicator_scene = scene;
}

Ref<PackedScene> InputManager::get_click_indicator_scene() const {
  return click_indicator_scene;
}

void InputManager::set_move_action(const String& action) {
  move_action = action;
  DBG_INFO("InputManager", "Move action set to: " + action);
}

String InputManager::get_move_action() const {
  return move_action;
}

void InputManager::set_cast_action(const String& action) {
  cast_action = action;
  DBG_INFO("InputManager", "Cast action set to: " + action);
}

String InputManager::get_cast_action() const {
  return cast_action;
}

bool InputManager::_try_raycast(Vector3& out_position,
                                godot::Object*& out_collider) {
  out_collider = nullptr;

  if (camera == nullptr) {
    return false;
  }

  // Get the current physics space from the camera (which is a Node3D)
  Node3D* camera_node = Object::cast_to<Node3D>(camera);
  if (camera_node == nullptr) {
    return false;
  }

  auto world = camera_node->get_world_3d();
  if (world == nullptr) {
    return false;
  }

  physics_state = world->get_direct_space_state();
  if (physics_state == nullptr) {
    return false;
  }

  // Get mouse position and convert to world space
  auto viewport = get_viewport();
  if (viewport == nullptr) {
    return false;
  }

  Vector2 mouse_pos = viewport->get_mouse_position();

  // Create ray from camera through mouse position
  Vector3 ray_from = camera->project_ray_origin(mouse_pos);
  Vector3 ray_normal = camera->project_ray_normal(mouse_pos);
  Vector3 ray_to = ray_from + (ray_normal * raycast_distance);

  // Setup raycast query
  Ref<PhysicsRayQueryParameters3D> query =
      PhysicsRayQueryParameters3D::create(ray_from, ray_to);
  query->set_collide_with_bodies(true);
  query->set_collide_with_areas(true);

  // Avoid hitting the player's own unit.
  if (controlled_unit != nullptr && controlled_unit->is_inside_tree()) {
    godot::Array exclude;
    exclude.push_back(controlled_unit->get_rid());
    query->set_exclude(exclude);
  }

  // Execute raycast
  Dictionary result = physics_state->intersect_ray(query);

  if (result.is_empty()) {
    out_collider = nullptr;
    return false;
  }

  out_position = result["position"];
  Object* collider = result["collider"];

  // Try to get the Unit that owns this collider
  // If we hit a CollisionShape, traverse up to find the Unit
  Node* node = Object::cast_to<Node>(collider);
  while (node != nullptr) {
    Unit* unit = Object::cast_to<Unit>(node);
    if (unit != nullptr) {
      out_collider = unit;
      return true;
    }
    node = node->get_parent();
  }

  // If not part of a Unit, return the collider as-is
  out_collider = collider;
  return true;
}

void InputManager::_show_click_marker(const Vector3& position) {
  // Clean up old marker if it exists
  if (click_marker != nullptr) {
    click_marker->queue_free();
  }

  // Check if scene is set
  if (click_indicator_scene.is_null()) {
    return;  // No scene configured
  }

  // Instantiate the click indicator scene
  Node* marker_instance = click_indicator_scene->instantiate();
  auto marker = Object::cast_to<Node3D>(marker_instance);
  if (marker == nullptr) {
    return;  // Scene root is not a Node3D
  }

  // Add to scene FIRST, then set position
  Node* parent = get_parent();
  if (parent != nullptr) {
    parent->add_child(marker);
  }

  // Position at click location (after adding to tree)
  Vector3 marker_pos = position;
  marker_pos.y += 0.5f;
  marker->set_global_position(marker_pos);

  click_marker = marker;
  marker_active = true;
  marker_fade_timer = 0.0f;
}

void InputManager::_update_click_marker(double delta) {
  if (click_marker == nullptr) {
    return;
  }

  if (!marker_active) {
    return;
  }

  marker_fade_timer += delta;

  // Fade out the marker
  if (marker_fade_timer < marker_fade_duration) {
    float progress = marker_fade_timer / marker_fade_duration;
    float alpha = 1.0f - progress;  // Fade from 1 to 0

    if (marker_material.is_valid()) {
      godot::Color color = marker_material->get_albedo();
      color.a = alpha;
      marker_material->set_albedo(color);
    }
  } else {
    // Remove marker after fade is complete
    click_marker->queue_free();
    click_marker = nullptr;
    marker_material.unref();
    marker_active = false;
    marker_fade_timer = 0.0f;
  }
}

void InputManager::bind_ability_to_key(const String& key, int ability_slot) {
  if (ability_slot < 0 || ability_slot > 3) {
    DBG_INFO("InputManager",
             "Invalid ability slot: " + String::num(ability_slot));
    return;
  }
  keybind_map[key] = ability_slot;
  DBG_INFO("InputManager",
           "Bound " + key + " to ability slot " + String::num(ability_slot));
}

void InputManager::unbind_key(const String& key) {
  if (keybind_map.has(key)) {
    keybind_map.erase(key);
    DBG_INFO("InputManager", "Unbound " + key);
  }
}

int InputManager::get_bound_ability(const String& key) const {
  if (keybind_map.has(key)) {
    return keybind_map[key];
  }
  return -1;  // Not bound
}

void InputManager::_init_default_keybinds() {
  // Initialize keybinds for standard ability slots (0-3)
  // Ability information is no longer queried directly
  if (controlled_unit == nullptr) {
    return;
  }

  // Standard MOBA has 4 ability slots (Q, W, E, R typically)
  int ability_count = 4;
  for (int i = 0; i < ability_count; i++) {
    String action_name =
        String("game_ability_") +
        String::num(i + 1, 0);  // Force integer format (no decimals)
    keybind_map[action_name] = i;
    String key_name = _get_key_name_for_action(action_name);
    DBG_INFO("InputManager", "Ability slot " + String::num(i + 1, 0) +
                                 " bound to key: " + key_name);
  }

  DBG_INFO("InputManager", "Initialized keybinds for " +
                               String::num(ability_count) + " ability slots");
}

void InputManager::_handle_ability_input(const String& key) {
  if (controlled_unit == nullptr) {
    return;
  }

  // Look up which ability slot this key is bound to
  int ability_slot = get_bound_ability(key);
  if (ability_slot < 0 || ability_slot > 3) {
    return;  // Key not bound to an ability
  }

  CastingMode casting_mode = GameSettings::get_casting_mode_enum();

  // Handle based on casting mode
  // Note: We no longer query the ability directly for targeting type.
  // Instead, we assume abilities are configured consistently and use
  // the casting mode to determine behavior.
  switch (casting_mode) {
    case CastingMode::INSTANT: {
      // Instant cast mode - cast at current cursor position
      Vector3 cursor_position;
      godot::Object* cursor_target = nullptr;
      if (_try_raycast(cursor_position, cursor_target)) {
        // Try casting on unit target first if available
        if (cursor_target != nullptr) {
          controlled_unit->relay(cast_ability_unit_target, ability_slot,
                                 cursor_target);
          DBG_INFO("InputManager", "Instant cast on unit target: slot=" +
                                       String::num(ability_slot));
        } else {
          // Otherwise cast at position
          DBG_INFO("InputManager",
                   "Raycast hit at: (" + String::num(cursor_position.x, 2) +
                       ", " + String::num(cursor_position.y, 2) + ", " +
                       String::num(cursor_position.z, 2) + ")");
          controlled_unit->relay(cast_ability_point_target, ability_slot,
                                 cursor_position);
          DBG_INFO("InputManager", "Instant cast at position: slot=" +
                                       String::num(ability_slot));
        }
      } else {
        DBG_INFO("InputManager", "Cannot cast - no valid target position");
      }
      break;
    }

    case CastingMode::CLICK_TO_CAST: {
      // Click to cast mode - wait for user to click
      // We assume unit-target for now; the actual targeting type is
      // defined in the ability configuration
      _enter_ability_targeting_mode(ability_slot, 0);  // 0 = assume unit-target
      break;
    }

    case CastingMode::INDICATOR: {
      // Indicator cast mode - start charging
      indicator_ability_slot = ability_slot;
      indicator_charging = true;
      indicator_charge_time = 0.0;
      DBG_INFO("InputManager", "Indicator charging for ability slot " +
                                   String::num(ability_slot) +
                                   " - release to cast");
      break;
    }
  }
}

void InputManager::_enter_ability_targeting_mode(int ability_slot,
                                                 int targeting_type) {
  awaiting_target_slot = ability_slot;
  is_awaiting_unit_target = (targeting_type == 0);  // UNIT_TARGET requires unit

  if (targeting_type == 0) {  // UNIT_TARGET
    DBG_INFO("InputManager", "Ability slot " + String::num(ability_slot) +
                                 " waiting for unit target - click on target");
  } else if (targeting_type == 3) {  // SKILLSHOT
    DBG_INFO("InputManager",
             "Ability slot " + String::num(ability_slot) +
                 " waiting for skillshot direction - click to aim");
  } else {  // POINT_TARGET (1), AREA (2), or other position-based
    DBG_INFO("InputManager",
             "Ability slot " + String::num(ability_slot) +
                 " waiting for position target - click to cast");
  }
}

void InputManager::_handle_stop_command() {
  if (controlled_unit == nullptr) {
    return;
  }

  bool did_stop_anything = false;

  // Cancel any ability targeting
  if (awaiting_target_slot >= 0) {
    DBG_INFO("InputManager",
             "Stop command: Cancelled targeting mode for ability " +
                 String::num(awaiting_target_slot));
    awaiting_target_slot = -1;
    is_awaiting_unit_target = false;
    did_stop_anything = true;
  }

  // Note: Interrupting channel abilities would require querying ability state.
  // Since we're now signal-based, channel interruption should be handled
  // through a separate signal if needed. For now, we just cancel targeting mode
  // above.

  // Cancel any movement orders - relay STOP order
  controlled_unit->relay(stop_requested);
  DBG_INFO("InputManager", "Stop command: Cancelled movement");
  did_stop_anything = true;

  if (!did_stop_anything) {
    DBG_INFO("InputManager", "Stop command: Nothing to stop");
  }
}

void InputManager::_cancel_targeting() {
  // Cancel the "ready to cast" state when player takes any action
  if (awaiting_target_slot >= 0) {
    DBG_INFO("InputManager", "Cancelled targeting mode for ability " +
                                 String::num(awaiting_target_slot) +
                                 " (action taken)");
    awaiting_target_slot = -1;
    is_awaiting_unit_target = false;
  }
}

String InputManager::_get_key_name_for_action(const String& action) {
  // Get the actual key bound to this action from Godot's InputMap
  godot::InputMap* input_map = godot::InputMap::get_singleton();
  if (input_map == nullptr) {
    return "UNBOUND";
  }

  // Get the list of input events for this action
  godot::Array events = input_map->action_get_events(action);
  if (events.size() == 0) {
    return "UNBOUND";
  }

  // Get the first event (usually a key press)
  godot::Ref<godot::InputEventKey> key_event =
      Object::cast_to<godot::InputEventKey>(events[0]);
  if (key_event.is_null()) {
    return "UNBOUND";
  }

  // Convert keycode to its physical character representation
  // keycode gives us the key number (81=Q, 87=W, etc)
  uint32_t keycode = key_event->get_keycode();

  // Map keycodes to character names
  switch (keycode) {
    case 81:
      return "Q";  // KEY_Q
    case 87:
      return "W";  // KEY_W
    case 69:
      return "E";  // KEY_E
    case 82:
      return "R";  // KEY_R
    case 68:
      return "D";  // KEY_D
    case 70:
      return "F";  // KEY_F
    default:
      return String::num(keycode);  // Return keycode if not in standard mapping
  }
}

void InputManager::_update_hover_glow() {
  if (controlled_unit == nullptr || camera == nullptr) {
    return;
  }

  // Raycast to see what's under the cursor
  Vector3 mouse_pos;
  godot::Object* hovered_object = nullptr;
  if (!_try_raycast(mouse_pos, hovered_object)) {
    // Nothing hovered - remove glow if a unit was previously hovered
    if (hovered_unit != nullptr) {
      _remove_outline_glow(hovered_unit);
      hovered_unit = nullptr;
    }
    return;
  }

  // Try to cast the hovered object to a Unit
  auto hovered_candidate = Object::cast_to<Unit>(hovered_object);

  // Check if this is a valid targetable unit
  bool is_valid_target = false;
  if (hovered_candidate != nullptr && hovered_candidate != controlled_unit) {
    // Don't glow if it's the controlled unit or the same faction
    if (hovered_candidate->get_faction_id() !=
        controlled_unit->get_faction_id()) {
      is_valid_target = true;
    }
  }

  // Update glow based on whether we're hovering a valid target
  if (is_valid_target && hovered_candidate != hovered_unit) {
    // New valid target - apply glow
    if (hovered_unit != nullptr) {
      _remove_outline_glow(hovered_unit);
    }
    hovered_unit = hovered_candidate;
    _apply_outline_glow(hovered_unit);
  } else if (!is_valid_target && hovered_unit != nullptr) {
    // Was hovering a valid target, but now hovering something else
    _remove_outline_glow(hovered_unit);
    hovered_unit = nullptr;
  }
}

void InputManager::_apply_outline_glow(Unit* unit) {
  if (unit == nullptr) {
    return;
  }

  // Remove any existing glow overlay
  if (glow_overlay != nullptr) {
    glow_overlay->queue_free();
    glow_overlay = nullptr;
  }

  // Create a glow overlay by using the unit's mesh with a glowing material
  // Find the main mesh instance of the unit
  MeshInstance3D* main_mesh = nullptr;
  for (int i = 0; i < unit->get_child_count(); ++i) {
    godot::Node* child = unit->get_child(i);
    auto mesh_instance = Object::cast_to<MeshInstance3D>(child);
    if (mesh_instance != nullptr) {
      main_mesh = mesh_instance;
      break;  // Use the first mesh instance found
    }
  }

  if (main_mesh == nullptr) {
    return;  // No mesh to glow
  }

  // Create a new MeshInstance3D as the glow overlay
  glow_overlay = memnew(MeshInstance3D);
  unit->add_child(glow_overlay);

  // Copy the mesh from the main mesh instance
  Ref<godot::Mesh> mesh = main_mesh->get_mesh();
  if (mesh.is_valid()) {
    glow_overlay->set_mesh(mesh);
  }

  // Create a glowing material
  Ref<StandardMaterial3D> glow_material = memnew(StandardMaterial3D);
  glow_material->set_shading_mode(godot::BaseMaterial3D::SHADING_MODE_UNSHADED);
  glow_material->set_albedo(glow_color);
  glow_material->set_emission(glow_color);
  glow_material->set_emission_energy_multiplier(0.5f);
  glow_material->set_transparency(godot::BaseMaterial3D::TRANSPARENCY_ALPHA);

  // Apply the glow material to all surfaces
  glow_overlay->set_surface_override_material(0, glow_material);

  // Copy the transform of the main mesh
  glow_overlay->set_transform(main_mesh->get_transform());
}

void InputManager::_remove_outline_glow(Unit* unit) {
  if (unit == nullptr) {
    return;
  }

  // Remove the glow overlay
  if (glow_overlay != nullptr) {
    glow_overlay->queue_free();
    glow_overlay = nullptr;
  }
}

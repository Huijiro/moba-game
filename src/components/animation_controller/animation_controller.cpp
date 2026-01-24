/// @file animation_controller.cpp
/// Inspector-configurable animation controller implementation

#include "animation_controller.hpp"

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "animation_binding.hpp"
#include "unit.hpp"

using godot::AnimationPlayer;
using godot::Callable;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Node;
using godot::Object;
using godot::PROPERTY_HINT_ARRAY_TYPE;
using godot::PropertyInfo;
using godot::String;
using godot::StringName;
using godot::TypedArray;
using godot::UtilityFunctions;
using godot::Variant;

AnimationController::AnimationController() = default;

AnimationController::~AnimationController() = default;

void AnimationController::_bind_methods() {
  // Animation list property (inspector-configurable)
  ClassDB::bind_method(D_METHOD("set_animations", "bindings"),
                       &AnimationController::set_animations);
  ClassDB::bind_method(D_METHOD("get_animations"),
                       &AnimationController::get_animations);

  PropertyInfo animations_prop(Variant::ARRAY, "animations",
                               PROPERTY_HINT_ARRAY_TYPE, "AnimationBinding");
  ClassDB::add_property("AnimationController", animations_prop,
                        "set_animations", "get_animations");

  // Playback methods
  ClassDB::bind_method(D_METHOD("play_animation", "name", "speed"),
                       &AnimationController::play_animation, DEFVAL(0.0f));
  ClassDB::bind_method(D_METHOD("play_animation_index", "index", "speed"),
                       &AnimationController::play_animation_index,
                       DEFVAL(0.0f));
}

void AnimationController::_ready() {
  // Skip in editor
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Find AnimationPlayer in Unit's scene tree
  if (get_unit()) {
    animation_player = find_animation_player(get_unit());
  }

  if (!animation_player) {
    UtilityFunctions::push_error(
        "[AnimationController] No AnimationPlayer found in Unit: " +
        String(get_unit() ? get_unit()->get_name() : "null"));
    return;
  }

  // Connect all configured signals
  connect_animation_signals();
}

AnimationPlayer* AnimationController::find_animation_player(Node* node) {
  if (!node) {
    return nullptr;
  }

  // Check if current node is AnimationPlayer
  if (auto* ap = Object::cast_to<AnimationPlayer>(node)) {
    return ap;
  }

  // Recursively search children
  for (int i = 0; i < node->get_child_count(); i++) {
    if (auto* result = find_animation_player(node->get_child(i))) {
      return result;
    }
  }

  return nullptr;
}

void AnimationController::set_animations(
    const TypedArray<AnimationBinding>& bindings) {
  animation_bindings = bindings;
}

TypedArray<AnimationBinding> AnimationController::get_animations() const {
  return animation_bindings;
}

void AnimationController::connect_animation_signals() {
  if (!get_unit()) {
    return;
  }

  for (int i = 0; i < static_cast<int>(animation_bindings.size()); i++) {
    // Get binding from TypedArray (returns Variant)
    Variant binding_variant = animation_bindings[i];
    AnimationBinding* binding =
        Object::cast_to<AnimationBinding>(binding_variant);

    if (!binding) {
      continue;
    }

    // Skip if no signal is configured
    StringName trigger_signal = binding->get_trigger_signal();
    if (trigger_signal.is_empty()) {
      continue;
    }

    // Find signal emitter in Unit's children
    Object* emitter = find_signal_emitter(trigger_signal);

    if (!emitter) {
      UtilityFunctions::push_warning(
          "[AnimationController] Could not find signal emitter for: " +
          String(trigger_signal));
      continue;
    }

    // Connect signal to play_animation with binding index
    // We use index-based binding to support speed multiplier
    Callable callback =
        Callable(this, "play_animation_index").bind(Variant(int32_t(i)));
    emitter->connect(trigger_signal, callback);

    UtilityFunctions::print("[AnimationController] Connected " +
                            String(trigger_signal) + " → " +
                            String(binding->get_animation_name()));
  }
}

Object* AnimationController::find_signal_emitter(
    const StringName& signal_path) {
  if (signal_path.is_empty()) {
    return nullptr;
  }

  String path_str(signal_path);

  // Parse format: "ComponentName:signal_name" or "../ComponentName:signal_name"
  int colon_pos = path_str.rfind(":");
  if (colon_pos == -1) {
    return nullptr;
  }

  String node_path = path_str.substr(0, colon_pos);

  // Try to get node from path
  Node* unit = get_unit();
  if (!unit) {
    return nullptr;
  }

  Node* emitter = unit->get_node_or_null(node_path);
  return emitter;
}

void AnimationController::play_animation(const StringName& name, float speed) {
  if (!animation_player) {
    return;
  }

  // Use speed=0 as indicator to use default; otherwise use provided speed
  float final_speed = (speed > 0.0f) ? speed : 1.0f;

  UtilityFunctions::print("[Animation] Playing: " + String(name) + " @ " +
                          String::num(final_speed) + "x");

  animation_player->play(name);
  animation_player->set_speed_scale(final_speed);
}

void AnimationController::play_animation_index(int32_t index, float speed) {
  if (index < 0 || index >= static_cast<int32_t>(animation_bindings.size())) {
    UtilityFunctions::push_warning(
        "[AnimationController] Invalid animation index: " + String::num(index));
    return;
  }

  Variant binding_variant = animation_bindings[index];
  AnimationBinding* binding =
      Object::cast_to<AnimationBinding>(binding_variant);

  if (!binding) {
    return;
  }

  // Use binding speed if no override provided
  float final_speed = (speed > 0.0f) ? speed : binding->get_playback_speed();

  play_animation(binding->get_animation_name(), final_speed);
}

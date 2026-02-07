#include "vfx_node.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::String;
using godot::UtilityFunctions;
using godot::Variant;

VFXNode::VFXNode() {
  set_process(true);
}

VFXNode::~VFXNode() = default;

void VFXNode::_bind_methods() {
  ClassDB::bind_method(D_METHOD("play", "params"), &VFXNode::play,
                       DEFVAL(Dictionary()));
  ClassDB::bind_method(D_METHOD("stop"), &VFXNode::stop);
  ClassDB::bind_method(D_METHOD("is_playing"), &VFXNode::is_playing);

  ClassDB::bind_method(D_METHOD("set_duration", "seconds"),
                       &VFXNode::set_duration);
  ClassDB::bind_method(D_METHOD("get_duration"), &VFXNode::get_duration);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration",
               "get_duration");

  // Internal callbacks
  ClassDB::bind_method(D_METHOD("_on_finished"), &VFXNode::_on_finished);
  // Note: _process is a virtual method, don't bind it as a regular method
}

void VFXNode::_process(double delta) {
  if (!is_playing_internal) {
    return;
  }

  elapsed_time += static_cast<float>(delta);

  // Check if duration has elapsed
  if (elapsed_time >= duration) {
    _on_finished();
  }
}

void VFXNode::play(const Dictionary& params) {
  DBG_INFO("VFXNode", "Playing VFX: " + get_name());
  is_playing_internal = true;
  elapsed_time = 0.0f;

  // Default implementation - just sits for duration
  // Subclasses should override this method to implement actual animations
  // They can manipulate properties directly using tweens created via get_tree()
}

void VFXNode::stop() {
  // Mark as not playing - _process won't call _on_finished
  is_playing_internal = false;
  DBG_INFO("VFXNode", "Stopped VFX: " + get_name());
}

void VFXNode::_on_finished() {
  is_playing_internal = false;
  DBG_INFO("VFXNode", "VFX finished: " + get_name());

  // Clear callbacks before cleanup
  clear_callbacks();

  // Auto-cleanup: queue this node for deletion
  queue_free();
}

bool VFXNode::is_playing() const {
  return is_playing_internal;
}

void VFXNode::set_duration(float seconds) {
  duration = seconds >= 0.0f ? seconds : 1.0f;
}

float VFXNode::get_duration() const {
  return duration;
}

void VFXNode::register_callback(String signal_name,
                                std::function<void()> callback) {
  DBG_INFO("VFXNode", "Registering callback for signal: " + signal_name +
                          " on " + get_name());
  callbacks[signal_name] = callback;
}

void VFXNode::_on_animation_signal(String signal_name) {
  DBG_INFO("VFXNode", "Animation signal received: " + signal_name);

  // Check if we have a callback for this signal
  auto it = callbacks.find(signal_name);
  if (it == callbacks.end()) {
    DBG_WARN("VFXNode", "Unexpected animation signal: " + signal_name + " on " +
                            get_name());
    return;
  }

  // Execute the callback
  if (it->second) {
    it->second();
    DBG_INFO("VFXNode", "Callback executed for signal: " + signal_name);
  }
}

AnimationPlayer* VFXNode::get_animation_player() {
  // Return cached reference if available
  if (animation_player_cache != nullptr) {
    return animation_player_cache;
  }

  // Find AnimationPlayer child node
  for (int i = 0; i < get_child_count(); i++) {
    AnimationPlayer* ap = godot::Object::cast_to<AnimationPlayer>(get_child(i));
    if (ap != nullptr) {
      animation_player_cache = ap;
      return ap;
    }
  }

  // AnimationPlayer is optional - return nullptr silently if not found
  return nullptr;
}

void VFXNode::validate_animation_signals() {
  AnimationPlayer* ap = get_animation_player();
  if (ap == nullptr) {
    DBG_WARN("VFXNode", "Cannot validate signals - no AnimationPlayer found");
    return;
  }

  // Get current animation
  godot::String current_anim = ap->get_assigned_animation();
  if (current_anim.is_empty()) {
    DBG_WARN("VFXNode", "No animation assigned for validation");
    return;
  }

  // For now, just log what signals we expect
  // Full validation would require inspecting animation tracks
  DBG_INFO("VFXNode", "Validating " + String::num(expected_signals.size()) +
                          " expected signals for animation: " + current_anim);
}

void VFXNode::clear_callbacks() {
  DBG_INFO("VFXNode", "Clearing callbacks for " + get_name());
  callbacks.clear();
}

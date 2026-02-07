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
  ClassDB::bind_method(D_METHOD("_on_animation_signal", "signal_name"),
                       &VFXNode::_on_animation_signal);

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
  is_playing_internal = true;
  elapsed_time = 0.0f;

  // Default implementation - just sits for duration
  // Subclasses should override this method to implement actual animations
  // They can manipulate properties directly using tweens created via get_tree()
}

void VFXNode::stop() {
  // Mark as not playing - _process won't call _on_finished
  is_playing_internal = false;
}

void VFXNode::_on_finished() {
  is_playing_internal = false;

  // Clear callbacks before cleanup
  clear_callbacks();

  // Explicitly cleanup children to ensure rendering resources are freed
  // This prevents material/shader/texture RID leaks
  for (int i = get_child_count() - 1; i >= 0; i--) {
    Node* child = get_child(i);
    if (child != nullptr) {
      remove_child(child);
      child->queue_free();
    }
  }

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
  callbacks[signal_name] = callback;

  // Dynamically add the signal so animation method tracks can emit it
  if (!has_signal(signal_name)) {
    add_user_signal(signal_name);
  }
}

void VFXNode::_on_animation_signal(String signal_name) {
  // Check if we have a callback for this signal
  auto it = callbacks.find(signal_name);
  if (it == callbacks.end()) {
    return;
  }

  // Execute the callback
  if (it->second) {
    it->second();
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
    return;
  }

  // Get current animation
  godot::String current_anim = ap->get_assigned_animation();
  if (current_anim.is_empty()) {
    return;
  }

  // For now, just validate that signals are defined
  // Full validation would require inspecting animation tracks
}

void VFXNode::clear_callbacks() {
  callbacks.clear();
}

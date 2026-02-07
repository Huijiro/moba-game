#include "vfx_node.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
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
  ClassDB::bind_method(D_METHOD("_process", "delta"), &VFXNode::_process);
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

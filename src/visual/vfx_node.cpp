#include "vfx_node.hpp"

#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../debug/debug_macros.hpp"

using godot::Callable;
using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

VFXNode::VFXNode() = default;

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

  ClassDB::bind_method(D_METHOD("create_parameterized_tween"),
                       &VFXNode::create_parameterized_tween);

  // Internal callback
  ClassDB::bind_method(D_METHOD("_on_finished"), &VFXNode::_on_finished);
}

Ref<Tween> VFXNode::_create_tween() {
  // Create a new tween bound to this node
  // When node is freed, tween automatically stops
  auto* scene_tree = get_tree();
  if (scene_tree == nullptr) {
    DBG_ERROR("VFXNode", "Cannot create tween - node not in tree");
    return nullptr;
  }

  Ref<Tween> tween = scene_tree->create_tween();
  if (tween.is_valid()) {
    tween->bind_node(this);
  }
  return tween;
}

void VFXNode::play(const Dictionary& params) {
  DBG_INFO("VFXNode", "Playing VFX: " + get_name());
  is_playing_internal = true;

  // Default implementation - just sits for duration
  // Subclasses should override this method to implement actual animations
  // They can use create_parameterized_tween() to build tween chains
  // and call _on_finished() when done
}

void VFXNode::stop() {
  if (active_tween.is_valid()) {
    active_tween->kill();
  }
  active_tween.unref();
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

Ref<Tween> VFXNode::create_parameterized_tween() {
  return _create_tween();
}

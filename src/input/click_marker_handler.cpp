#include "click_marker_handler.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/color.hpp>

#include "../common/unit_signals.hpp"
#include "../core/unit.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Node3D;
using godot::Object;
using godot::PropertyInfo;
using godot::Ref;
using godot::Variant;
using godot::Vector3;

ClickMarkerHandler::ClickMarkerHandler() = default;
ClickMarkerHandler::~ClickMarkerHandler() {
  if (marker != nullptr) {
    marker->queue_free();
    marker = nullptr;
  }
}

void ClickMarkerHandler::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_click_indicator_scene", "scene"),
                       &ClickMarkerHandler::set_click_indicator_scene);
  ClassDB::bind_method(D_METHOD("get_click_indicator_scene"),
                       &ClickMarkerHandler::get_click_indicator_scene);
  ADD_PROPERTY(
      PropertyInfo(Variant::OBJECT, "click_indicator_scene",
                   godot::PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"),
      "set_click_indicator_scene", "get_click_indicator_scene");

  ClassDB::bind_method(D_METHOD("set_fade_duration", "duration"),
                       &ClickMarkerHandler::set_fade_duration);
  ClassDB::bind_method(D_METHOD("get_fade_duration"),
                       &ClickMarkerHandler::get_fade_duration);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fade_duration"),
               "set_fade_duration", "get_fade_duration");

  ClassDB::bind_method(D_METHOD("show_at", "position"),
                       &ClickMarkerHandler::show_at);

  ClassDB::bind_method(D_METHOD("_on_move_requested", "position"),
                       &ClickMarkerHandler::_on_move_requested);
}

void ClickMarkerHandler::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) return;

  // Find the controlled unit from sibling PlayerController or parent
  // and connect to move_requested. For now, PlayerController calls show_at()
  // directly.
}

void ClickMarkerHandler::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) return;
  if (!active || marker == nullptr) return;

  fade_timer += delta;

  if (fade_timer < fade_duration) {
    float progress = fade_timer / fade_duration;
    float alpha = 1.0f - progress;
    if (marker_material.is_valid()) {
      godot::Color color = marker_material->get_albedo();
      color.a = alpha;
      marker_material->set_albedo(color);
    }
  } else {
    marker->queue_free();
    marker = nullptr;
    marker_material.unref();
    active = false;
    fade_timer = 0.0f;
  }
}

void ClickMarkerHandler::show_at(const Vector3& position) {
  // Clean up old marker
  if (marker != nullptr) {
    marker->queue_free();
    marker = nullptr;
  }

  if (click_indicator_scene.is_null()) return;

  godot::Node* instance = click_indicator_scene->instantiate();
  auto* node3d = Object::cast_to<Node3D>(instance);
  if (node3d == nullptr) {
    instance->queue_free();
    return;
  }

  godot::Node* parent = get_parent();
  if (parent != nullptr) {
    parent->add_child(node3d);
  }

  Vector3 marker_pos = position;
  marker_pos.y += 0.5f;
  node3d->set_global_position(marker_pos);

  marker = node3d;
  active = true;
  fade_timer = 0.0f;
}

void ClickMarkerHandler::_on_move_requested(const Vector3& position) {
  show_at(position);
}

void ClickMarkerHandler::set_click_indicator_scene(
    const Ref<godot::PackedScene>& scene) {
  click_indicator_scene = scene;
}
Ref<godot::PackedScene> ClickMarkerHandler::get_click_indicator_scene() const {
  return click_indicator_scene;
}

void ClickMarkerHandler::set_fade_duration(float duration) {
  fade_duration = duration;
}
float ClickMarkerHandler::get_fade_duration() const { return fade_duration; }

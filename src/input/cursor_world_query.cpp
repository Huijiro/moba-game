#include "cursor_world_query.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include "../common/collision_layers.hpp"
#include "../core/unit.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Dictionary;
using godot::Engine;
using godot::Node;
using godot::Node3D;
using godot::Object;
using godot::PhysicsRayQueryParameters3D;
using godot::PropertyInfo;
using godot::Ref;
using godot::Variant;
using godot::Vector2;
using godot::Vector3;

CursorWorldQuery::CursorWorldQuery() = default;
CursorWorldQuery::~CursorWorldQuery() = default;

void CursorWorldQuery::_bind_methods() {
  ClassDB::bind_method(D_METHOD("has_hit"), &CursorWorldQuery::has_hit);
  ClassDB::bind_method(D_METHOD("get_hit_position"),
                       &CursorWorldQuery::get_hit_position);
  ClassDB::bind_method(D_METHOD("get_hit_object"),
                       &CursorWorldQuery::get_hit_object);
  ClassDB::bind_method(D_METHOD("get_hit_unit"),
                       &CursorWorldQuery::get_hit_unit);

  ClassDB::bind_method(D_METHOD("set_camera", "camera"),
                       &CursorWorldQuery::set_camera);
  ClassDB::bind_method(D_METHOD("get_camera"),
                       &CursorWorldQuery::get_camera);

  ClassDB::bind_method(D_METHOD("set_controlled_unit", "unit"),
                       &CursorWorldQuery::set_controlled_unit);
  ClassDB::bind_method(D_METHOD("get_controlled_unit"),
                       &CursorWorldQuery::get_controlled_unit);

  ClassDB::bind_method(D_METHOD("set_raycast_distance", "distance"),
                       &CursorWorldQuery::set_raycast_distance);
  ClassDB::bind_method(D_METHOD("get_raycast_distance"),
                       &CursorWorldQuery::get_raycast_distance);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "camera",
                            godot::PROPERTY_HINT_NODE_TYPE, "Camera3D"),
               "set_camera", "get_camera");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "raycast_distance"),
               "set_raycast_distance", "get_raycast_distance");
}

void CursorWorldQuery::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) return;

  // Auto-find camera
  if (camera == nullptr) {
    camera = Object::cast_to<godot::Camera3D>(get_viewport()->get_camera_3d());
  }
}

void CursorWorldQuery::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) return;
  _update_query();
}

void CursorWorldQuery::_update_query() {
  hit = false;
  hit_position = Vector3();
  hit_object = nullptr;
  hit_unit = nullptr;

  if (camera == nullptr) return;

  Node3D* camera_node = Object::cast_to<Node3D>(camera);
  if (camera_node == nullptr) return;

  auto world = camera_node->get_world_3d();
  if (world.is_null()) return;

  auto* space = world->get_direct_space_state();
  if (space == nullptr) return;

  auto* viewport = get_viewport();
  if (viewport == nullptr) return;

  Vector2 mouse_pos = viewport->get_mouse_position();
  Vector3 ray_from = camera->project_ray_origin(mouse_pos);
  Vector3 ray_normal = camera->project_ray_normal(mouse_pos);
  Vector3 ray_to = ray_from + (ray_normal * raycast_distance);

  Ref<PhysicsRayQueryParameters3D> query =
      PhysicsRayQueryParameters3D::create(ray_from, ray_to);
  query->set_collide_with_bodies(true);
  query->set_collide_with_areas(false);
  query->set_collision_mask(CollisionLayer::WORLD | CollisionLayer::UNITS);

  if (controlled_unit != nullptr && controlled_unit->is_inside_tree()) {
    godot::Array exclude;
    exclude.push_back(controlled_unit->get_rid());
    query->set_exclude(exclude);
  }

  Dictionary result = space->intersect_ray(query);
  if (result.is_empty()) return;

  hit = true;
  hit_position = result["position"];
  Object* collider = result["collider"];

  // Walk up parents to find a Unit
  Node* node = Object::cast_to<Node>(collider);
  while (node != nullptr) {
    Unit* unit = Object::cast_to<Unit>(node);
    if (unit != nullptr) {
      hit_unit = unit;
      hit_object = unit;
      return;
    }
    node = node->get_parent();
  }

  hit_object = collider;
}

bool CursorWorldQuery::has_hit() const { return hit; }
Vector3 CursorWorldQuery::get_hit_position() const { return hit_position; }
Object* CursorWorldQuery::get_hit_object() const { return hit_object; }
Unit* CursorWorldQuery::get_hit_unit() const { return hit_unit; }

void CursorWorldQuery::set_camera(godot::Camera3D* cam) { camera = cam; }
godot::Camera3D* CursorWorldQuery::get_camera() const { return camera; }

void CursorWorldQuery::set_controlled_unit(Unit* unit) {
  controlled_unit = unit;
}
Unit* CursorWorldQuery::get_controlled_unit() const {
  return controlled_unit;
}

void CursorWorldQuery::set_raycast_distance(float distance) {
  raycast_distance = distance;
}
float CursorWorldQuery::get_raycast_distance() const {
  return raycast_distance;
}

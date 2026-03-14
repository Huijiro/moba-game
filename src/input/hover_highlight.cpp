#include "hover_highlight.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

#include "../core/unit.hpp"
#include "cursor_world_query.hpp"

using godot::ClassDB;
using godot::Color;
using godot::D_METHOD;
using godot::Engine;
using godot::MeshInstance3D;
using godot::Object;
using godot::PropertyInfo;
using godot::Ref;
using godot::StandardMaterial3D;
using godot::Variant;

HoverHighlight::HoverHighlight() = default;
HoverHighlight::~HoverHighlight() = default;

void HoverHighlight::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_glow_color", "color"),
                       &HoverHighlight::set_glow_color);
  ClassDB::bind_method(D_METHOD("get_glow_color"),
                       &HoverHighlight::get_glow_color);
  ADD_PROPERTY(PropertyInfo(Variant::COLOR, "glow_color"), "set_glow_color",
               "get_glow_color");

  ClassDB::bind_method(D_METHOD("set_controlled_unit", "unit"),
                       &HoverHighlight::set_controlled_unit);
  ClassDB::bind_method(D_METHOD("get_controlled_unit"),
                       &HoverHighlight::get_controlled_unit);
}

void HoverHighlight::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) return;

  // Find sibling CursorWorldQuery
  godot::Node* parent = get_parent();
  if (parent != nullptr) {
    for (int i = 0; i < parent->get_child_count(); i++) {
      cursor_query =
          Object::cast_to<CursorWorldQuery>(parent->get_child(i));
      if (cursor_query != nullptr) break;
    }
  }
}

void HoverHighlight::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) return;
  if (cursor_query == nullptr || controlled_unit == nullptr) return;

  Unit* candidate = cursor_query->get_hit_unit();

  // Only highlight enemy units
  bool is_valid = false;
  if (candidate != nullptr && candidate != controlled_unit) {
    if (candidate->get_faction_id() != controlled_unit->get_faction_id()) {
      is_valid = true;
    }
  }

  if (is_valid && candidate != hovered_unit) {
    _remove_glow();
    hovered_unit = candidate;
    _apply_glow(hovered_unit);
  } else if (!is_valid && hovered_unit != nullptr) {
    _remove_glow();
    hovered_unit = nullptr;
  }
}

void HoverHighlight::_apply_glow(Unit* unit) {
  if (unit == nullptr) return;

  _remove_glow();

  // Find the unit's mesh
  MeshInstance3D* main_mesh = nullptr;
  for (int i = 0; i < unit->get_child_count(); i++) {
    auto* mesh = Object::cast_to<MeshInstance3D>(unit->get_child(i));
    if (mesh != nullptr) {
      main_mesh = mesh;
      break;
    }
  }
  if (main_mesh == nullptr) return;

  // Create glow overlay
  glow_overlay = memnew(MeshInstance3D);
  unit->add_child(glow_overlay);

  Ref<godot::Mesh> mesh = main_mesh->get_mesh();
  if (mesh.is_valid()) {
    glow_overlay->set_mesh(mesh);
  }

  Ref<StandardMaterial3D> mat = memnew(StandardMaterial3D);
  mat->set_shading_mode(godot::BaseMaterial3D::SHADING_MODE_UNSHADED);
  mat->set_albedo(glow_color);
  mat->set_emission(glow_color);
  mat->set_emission_energy_multiplier(0.5f);
  mat->set_transparency(godot::BaseMaterial3D::TRANSPARENCY_ALPHA);
  glow_overlay->set_surface_override_material(0, mat);

  glow_overlay->set_transform(main_mesh->get_transform());
}

void HoverHighlight::_remove_glow() {
  if (glow_overlay != nullptr) {
    glow_overlay->queue_free();
    glow_overlay = nullptr;
  }
}

void HoverHighlight::set_glow_color(const Color& color) {
  glow_color = color;
}
Color HoverHighlight::get_glow_color() const { return glow_color; }

void HoverHighlight::set_controlled_unit(Unit* unit) {
  controlled_unit = unit;
}
Unit* HoverHighlight::get_controlled_unit() const {
  return controlled_unit;
}

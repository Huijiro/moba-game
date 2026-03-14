#include "cooldown_icon.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../common/unit_signals.hpp"
#include "../../core/match_manager.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../abilities/ability_component.hpp"
#include "../abilities/ability_node.hpp"

using godot::ClassDB;
using godot::Color;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::String;
using godot::Variant;

CooldownIcon::CooldownIcon() = default;

CooldownIcon::~CooldownIcon() = default;

void CooldownIcon::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_ability_slot", "slot"),
                       &CooldownIcon::set_ability_slot);
  ClassDB::bind_method(D_METHOD("get_ability_slot"),
                       &CooldownIcon::get_ability_slot);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "ability_slot"), "set_ability_slot",
               "get_ability_slot");

  ClassDB::bind_method(D_METHOD("set_icon_size", "size"),
                       &CooldownIcon::set_icon_size);
  ClassDB::bind_method(D_METHOD("get_icon_size"), &CooldownIcon::get_icon_size);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "icon_size"), "set_icon_size",
               "get_icon_size");

  ClassDB::bind_method(D_METHOD("_on_cooldown_started", "slot", "duration"),
                       &CooldownIcon::_on_cooldown_started);
}

void CooldownIcon::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  set_custom_minimum_size(icon_size);
  set_expand_mode(godot::TextureRect::EXPAND_IGNORE_SIZE);
  set_stretch_mode(godot::TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
  show();

  // Find MatchManager in scene root
  Node* current = this;
  Node* scene_root = nullptr;
  while (current && current->get_parent()) {
    scene_root = current;
    current = current->get_parent();
  }

  MatchManager* match_manager = nullptr;
  if (scene_root) {
    for (int i = 0; i < scene_root->get_child_count(); i++) {
      match_manager = Object::cast_to<MatchManager>(scene_root->get_child(i));
      if (match_manager) break;
    }
  }

  if (!match_manager) {
    DBG_WARN("CooldownIcon", "MatchManager not found");
    return;
  }

  Unit* main_unit = match_manager->get_main_unit();
  if (!main_unit) {
    DBG_WARN("CooldownIcon", "No main_unit in MatchManager");
    return;
  }

  // Connect to cooldown signal
  main_unit->connect(
      ability_cooldown_started,
      godot::Callable(this, godot::StringName("_on_cooldown_started")));

  // Load ability icon
  AbilityComponent* ability_comp = nullptr;
  for (int i = 0; i < main_unit->get_child_count(); i++) {
    ability_comp = Object::cast_to<AbilityComponent>(main_unit->get_child(i));
    if (ability_comp != nullptr) break;
  }

  if (ability_comp != nullptr) {
    AbilityNode* ability = ability_comp->get_ability(ability_slot);
    if (ability != nullptr) {
      godot::Ref<godot::Texture2D> icon = ability->get_icon();
      if (icon.is_valid()) {
        set_texture(icon);
      }
    }
  }
}

void CooldownIcon::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (on_cooldown) {
    cooldown_remaining -= delta;
    if (cooldown_remaining <= 0.0f) {
      cooldown_remaining = 0.0f;
      on_cooldown = false;
      cooldown_duration = 0.0f;
    }
    queue_redraw();
  }
}

void CooldownIcon::_draw() {
  if (!get_texture().is_valid()) {
    return;
  }

  TextureRect::_draw();

  if (on_cooldown && cooldown_duration > 0.0f) {
    float progress =
        (cooldown_duration - cooldown_remaining) / cooldown_duration;
    progress = godot::Math::clamp(progress, 0.0f, 1.0f);

    draw_set_transform(Vector2(0, 0), 0, Vector2(1, 1));

    Vector2 center = icon_size / 2.0f;
    float half_x = icon_size.x / 2.0f;
    float half_y = icon_size.y / 2.0f;
    float radius = godot::Math::sqrt(half_x * half_x + half_y * half_y);

    int segments = 64;
    float start_angle = -(float)Math_PI / 2.0f;
    float remaining = 1.0f - progress;
    float sweep_angle = 2.0f * (float)Math_PI * remaining;

    Color dark_overlay = Color(0, 0, 0, 0.7f);

    for (int i = 0; i < segments; i++) {
      float ratio1 = (float)i / segments;
      float ratio2 = (float)(i + 1) / segments;

      if (ratio1 >= remaining) break;

      float angle1 = start_angle + sweep_angle * ratio1;
      float angle2 = start_angle + sweep_angle * ratio2;

      Vector2 point1 =
          center +
          Vector2(godot::Math::cos(angle1), godot::Math::sin(angle1)) * radius;
      Vector2 point2 =
          center +
          Vector2(godot::Math::cos(angle2), godot::Math::sin(angle2)) * radius;

      point1.x = godot::Math::clamp(point1.x, 0.0f, icon_size.x);
      point1.y = godot::Math::clamp(point1.y, 0.0f, icon_size.y);
      point2.x = godot::Math::clamp(point2.x, 0.0f, icon_size.x);
      point2.y = godot::Math::clamp(point2.y, 0.0f, icon_size.y);

      godot::PackedVector2Array points;
      points.push_back(center);
      points.push_back(point1);
      points.push_back(point2);

      draw_colored_polygon(points, dark_overlay);
    }
  }
}

void CooldownIcon::_on_cooldown_started(int slot, float duration) {
  if (slot != ability_slot) return;

  cooldown_duration = duration;
  cooldown_remaining = duration;
  on_cooldown = true;
  queue_redraw();
}

void CooldownIcon::set_ability_slot(int slot) {
  ability_slot = godot::Math::clamp(slot, 0, 3);
}

int CooldownIcon::get_ability_slot() const {
  return ability_slot;
}

void CooldownIcon::set_icon_size(Vector2 size) {
  icon_size = size;
  set_custom_minimum_size(icon_size);
}

Vector2 CooldownIcon::get_icon_size() const {
  return icon_size;
}

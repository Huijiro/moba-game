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

  // Signal handlers
  ClassDB::bind_method(D_METHOD("_on_cooldown_started", "slot", "duration"),
                       &CooldownIcon::_on_cooldown_started);
  ClassDB::bind_method(D_METHOD("_on_cooldown_changed", "slot"),
                       &CooldownIcon::_on_cooldown_changed);
  ClassDB::bind_method(D_METHOD("_log_debug_position"),
                       &CooldownIcon::_log_debug_position);
}

void CooldownIcon::_ready() {
  DBG_DEBUG("CooldownIcon",
            "CooldownIcon _ready called for slot " + String::num(ability_slot));
  DBG_DEBUG("CooldownIcon",
            "  Visible: " + String(is_visible() ? "yes" : "no") +
                ", In tree: " + String(is_inside_tree() ? "yes" : "no"));

  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Set TextureRect size and expand settings
  set_custom_minimum_size(icon_size);
  set_expand_mode(godot::TextureRect::EXPAND_IGNORE_SIZE);
  set_stretch_mode(godot::TextureRect::STRETCH_KEEP_ASPECT_CENTERED);

  // Make sure the node is visible
  show();
  DBG_DEBUG("CooldownIcon",
            "  After show - Visible: " + String(is_visible() ? "yes" : "no"));

  // Log positioning after a frame to let layout compute
  call_deferred(godot::StringName("_log_debug_position"));

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
      if (match_manager) {
        break;
      }
    }
  }

  if (!match_manager) {
    DBG_WARN("CooldownIcon", "MatchManager not found in scene");
    return;
  }

  // Get main unit
  Unit* main_unit = match_manager->get_main_unit();
  if (!main_unit) {
    DBG_WARN("CooldownIcon", "No main_unit in MatchManager");
    return;
  }

  // Get AbilityComponent
  ability_component = Object::cast_to<AbilityComponent>(
      main_unit->get_component_by_class("AbilityComponent"));
  if (!ability_component) {
    DBG_WARN("CooldownIcon", "No AbilityComponent on main_unit");
    return;
  }

  // Get ability icon for this slot
  AbilityNode* ability = ability_component->get_ability(ability_slot);
  if (ability) {
    Ref<godot::Texture2D> ability_icon = ability->get_icon();
    if (ability_icon.is_valid()) {
      set_texture(ability_icon);
      DBG_DEBUG("CooldownIcon",
                "Set icon from ability for slot " + String::num(ability_slot));
    } else {
      DBG_DEBUG("CooldownIcon",
                "Ability has no icon, keeping editor texture for slot " +
                    String::num(ability_slot));
      // Keep whatever texture was set in the editor
    }
  } else {
    DBG_WARN("CooldownIcon", "No ability at slot " + String::num(ability_slot));
  }

  // Connect to cooldown signals
  ability_component->connect(
      "ability_cooldown_started",
      godot::Callable(this, godot::StringName("_on_cooldown_started")));
  ability_component->connect(
      "cooldown_changed",
      godot::Callable(this, godot::StringName("_on_cooldown_changed")));

  DBG_INFO("CooldownIcon",
           "Initialized for ability slot " + String::num(ability_slot));
}

void CooldownIcon::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (!on_cooldown) {
    return;
  }

  // Update cooldown elapsed time
  cooldown_elapsed += delta;

  // Check if cooldown is complete
  if (cooldown_elapsed >= cooldown_duration) {
    on_cooldown = false;
    cooldown_elapsed = 0.0f;
    cooldown_duration = 0.0f;
    queue_redraw();
    return;
  }

  // Redraw to update the cooldown overlay
  queue_redraw();
}

void CooldownIcon::_draw() {
  // Don't draw anything if texture is not set
  if (!get_texture().is_valid()) {
    return;
  }

  // If on cooldown, draw a radial cooldown overlay
  if (on_cooldown && cooldown_duration > 0.0f) {
    float progress = cooldown_elapsed / cooldown_duration;
    progress = godot::Math::clamp(progress, 0.0f, 1.0f);

    // Set clip rect to constrain drawing to icon bounds
    draw_set_transform(Vector2(0, 0), 0, Vector2(1, 1));

    // Draw dark overlay for the REMAINING cooldown portion
    // Arc sweeps clockwise from top (12 o'clock), darkness decreases as
    // cooldown completes
    Vector2 center = icon_size / 2.0f;
    // Radius is distance from center to corner - perfectly fills the icon
    float half_x = icon_size.x / 2.0f;
    float half_y = icon_size.y / 2.0f;
    float radius = godot::Math::sqrt(half_x * half_x + half_y * half_y);

    int segments = 32;  // Number of segments for smooth arc
    float start_angle = -(float)Math_PI / 2.0f;  // Start at top (12 o'clock)
    float remaining = 1.0f - progress;  // Remaining cooldown as a fraction
    float sweep_angle =
        2.0f * (float)Math_PI * remaining;  // How much arc to draw

    Color dark_overlay =
        Color(0, 0, 0, 0.7f);  // Dark overlay for remaining time

    // Draw filled arc by creating triangles from center to arc
    for (int i = 0; i < segments; i++) {
      float ratio1 = (float)i / segments;
      float ratio2 = (float)(i + 1) / segments;

      // Only draw if within the remaining range
      if (ratio1 >= remaining)
        break;

      float angle1 = start_angle + sweep_angle * ratio1;
      float angle2 = start_angle + sweep_angle * ratio2;

      Vector2 point1 =
          center +
          Vector2(godot::Math::cos(angle1), godot::Math::sin(angle1)) * radius;
      Vector2 point2 =
          center +
          Vector2(godot::Math::cos(angle2), godot::Math::sin(angle2)) * radius;

      // Clamp points to icon bounds
      point1.x = godot::Math::clamp(point1.x, 0.0f, icon_size.x);
      point1.y = godot::Math::clamp(point1.y, 0.0f, icon_size.y);
      point2.x = godot::Math::clamp(point2.x, 0.0f, icon_size.x);
      point2.y = godot::Math::clamp(point2.y, 0.0f, icon_size.y);

      // Draw triangle from center to arc
      godot::PackedVector2Array points;
      points.push_back(center);
      points.push_back(point1);
      points.push_back(point2);

      draw_colored_polygon(points, dark_overlay);
    }
  }
}

void CooldownIcon::_on_cooldown_started(int slot, float duration) {
  if (slot != ability_slot) {
    return;
  }

  cooldown_duration = duration;
  cooldown_elapsed = 0.0f;
  on_cooldown = true;

  DBG_DEBUG("CooldownIcon", "Cooldown started for slot " + String::num(slot) +
                                ": " + String::num(duration, 2) + "s");
  queue_redraw();
}

void CooldownIcon::_on_cooldown_changed(int slot) {
  if (slot != ability_slot) {
    return;
  }

  on_cooldown = false;
  cooldown_elapsed = 0.0f;
  cooldown_duration = 0.0f;

  DBG_DEBUG("CooldownIcon", "Cooldown reset for slot " + String::num(slot));
  queue_redraw();
}

void CooldownIcon::_log_debug_position() {
  // Get viewport size
  godot::Viewport* viewport = get_viewport();
  Vector2 screen_size =
      viewport ? viewport->get_visible_rect().size : Vector2(0, 0);

  // Get our rect
  godot::Rect2 rect = get_rect();

  DBG_DEBUG("CooldownIcon",
            "Position Debug for slot " + String::num(ability_slot));
  DBG_DEBUG("CooldownIcon", "  Screen size: " + String::num(screen_size.x) +
                                "x" + String::num(screen_size.y));
  DBG_DEBUG("CooldownIcon", "  Node rect: pos(" + String::num(rect.position.x) +
                                ", " + String::num(rect.position.y) +
                                ") size(" + String::num(rect.size.x) + ", " +
                                String::num(rect.size.y) + ")");
  DBG_DEBUG("CooldownIcon",
            "  Global rect: pos(" + String::num(get_global_rect().position.x) +
                ", " + String::num(get_global_rect().position.y) + ") size(" +
                String::num(get_global_rect().size.x) + ", " +
                String::num(get_global_rect().size.y) + ")");
  DBG_DEBUG(
      "CooldownIcon",
      "  Texture: " + String(get_texture().is_valid() ? "valid" : "INVALID"));
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

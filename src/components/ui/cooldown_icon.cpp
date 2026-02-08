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

  // Signal handlers
  ClassDB::bind_method(D_METHOD("_on_cooldown_started", "slot", "duration"),
                       &CooldownIcon::_on_cooldown_started);
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

  // Connect to cooldown_started signal to handle sudden cooldown changes
  // (e.g., if cooldown is reset or changed by the ability)
  // We do our own delta-based tracking in _process for the countdown
  main_unit->connect(
      ability_cooldown_started,
      godot::Callable(this, godot::StringName("_on_cooldown_started")));

  // Try to load ability icon from the AbilityComponent
  // First, find the AbilityComponent on the unit
  AbilityComponent* ability_comp = nullptr;
  for (int i = 0; i < main_unit->get_child_count(); i++) {
    ability_comp = Object::cast_to<AbilityComponent>(main_unit->get_child(i));
    if (ability_comp != nullptr) {
      break;
    }
  }

  // If found, get the ability icon for this slot and set it
  if (ability_comp != nullptr) {
    AbilityNode* ability = ability_comp->get_ability(ability_slot);
    if (ability != nullptr) {
      godot::Ref<godot::Texture2D> icon = ability->get_icon();
      if (icon.is_valid()) {
        set_texture(icon);
        DBG_DEBUG("CooldownIcon",
                  "Loaded icon for ability slot " + String::num(ability_slot));
      } else {
        DBG_WARN("CooldownIcon", "Ability at slot " +
                                     String::num(ability_slot) +
                                     " has no icon texture");
      }
    } else {
      DBG_WARN("CooldownIcon",
               "Could not find ability at slot " + String::num(ability_slot));
    }
  } else {
    DBG_WARN("CooldownIcon", "Could not find AbilityComponent on main unit");
  }

  DBG_INFO("CooldownIcon",
           "Initialized for ability slot " + String::num(ability_slot));
}

void CooldownIcon::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Do our own delta-based cooldown tracking
  if (on_cooldown) {
    cooldown_remaining -= delta;
    if (cooldown_remaining <= 0.0f) {
      cooldown_remaining = 0.0f;
      on_cooldown = false;
      cooldown_duration = 0.0f;
    }
    queue_redraw();  // Redraw every frame during cooldown for smooth animation
  }
}

void CooldownIcon::_draw() {
  // Don't draw anything if texture is not set
  if (!get_texture().is_valid()) {
    return;
  }

  // First, let TextureRect draw the icon texture
  TextureRect::_draw();

  // If on cooldown, draw a radial cooldown overlay on top
  if (on_cooldown && cooldown_duration > 0.0f) {
    float progress =
        (cooldown_duration - cooldown_remaining) / cooldown_duration;
    progress = godot::Math::clamp(progress, 0.0f, 1.0f);

    // DEBUG: Log every N frames to see if this is being called
    static int frame_count = 0;
    if (frame_count % 30 == 0) {
      DBG_DEBUG("CooldownIcon",
                "Drawing cooldown for slot " + String::num(ability_slot) +
                    ", progress=" + String::num(progress, 2) +
                    ", remaining=" + String::num(cooldown_remaining, 2));
    }
    frame_count++;

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

    int segments = 64;  // Number of segments for smooth arc (increased for
                        // smoother animation)
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
  cooldown_remaining = duration;
  on_cooldown = true;

  DBG_DEBUG("CooldownIcon", "Cooldown started for slot " + String::num(slot) +
                                ": " + String::num(duration, 2) + "s");
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

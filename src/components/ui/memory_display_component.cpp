#include "memory_display_component.hpp"

#include <godot_cpp/classes/style_box_flat.hpp>
#include <godot_cpp/classes/theme.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../debug/memory_profiler.hpp"

using godot::ClassDB;
using godot::Color;
using godot::D_METHOD;
using godot::Dictionary;
using godot::Object;
using godot::PropertyInfo;
using godot::String;
using godot::StyleBoxFlat;
using godot::Theme;
using godot::UtilityFunctions;
using godot::Variant;

MemoryDisplayComponent::MemoryDisplayComponent() {
  // Nothing special in constructor
}

MemoryDisplayComponent::~MemoryDisplayComponent() = default;

void MemoryDisplayComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_show_stats", "show"),
                       &MemoryDisplayComponent::set_show_stats);
  ClassDB::bind_method(D_METHOD("is_showing_stats"),
                       &MemoryDisplayComponent::is_showing_stats);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_stats"), "set_show_stats",
               "is_showing_stats");

  ClassDB::bind_method(D_METHOD("set_update_interval", "interval"),
                       &MemoryDisplayComponent::set_update_interval);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "update_interval"),
               "set_update_interval", "set_update_interval");
}

void MemoryDisplayComponent::_ready() {
  create_ui();
  MemoryProfiler::initialize();
}

void MemoryDisplayComponent::create_ui() {
  // Create panel container for background
  panel = memnew(PanelContainer);
  panel->set_custom_minimum_size(godot::Vector2(400, 300));
  add_child(panel);

  // Create label for text
  memory_label = memnew(Label);
  memory_label->set_text("Memory Stats");
  panel->add_child(memory_label);

  // Style the panel
  auto theme = memnew(Theme);
  auto panel_style = memnew(StyleBoxFlat);
  panel_style->set_bg_color(Color(0, 0, 0, 0.7f));
  panel_style->set_border_color(Color(0.2f, 0.8f, 0.2f, 1.0f));
  panel_style->set_border_width_all(2);
  theme->set_stylebox("panel", "PanelContainer", panel_style);

  panel->set_theme(theme);
}

void MemoryDisplayComponent::_process(double delta) {
  if (!show_memory_stats) {
    return;
  }

  elapsed_time += static_cast<float>(delta);

  if (elapsed_time >= update_interval) {
    elapsed_time = 0.0f;
    update_memory_display();
  }
}

void MemoryDisplayComponent::update_memory_display() {
  if (memory_label == nullptr) {
    return;
  }

  Dictionary all_stats = MemoryProfiler::get_all_stats_dict();
  Dictionary alive_allocs = MemoryProfiler::get_alive_allocations();

  String text = "=== Memory Stats ===\n";

  // Display per-category stats
  for (int i = 0; i < all_stats.size(); i++) {
    String key = all_stats.keys()[i];
    Dictionary cat_stats = all_stats[key];

    text += "\n" + key + ":\n";
    text += "  Allocated: " + String::num_int64(cat_stats["allocated"]) + "\n";
    text += "  Freed: " + String::num_int64(cat_stats["freed"]) + "\n";
    text += "  Alive: " + String::num_int64(cat_stats["alive"]) + "\n";
    text += "  Peak: " + String::num_int64(cat_stats["peak"]) + "\n";
    text += "  Avg Lifetime: " +
            String::num(double(cat_stats["avg_lifetime_ms"]), 1) + " ms\n";

    // Warn if alive count seems high
    int alive = cat_stats["alive"];
    if (alive > 10) {
      text += "  [WARNING: High alive count!]\n";
    }
  }

  memory_label->set_text(text);
}

void MemoryDisplayComponent::set_show_stats(bool show) {
  show_memory_stats = show;
  if (panel != nullptr) {
    panel->set_visible(show);
  }
}

void MemoryDisplayComponent::set_update_interval(float interval) {
  update_interval = interval > 0.1f ? interval : 0.1f;
}

#include "data_progress_bar.hpp"

#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector2.hpp>

using godot::ClassDB;
using godot::Color;
using godot::Font;
using godot::HorizontalAlignment;
using godot::Rect2;
using godot::String;
using godot::Vector2;

DataProgressBar::DataProgressBar() {
  // Enable show_percentage so ProgressBar reserves space for text
  set_show_percentage(true);
}

DataProgressBar::~DataProgressBar() = default;

void DataProgressBar::_bind_methods() {
  // No methods to bind
}

void DataProgressBar::_notification(int p_what) {
  if (p_what == NOTIFICATION_DRAW) {
    // Let parent handle everything first, which will draw percentage
    ProgressBar::_notification(p_what);

    // Now we need to overlay our custom text
    godot::Ref<Font> font = get_theme_font("font");
    int font_size = get_theme_font_size("font_size");
    Color font_color = get_theme_color("font_color");
    int outline_size = get_theme_constant("outline_size");
    Color outline_color = get_theme_color("font_outline_color");

    if (font.is_null()) {
      return;
    }

    // Create custom text
    String txt =
        String::num((int)get_value()) + "/" + String::num((int)get_max());

    // Measure the text using the font
    Vector2 text_size = font->get_string_size(
        txt, HorizontalAlignment::HORIZONTAL_ALIGNMENT_LEFT, -1, font_size);

    // Center on the progress bar
    Vector2 bar_size = get_size();
    Vector2 text_pos = (bar_size - text_size) / 2.0;
    text_pos = text_pos.round();

    // Draw a background rect to cover the percentage text
    Color bg_color = get_theme_color("font_color");
    bg_color.a = 0.6;
    draw_rect(Rect2(text_pos - Vector2(2, 2), text_size + Vector2(4, 4)),
              bg_color * Color(0, 0, 0, 1));

    // Draw the custom text
    font->draw_string(get_canvas_item(), text_pos, txt,
                      HorizontalAlignment::HORIZONTAL_ALIGNMENT_LEFT, -1,
                      font_size, font_color);

    // Draw outline if needed
    if (outline_size > 0 && outline_color.a > 0) {
      font->draw_string_outline(get_canvas_item(), text_pos, txt,
                                HorizontalAlignment::HORIZONTAL_ALIGNMENT_LEFT,
                                -1, font_size, outline_size, outline_color);
    }

  } else {
    // For all other notifications, call parent
    ProgressBar::_notification(p_what);
  }
}

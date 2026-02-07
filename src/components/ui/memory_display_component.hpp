#ifndef GDEXTENSION_MEMORY_DISPLAY_COMPONENT_H
#define GDEXTENSION_MEMORY_DISPLAY_COMPONENT_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/core/object.hpp>

using godot::Control;
using godot::Label;
using godot::PanelContainer;

/// UI component to display memory profiling stats in-game
/// Shows VFX allocations, cleanup status, and potential leaks
class MemoryDisplayComponent : public Control {
  GDCLASS(MemoryDisplayComponent, Control)

 protected:
  static void _bind_methods();

  Label* memory_label = nullptr;
  PanelContainer* panel = nullptr;
  bool show_memory_stats = true;
  float update_interval = 0.5f;  // Update every 0.5 seconds
  float elapsed_time = 0.0f;

 public:
  MemoryDisplayComponent();
  ~MemoryDisplayComponent();

  void _ready() override;
  void _process(double delta) override;

  // Toggle display
  void set_show_stats(bool show);
  bool is_showing_stats() const { return show_memory_stats; }

  // Set/get update interval (in seconds)
  void set_update_interval(float interval);
  float get_update_interval() const { return update_interval; }

 private:
  void update_memory_display();
  void create_ui();
};

#endif  // GDEXTENSION_MEMORY_DISPLAY_COMPONENT_H

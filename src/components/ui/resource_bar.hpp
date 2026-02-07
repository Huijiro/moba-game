#ifndef GDEXTENSION_RESOURCE_BAR_H
#define GDEXTENSION_RESOURCE_BAR_H

#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/variant/string_name.hpp>

using godot::ProgressBar;
using godot::StringName;

class ResourcePoolComponent;

/// ResourceBar - displays a specific resource pool's current/max values
///
/// This component explicitly tracks which ResourcePoolComponent to display
/// via a pool_id property, eliminating fragile node name parsing.
///
/// Properties:
/// - pool_id: The StringName ID of the resource pool to display (e.g., "mana")
///
/// This component automatically:
/// 1. Finds the ResourcePoolComponent with matching pool_id in the parent Unit
/// 2. Connects to its value_changed signal
/// 3. Updates the ProgressBar value/max whenever the resource changes
class ResourceBar : public ProgressBar {
  GDCLASS(ResourceBar, ProgressBar)

 protected:
  static void _bind_methods();

  StringName pool_id = "mana";
  ResourcePoolComponent* resource_pool = nullptr;

  // Signal handler
  void _on_resource_changed(float current, float max);

 public:
  ResourceBar();
  ~ResourceBar();

  void _ready() override;

  // Properties
  void set_pool_id(StringName id);
  StringName get_pool_id() const;
};

#endif  // GDEXTENSION_RESOURCE_BAR_H

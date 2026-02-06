#ifndef GDEXTENSION_LABEL_REGISTRY_H
#define GDEXTENSION_LABEL_REGISTRY_H

#include <godot_cpp/variant/string.hpp>
#include <vector>

using godot::String;

/// Registry for collecting debug label data from components
/// Components register properties and this formats them for display
class LabelRegistry {
 private:
  struct RegistryEntry {
    String component_name;
    String key;
    String value;
  };

  std::vector<RegistryEntry> entries;

 public:
  LabelRegistry() = default;
  ~LabelRegistry() = default;

  /// Register a property to be displayed
  /// Example: register_property("Health", "current", "85")
  void register_property(const String& component_name,
                         const String& key,
                         const String& value);

  /// Clear all entries (called at start of each update frame)
  void clear();

  /// Get formatted text for display
  /// Returns multi-line string with format: [ComponentName] key1=value1,
  /// key2=value2
  String get_formatted_text() const;
};

#endif  // GDEXTENSION_LABEL_REGISTRY_H

#include "label_registry.hpp"

void LabelRegistry::register_property(const String& component_name,
                                      const String& key,
                                      const String& value) {
  entries.push_back({component_name, key, value});
}

void LabelRegistry::clear() {
  entries.clear();
}

String LabelRegistry::get_formatted_text() const {
  if (entries.empty()) {
    return String();
  }

  String result;
  String current_component;

  for (size_t i = 0; i < entries.size(); ++i) {
    const auto& entry = entries[i];

    // Start new line if component changed
    if (entry.component_name != current_component) {
      if (i > 0) {
        result += String("\n");
      }
      result += String("[") + entry.component_name + String("] ");
      current_component = entry.component_name;
    } else {
      // Add comma separator between properties of same component
      result += String(", ");
    }

    // Add key=value pair
    result += entry.key + String("=") + entry.value;
  }

  return result;
}

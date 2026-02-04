#ifndef GDEXTENSION_DEBUG_LOGGER_H
#define GDEXTENSION_DEBUG_LOGGER_H

#include <godot_cpp/classes/logger.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using godot::Logger;
using godot::Ref;
using godot::RefCounted;
using godot::String;
using godot::TypedArray;

// Log level constants (use as integers, not as enum)
namespace LogLevel {
constexpr int DEBUG = 0;
constexpr int INFO = 1;
constexpr int WARNING = 2;
constexpr int ERROR = 3;
}  // namespace LogLevel

/// Extended logging system built on Godot's Logger class
/// Provides categorized, leveled logging with Godot integration
class DebugLogger : public Logger {
  GDCLASS(DebugLogger, Logger)

 protected:
  static void _bind_methods();

  int current_log_level = ::LogLevel::DEBUG;
  bool log_to_output = true;

 public:
  // Override Godot's Logger virtual methods for custom logging
  void _log_message(const String& p_message, bool p_error) override;

  DebugLogger();
  ~DebugLogger();

  // Core logging functions with categories
  void debug(const String& category, const String& message);
  void info(const String& category, const String& message);
  void warning(const String& category, const String& message);
  void error(const String& category, const String& message);

  // Utility functions
  void set_log_level(int level);
  int get_log_level() const;

  void set_log_to_output(bool enabled);
  bool is_logging_to_output() const;

  static DebugLogger* get_singleton();
  static DebugLogger* ensure_singleton();  // Creates singleton if needed

 private:
  static DebugLogger* singleton_instance;

  // Internal logging method
  void _log(int level, const String& category, const String& message);
};

#endif  // GDEXTENSION_DEBUG_LOGGER_H

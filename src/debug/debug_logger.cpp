#include "debug_logger.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::UtilityFunctions;

DebugLogger* DebugLogger::singleton_instance = nullptr;

DebugLogger::DebugLogger() {
  singleton_instance = this;
  UtilityFunctions::print("[DebugLogger] Initialized with log level: DEBUG");
}

DebugLogger::~DebugLogger() {
  if (singleton_instance == this) {
    singleton_instance = nullptr;
  }
}

void DebugLogger::_bind_methods() {
  // Instance methods for logging
  ClassDB::bind_method(D_METHOD("debug", "category", "message"),
                       &DebugLogger::debug);
  ClassDB::bind_method(D_METHOD("info", "category", "message"),
                       &DebugLogger::info);
  ClassDB::bind_method(D_METHOD("warning", "category", "message"),
                       &DebugLogger::warning);
  ClassDB::bind_method(D_METHOD("error", "category", "message"),
                       &DebugLogger::error);

  ClassDB::bind_method(D_METHOD("set_log_level", "level"),
                       &DebugLogger::set_log_level);
  ClassDB::bind_method(D_METHOD("get_log_level"), &DebugLogger::get_log_level);

  ClassDB::bind_method(D_METHOD("set_log_to_output", "enabled"),
                       &DebugLogger::set_log_to_output);
  ClassDB::bind_method(D_METHOD("is_logging_to_output"),
                       &DebugLogger::is_logging_to_output);

  // Expose log level constants
  ClassDB::bind_integer_constant("DebugLogger", "LogLevel", "DEBUG",
                                 ::LogLevel::DEBUG);
  ClassDB::bind_integer_constant("DebugLogger", "LogLevel", "INFO",
                                 ::LogLevel::INFO);
  ClassDB::bind_integer_constant("DebugLogger", "LogLevel", "WARNING",
                                 ::LogLevel::WARNING);
  ClassDB::bind_integer_constant("DebugLogger", "LogLevel", "ERROR",
                                 ::LogLevel::ERROR);
}

String DebugLogger::_get_level_prefix(int level) const {
  switch (level) {
    case ::LogLevel::DEBUG:
      return "[DEBUG]";
    case ::LogLevel::INFO:
      return "[INFO]";
    case ::LogLevel::WARNING:
      return "[WARNING]";
    case ::LogLevel::ERROR:
      return "[ERROR]";
    default:
      return "[UNKNOWN]";
  }
}

void DebugLogger::_log(int level,
                       const String& category,
                       const String& message) {
  // Skip if below log level threshold
  if (level < current_log_level) {
    return;
  }

  String level_prefix = _get_level_prefix(level);
  String log_message =
      level_prefix + String(" [") + category + String("] ") + message;

  if (log_to_output) {
    UtilityFunctions::print(log_message);
  }

  // Also push to Godot's logging system for file output
  if (level == ::LogLevel::WARNING) {
    UtilityFunctions::push_warning(log_message);
  } else if (level == ::LogLevel::ERROR) {
    UtilityFunctions::push_error(log_message);
  }
}

void DebugLogger::debug(const String& category, const String& message) {
  _log(::LogLevel::DEBUG, category, message);
}

void DebugLogger::info(const String& category, const String& message) {
  _log(::LogLevel::INFO, category, message);
}

void DebugLogger::warning(const String& category, const String& message) {
  _log(::LogLevel::WARNING, category, message);
}

void DebugLogger::error(const String& category, const String& message) {
  _log(::LogLevel::ERROR, category, message);
}

void DebugLogger::set_log_level(int level) {
  current_log_level = level;
  String level_name;
  switch (level) {
    case ::LogLevel::DEBUG:
      level_name = "DEBUG";
      break;
    case ::LogLevel::INFO:
      level_name = "INFO";
      break;
    case ::LogLevel::WARNING:
      level_name = "WARNING";
      break;
    case ::LogLevel::ERROR:
      level_name = "ERROR";
      break;
  }
  UtilityFunctions::print("[DebugLogger] Log level set to: " + level_name);
}

int DebugLogger::get_log_level() const {
  return current_log_level;
}

void DebugLogger::set_log_to_output(bool enabled) {
  log_to_output = enabled;
}

bool DebugLogger::is_logging_to_output() const {
  return log_to_output;
}

DebugLogger* DebugLogger::get_singleton() {
  return singleton_instance;
}

void DebugLogger::_log_message(const String& p_message, bool p_error) {
  // Override Godot's Logger to route through our system
  if (p_error) {
    error("Godot", p_message);
  } else {
    info("Godot", p_message);
  }
}

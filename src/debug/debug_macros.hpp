#ifndef GDEXTENSION_DEBUG_MACROS_H
#define GDEXTENSION_DEBUG_MACROS_H

#include <godot_cpp/variant/string.hpp>
#include "debug_logger.hpp"

using godot::String;

// Simple macro for quick category + message logging
// Usage: DBG_LOG("Movement", "Unit reached destination");

#define DBG_DEBUG(category, message)                  \
  do {                                                \
    auto* _dbg = DebugLogger::ensure_singleton();     \
    if (_dbg)                                         \
      _dbg->debug(String(category), String(message)); \
  } while (0)

#define DBG_INFO(category, message)                  \
  do {                                               \
    auto* _dbg = DebugLogger::ensure_singleton();    \
    if (_dbg)                                        \
      _dbg->info(String(category), String(message)); \
  } while (0)

#define DBG_WARN(category, message)                     \
  do {                                                  \
    auto* _dbg = DebugLogger::ensure_singleton();       \
    if (_dbg)                                           \
      _dbg->warning(String(category), String(message)); \
  } while (0)

#define DBG_ERROR(category, message)                  \
  do {                                                \
    auto* _dbg = DebugLogger::ensure_singleton();     \
    if (_dbg)                                         \
      _dbg->error(String(category), String(message)); \
  } while (0)

// Convenience macros for converting values to strings
// Usage: DBG_VALUE("Category", "name", value)

#define DBG_VALUE(category, name, value)                                       \
  do {                                                                         \
    auto* _dbg = DebugLogger::ensure_singleton();                              \
    if (_dbg)                                                                  \
      _dbg->debug(String(category), String(name) + ": " + String::num(value)); \
  } while (0)

#define DBG_VECTOR3(category, name, vec)                                     \
  do {                                                                       \
    auto* _dbg = DebugLogger::ensure_singleton();                            \
    if (_dbg)                                                                \
      _dbg->debug(String(category),                                          \
                  String(name) + ": (" + String::num(vec.x) + ", " +         \
                      String::num(vec.y) + ", " + String::num(vec.z) + ")"); \
  } while (0)

#define DBG_BOOL(category, name, value)                                    \
  do {                                                                     \
    auto* _dbg = DebugLogger::ensure_singleton();                          \
    if (_dbg)                                                              \
      _dbg->debug(String(category),                                        \
                  String(name) + ": " + String(value ? "true" : "false")); \
  } while (0)

// Assertion-like macro for debug builds
// Usage: DBG_ASSERT(ptr != nullptr, "Pointer", "ptr is null!");

#define DBG_ASSERT(condition, category, message)        \
  do {                                                  \
    if (!(condition)) {                                 \
      auto* _dbg = DebugLogger::ensure_singleton();     \
      if (_dbg)                                         \
        _dbg->error(String(category), String(message)); \
    }                                                   \
  } while (0)

// Conditional debug logging (only logs in DEBUG configuration)
// Build type is handled at compile time by checking NDEBUG
#ifdef NDEBUG
#define DBG_DEBUG_ONLY(category, message) ((void)0)
#else
#define DBG_DEBUG_ONLY(category, message) DBG_DEBUG(category, message)
#endif

#endif  // GDEXTENSION_DEBUG_MACROS_H

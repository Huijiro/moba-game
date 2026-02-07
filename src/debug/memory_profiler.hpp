#ifndef GDEXTENSION_MEMORY_PROFILER_H
#define GDEXTENSION_MEMORY_PROFILER_H

#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <map>
#include <string>
#include <vector>

using godot::Dictionary;
using godot::String;

/// Memory profiler for tracking VFX allocations and cleanup
/// Monitors:
/// - VFX node allocations/deallocations
/// - Child node creation/destruction
/// - Animation player lifetimes
/// - Mesh instance cleanup
class MemoryProfiler {
 public:
  struct AllocationRecord {
    std::string category;  // "VFXNode", "MeshInstance3D", etc.
    std::string name;      // Node name or description
    uint64_t timestamp;    // When allocated (ms since startup)
    bool is_alive = true;
    uint64_t freed_at = 0;  // When freed (0 if still alive)
  };

  struct CategoryStats {
    int allocated_count = 0;
    int freed_count = 0;
    int currently_alive = 0;
    uint64_t peak_alive = 0;
    uint64_t total_lifetime_ms = 0;  // Sum of all lifetimes
    double average_lifetime_ms = 0.0;
  };

  /// Track a new allocation
  static void track_allocation(const std::string& category,
                               const std::string& name);

  /// Track a deallocation
  static void track_deallocation(const std::string& category,
                                 const std::string& name);

  /// Get stats for a category
  static CategoryStats get_category_stats(const std::string& category);

  /// Get all category stats as Dictionary for display
  static Dictionary get_all_stats_dict();

  /// Get detailed allocation records for debugging
  static Dictionary get_detailed_records(const std::string& category);

  /// Clear all records (for session start)
  static void clear();

  /// Get summary report as string
  static String get_summary_report();

  /// Get currently alive allocations
  static Dictionary get_alive_allocations();

  /// Initialize profiler
  static void initialize();

  /// Check if profiling is enabled
  static bool is_enabled() { return enabled; }

  /// Enable/disable profiling
  static void set_enabled(bool e) { enabled = e; }

 private:
  static bool enabled;
  static std::map<std::string, std::vector<AllocationRecord>> allocations;
  static std::map<std::string, CategoryStats> stats;
  static uint64_t start_time_ms;

  static uint64_t get_current_time_ms();
  static void update_category_stats(const std::string& category);
};

#endif  // GDEXTENSION_MEMORY_PROFILER_H

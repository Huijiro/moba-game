#include "memory_profiler.hpp"

#include <chrono>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::Array;
using godot::UtilityFunctions;

// Static member initialization
bool MemoryProfiler::enabled = true;
std::map<std::string, std::vector<MemoryProfiler::AllocationRecord>>
    MemoryProfiler::allocations;
std::map<std::string, MemoryProfiler::CategoryStats> MemoryProfiler::stats;
uint64_t MemoryProfiler::start_time_ms = 0;

uint64_t MemoryProfiler::get_current_time_ms() {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

void MemoryProfiler::initialize() {
  if (start_time_ms == 0) {
    start_time_ms = get_current_time_ms();
  }
}

void MemoryProfiler::track_allocation(const std::string& category,
                                      const std::string& name) {
  if (!enabled)
    return;

  initialize();

  AllocationRecord record;
  record.category = category;
  record.name = name;
  record.timestamp = get_current_time_ms();
  record.is_alive = true;

  allocations[category].push_back(record);
  update_category_stats(category);
}

void MemoryProfiler::track_deallocation(const std::string& category,
                                        const std::string& name) {
  if (!enabled)
    return;

  initialize();

  // Find the most recent matching allocation (LIFO for proper pairing)
  auto it = allocations.find(category);
  if (it == allocations.end()) {
    return;
  }

  uint64_t current_time = get_current_time_ms();

  // Search backwards for the allocation
  for (int i = it->second.size() - 1; i >= 0; i--) {
    if (it->second[i].name == name && it->second[i].is_alive) {
      it->second[i].is_alive = false;
      it->second[i].freed_at = current_time;
      update_category_stats(category);
      return;
    }
  }
}

void MemoryProfiler::update_category_stats(const std::string& category) {
  auto& cat_allocs = allocations[category];
  auto& cat_stats = stats[category];

  cat_stats.allocated_count = 0;
  cat_stats.freed_count = 0;
  cat_stats.currently_alive = 0;
  cat_stats.peak_alive = 0;
  cat_stats.total_lifetime_ms = 0;

  uint64_t total_lifetime = 0;
  int alive_count = 0;
  int freed_count = 0;

  for (const auto& record : cat_allocs) {
    cat_stats.allocated_count++;

    if (record.is_alive) {
      alive_count++;
    } else {
      freed_count++;
      total_lifetime += (record.freed_at - record.timestamp);
    }
  }

  cat_stats.freed_count = freed_count;
  cat_stats.currently_alive = alive_count;
  cat_stats.peak_alive = std::max(cat_stats.peak_alive, (uint64_t)alive_count);
  cat_stats.total_lifetime_ms = total_lifetime;

  if (freed_count > 0) {
    cat_stats.average_lifetime_ms = (double)total_lifetime / freed_count;
  }
}

MemoryProfiler::CategoryStats MemoryProfiler::get_category_stats(
    const std::string& category) {
  auto it = stats.find(category);
  if (it != stats.end()) {
    return it->second;
  }
  return CategoryStats();
}

Dictionary MemoryProfiler::get_all_stats_dict() {
  Dictionary result;

  for (const auto& [category, cat_stats] : stats) {
    Dictionary cat_dict;
    cat_dict["allocated"] = cat_stats.allocated_count;
    cat_dict["freed"] = cat_stats.freed_count;
    cat_dict["alive"] = cat_stats.currently_alive;
    cat_dict["peak"] = (int)cat_stats.peak_alive;
    cat_dict["avg_lifetime_ms"] = cat_stats.average_lifetime_ms;
    result[godot::String(category.c_str())] = cat_dict;
  }

  return result;
}

Dictionary MemoryProfiler::get_detailed_records(const std::string& category) {
  Dictionary result;

  auto it = allocations.find(category);
  if (it == allocations.end()) {
    return result;
  }

  Array alive_records;
  Array freed_records;

  for (const auto& record : it->second) {
    Dictionary record_dict;
    record_dict["name"] = godot::String(record.name.c_str());
    record_dict["timestamp"] = (int64_t)record.timestamp;
    record_dict["alive"] = record.is_alive;

    if (!record.is_alive) {
      record_dict["freed_at"] = (int64_t)record.freed_at;
      record_dict["lifetime_ms"] =
          (int64_t)(record.freed_at - record.timestamp);
      freed_records.append(record_dict);
    } else {
      record_dict["age_ms"] =
          (int64_t)(get_current_time_ms() - record.timestamp);
      alive_records.append(record_dict);
    }
  }

  result["alive"] = alive_records;
  result["freed"] = freed_records;
  return result;
}

Dictionary MemoryProfiler::get_alive_allocations() {
  Dictionary result;

  for (const auto& [category, cat_allocs] : allocations) {
    Array alive;
    for (const auto& record : cat_allocs) {
      if (record.is_alive) {
        alive.append(godot::String(record.name.c_str()));
      }
    }
    if (alive.size() > 0) {
      result[godot::String(category.c_str())] = alive;
    }
  }

  return result;
}

String MemoryProfiler::get_summary_report() {
  String report = "=== Memory Profiler Report ===\n";

  for (const auto& [category, cat_stats] : stats) {
    report += godot::String("\n[") + category.c_str() + godot::String("]\n");
    report += godot::String("  Allocated: ") + cat_stats.allocated_count +
              godot::String("\n");
    report += godot::String("  Freed: ") + cat_stats.freed_count +
              godot::String("\n");
    report += godot::String("  Alive: ") + cat_stats.currently_alive +
              godot::String("\n");
    report += godot::String("  Peak Alive: ") + (int)cat_stats.peak_alive +
              godot::String("\n");
    report += godot::String("  Avg Lifetime: ") +
              cat_stats.average_lifetime_ms + godot::String(" ms\n");
  }

  return report;
}

void MemoryProfiler::clear() {
  allocations.clear();
  stats.clear();
  start_time_ms = get_current_time_ms();
}

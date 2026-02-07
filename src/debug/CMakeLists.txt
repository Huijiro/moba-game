# Debug visualization and logging systems
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./visual_debugger.hpp
  ./visual_debugger.cpp
  ./debug_logger.hpp
  ./debug_logger.cpp
  ./debug_macros.hpp
  ./debug_utils.hpp
  ./memory_profiler.hpp
  ./memory_profiler.cpp
)

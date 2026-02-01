# Debug visualization systems
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./debug_visuals.hpp
  ./debug_visuals.cpp
  ./godot_debug_drawer.hpp
  ./godot_debug_drawer.cpp
  ./debug_mesh_renderer.hpp
  ./debug_mesh_renderer.cpp
  ./skillshot_debug_renderer.hpp
  ./skillshot_debug_renderer.cpp
)

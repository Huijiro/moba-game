# Visual effect systems
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./vfx_node.hpp
  ./vfx_node.cpp
  ./vfx_manager.hpp
  ./vfx_manager.cpp
)

# Include VFX subdirectories
add_subdirectory(projectiles)
add_subdirectory(explosions)
add_subdirectory(area_effects)

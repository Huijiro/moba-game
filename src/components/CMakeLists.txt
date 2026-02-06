# Component system and all component modules
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./unit_component.hpp
  ./unit_component.cpp
)

# Component-specific subdirectories
add_subdirectory(movement)
add_subdirectory(combat)
add_subdirectory(health)
add_subdirectory(resources)
add_subdirectory(interaction)
add_subdirectory(abilities)
add_subdirectory(ui)

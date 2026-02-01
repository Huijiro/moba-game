# GDExtension registration (always needed)
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./register_types.hpp
  ./register_types.cpp
)

# Include all source modules
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(input)
add_subdirectory(camera)
add_subdirectory(components)
add_subdirectory(ai)

# Expose src directory for includes
target_include_directories(${PROJECT_NAME} PRIVATE "src")

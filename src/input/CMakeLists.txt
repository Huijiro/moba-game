# Input handling systems
target_sources(
  ${PROJECT_NAME} PRIVATE
  # Decomposed input system
  ./cursor_world_query.hpp
  ./cursor_world_query.cpp
  ./player_controller.hpp
  ./player_controller.cpp
  ./ability_targeting_handler.hpp
  ./ability_targeting_handler.cpp
  ./click_marker_handler.hpp
  ./click_marker_handler.cpp
  ./hover_highlight.hpp
  ./hover_highlight.cpp
  ./targeting_preview.hpp
  ./targeting_preview.cpp
)

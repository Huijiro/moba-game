#ifndef GDEXTENSION_UNIT_H
#define GDEXTENSION_UNIT_H

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {
class NavigationAgent3D;
}

using godot::CharacterBody3D;
using godot::PackedStringArray;
using godot::String;
using godot::Vector3;

class Unit : public CharacterBody3D {
  GDCLASS(Unit, CharacterBody3D)

 protected:
  static void _bind_methods();

 public:
  Unit();
  ~Unit();

  void _ready() override;
  void _physics_process(double delta) override;
  PackedStringArray _get_configuration_warnings() const override;

  void set_desired_location(const Vector3& location);
  Vector3 get_desired_location() const;

  void set_speed(float new_speed);
  float get_speed() const;

  godot::NavigationAgent3D* get_navigation_agent() const;

 private:
  void _cache_navigation_agent();
  godot::NavigationAgent3D* _find_navigation_agent() const;

  godot::NavigationAgent3D* navigation_agent = nullptr;
  Vector3 desired_location = Vector3(0, 0, 0);
  float speed = 5.0f;
  float rotation_speed = 10.0f;  // How fast the unit rotates to face direction
  bool is_ready = false;
  int32_t frame_count = 0;
};

#endif  // GDEXTENSION_UNIT_H

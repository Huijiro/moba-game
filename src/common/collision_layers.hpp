#ifndef COLLISION_LAYERS_HPP
#define COLLISION_LAYERS_HPP

#include <cstdint>

/// Physics collision layer assignments.
/// Godot uses bitmask layers (1-based in editor, 0-based in code).
///
/// Layer 1 (bit 0): World — terrain, obstacles, static geometry
/// Layer 2 (bit 1): Units — CharacterBody3D (players, enemies, NPCs)
/// Layer 3 (bit 2): Projectiles — Area3D projectiles (fireball, frost bolt)
/// Layer 4 (bit 3): Ability areas — AoE shapes, ability collision zones
namespace CollisionLayer {

constexpr uint32_t WORLD       = 1 << 0;  // Layer 1
constexpr uint32_t UNITS       = 1 << 1;  // Layer 2
constexpr uint32_t PROJECTILES = 1 << 2;  // Layer 3
constexpr uint32_t ABILITIES   = 1 << 3;  // Layer 4

}  // namespace CollisionLayer

#endif  // COLLISION_LAYERS_HPP

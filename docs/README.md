# MOBA Game Documentation

Complete documentation for the MOBA game development using Godot 4.6 and C++ GDExtension.

## Quick Navigation

### 🎮 Ability System
Start here to understand and work with the ability system.

- **[00_INDEX.md](ability-system/00_INDEX.md)** - Master index and quick reference
  - Overview of the ability system architecture
  - Quick links to all guides
  - Reference tables and common tasks
  
- **[01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md)** - Create new abilities
  - Complete step-by-step guide
  - Cast types and targeting paradigms
  - Frost Bolt example walkthrough
  - Custom effects and advanced features
  - Troubleshooting common issues

- **[02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md)** - Build and modify heroes
  - Creating new unit definitions
  - Modifying existing heroes
  - Using units in scenes
  - Creating unit variants
  - Support for minions and bosses

- **[03_TESTING.md](ability-system/03_TESTING.md)** - Test and verify abilities
  - Setup and configuration
  - Expected behavior reference
  - Testing checklist
  - Troubleshooting guide

### 🏗️ Setup & Development
Documentation for building and development workflow.

- **[AGENTS.md](../AGENTS.md)** - AI agent development guide
  - Project structure and architecture
  - Building and compilation
  - Common development tasks
  - Skills and workflow reference

### 📋 Structure

```
docs/
├── README.md                    (this file)
├── ability-system/
│   ├── 00_INDEX.md             (start here)
│   ├── 01_ADDING_ABILITIES.md  (create skills)
│   ├── 02_MODIFYING_UNITS.md   (build heroes)
│   └── 03_TESTING.md           (verify setup)
├── setup/
│   └── (build and environment guides)
└── development/
    └── (architecture and patterns)
```

## Getting Started

### For First-Time Users

1. **Learn the System** (5 min)
   - Read: [ability-system/00_INDEX.md](ability-system/00_INDEX.md)

2. **Create Your First Ability** (15 min)
   - Follow: [ability-system/01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md)
   - Use the Frost Bolt example as reference

3. **Build a Custom Hero** (10 min)
   - Follow: [ability-system/02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md)
   - Combine 4 abilities into a new unit definition

4. **Test Everything** (5 min)
   - Follow: [ability-system/03_TESTING.md](ability-system/03_TESTING.md)
   - Run in Godot editor and verify functionality

### For Developers

1. **Build the Project**
   - See [../AGENTS.md](../AGENTS.md) for build instructions

2. **Understand the Architecture**
   - Core components: Unit, AbilityComponent, UnitDefinition
   - See code references in ability system headers (below)

3. **Extend the System**
   - Refer to ability system guides for extending effects
   - Check C++ headers for API reference

## Code Reference

Quick links from C++ code to relevant documentation:

### UnitDefinition (src/common/unit_definition.hpp)
- **Purpose:** Data structure for unit configuration
- **Documentation:** [02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md)
- **Example:** TestHero.tres, MageHero.tres

### AbilityDefinition (src/components/abilities/ability_definition.hpp)
- **Purpose:** Configuration for individual abilities
- **Documentation:** [01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md)
- **Properties:** Cast type, targeting type, cost, cooldown, range
- **Example:** Q_Instant_Strike.tres, Frost_Bolt.tres

### AbilityComponent (src/components/abilities/ability_component.hpp)
- **Purpose:** Manages ability slots and casting state machine
- **Documentation:** [00_INDEX.md](ability-system/00_INDEX.md) - Core Components
- **Key Methods:** try_cast(), try_cast_point(), is_casting()
- **Features:** Cooldown tracking, mana validation, state management

### AbilityEffect (src/components/abilities/ability_effect.hpp)
- **Purpose:** Base class for ability gameplay effects
- **Documentation:** [01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md) - Step 1
- **Subclasses:** DamageEffect, AoEDamageEffect
- **Method:** execute(Unit* caster, Object* target)

### Unit (src/core/unit.hpp)
- **Purpose:** Character entity with abilities and components
- **Documentation:** [02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md)
- **Auto-loads abilities from UnitDefinition in _ready()

## Test Resources

Pre-configured test abilities and heroes:

**Test Abilities:**
- Q: Quick Strike (INSTANT, UNIT_TARGET, 25 dmg)
- W: Fireball (CAST_TIME, POINT_TARGET, 40 dmg)
- E: Energy Beam (CHANNEL, UNIT_TARGET, 50 dmg)
- R: Explosion (INSTANT, AREA, 35 dmg)
- Example: Frost Bolt (CAST_TIME, UNIT_TARGET, 35 dmg)

**Test Units:**
- TestHero (default test setup with Q/W/E/R)
- MageHero (variant with Frost Bolt as Q)

Location: `GodotGame/resources/`

## Key Concepts

### The 3-Part Ability System

1. **AbilityEffect** - What it does
   - DamageEffect, AoEDamageEffect, custom effects
   - Defined as `.tres` Resource files

2. **AbilityDefinition** - How it behaves
   - Cast type (INSTANT, CAST_TIME, CHANNEL)
   - Targeting type (UNIT_TARGET, POINT_TARGET, AREA, SELF_CAST)
   - Cost, cooldown, range
   - Defined as `.tres` Resource files

3. **UnitDefinition** - What abilities it has
   - 4 ability slots: Q, W, E, R
   - Works for heroes, minions, bosses, any unit type
   - Defined as `.tres` Resource files

### No C++ Required

All ability configuration happens in the Godot editor via `.tres` files. The C++ code provides the engine; the editor provides the content creation tool.

## Common Tasks

| Task | Guide | Time |
|------|-------|------|
| Create a new ability | [01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md) | 10 min |
| Create a new hero | [02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md) | 10 min |
| Replace one ability | [02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md) | 2 min |
| Test abilities in editor | [03_TESTING.md](ability-system/03_TESTING.md) | 5 min |
| Create a boss unit | [02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md) | 15 min |
| Fix mana not deducting | [03_TESTING.md](ability-system/03_TESTING.md) | varies |

## File Organization

```
src/
├── common/
│   ├── unit_definition.hpp/.cpp         ← See 02_MODIFYING_UNITS.md
│   └── unit_definition_manager.hpp/.cpp
├── components/abilities/
│   ├── ability_component.hpp/.cpp       ← See 00_INDEX.md
│   ├── ability_definition.hpp/.cpp      ← See 01_ADDING_ABILITIES.md
│   ├── ability_effect.hpp/.cpp          ← See 01_ADDING_ABILITIES.md
│   ├── ability_types.hpp
│   └── effects/
│       ├── damage_effect.hpp/.cpp
│       └── aoe_damage_effect.hpp/.cpp
├── core/
│   └── unit.hpp/.cpp                    ← See 02_MODIFYING_UNITS.md
└── input/
    └── input_manager.hpp/.cpp

GodotGame/resources/
├── abilities/
│   ├── Q_*.tres, W_*.tres, E_*.tres, R_*.tres
│   └── *Effect.tres
└── units/
    └── *Hero.tres
```

## Development Workflow

1. **Plan** - Design ability and hero concepts
2. **Create Effects** - Make AbilityEffect resources (if needed)
3. **Create Abilities** - Make AbilityDefinition resources
4. **Create Units** - Make UnitDefinition resources
5. **Test** - Verify in Godot editor
6. **Iterate** - Modify based on feedback

See [ability-system/00_INDEX.md](ability-system/00_INDEX.md) for complete workflow.

## Troubleshooting

**Abilities won't load?**
- See: [03_TESTING.md](ability-system/03_TESTING.md) - Troubleshooting

**Mana not deducting?**
- See: [03_TESTING.md](ability-system/03_TESTING.md) - Troubleshooting

**Unit definition not working?**
- See: [02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md) - Testing Your Modified Unit

**Ability never fires?**
- See: [01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md) - Common Mistakes

## Next Steps

- Create your first ability using [01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md)
- Build a custom hero using [02_MODIFYING_UNITS.md](ability-system/02_MODIFYING_UNITS.md)
- Expand with custom effects (requires C++ development)
- Add more complex targeting paradigms

## Contributing

When adding new features or abilities:
1. Create abilities following [01_ADDING_ABILITIES.md](ability-system/01_ADDING_ABILITIES.md)
2. Test thoroughly with [03_TESTING.md](ability-system/03_TESTING.md)
3. Document any new ability types or effects
4. Update [ability-system/00_INDEX.md](ability-system/00_INDEX.md) if needed

---

**Last Updated:** 2026-02-01
**Documentation Status:** Complete ✓
**System Status:** Production Ready ✓

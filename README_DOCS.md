# Documentation Structure

All project documentation has been organized into a dedicated `docs/` folder for easy navigation and maintenance.

## Quick Start

👉 **Start here:** [docs/README.md](docs/README.md)

This main documentation index provides:
- Quick navigation to all guides
- Overview of the ability system
- Code references from C++ headers
- Common tasks and workflows

## Documentation Structure

```
docs/
├── README.md                         (main index - START HERE)
└── ability-system/
    ├── 00_INDEX.md                  (quick reference)
    ├── 01_ADDING_ABILITIES.md       (create new skills)
    ├── 02_MODIFYING_UNITS.md        (build and modify heroes)
    └── 03_TESTING.md                (test and verify)
```

## What's Included

### Ability System Documentation
- **How to create new abilities** without writing C++ code
- **How to build heroes** by combining abilities
- **How to test** everything in the Godot editor
- **Reference materials** for cast types and targeting modes
- **Examples** with Frost Bolt, TestHero, MageHero

### Code Documentation
- **Header files** include documentation comments with references to relevant guides
- **Navigation:** C++ classes → Documentation files
- Examples:
  - `src/components/abilities/ability_definition.hpp` → docs/ability-system/01_ADDING_ABILITIES.md
  - `src/common/unit_definition.hpp` → docs/ability-system/02_MODIFYING_UNITS.md

## Key Documents

| Document | Purpose | Read Time |
|----------|---------|-----------|
| [docs/README.md](docs/README.md) | Main index and overview | 5 min |
| [docs/ability-system/00_INDEX.md](docs/ability-system/00_INDEX.md) | Quick reference | 10 min |
| [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md) | Create skills | 15 min |
| [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md) | Build heroes | 15 min |
| [docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md) | Test & verify | 10 min |

## For Different Users

### I want to create an ability
→ Read: [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md)

### I want to create a hero
→ Read: [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md)

### I want to understand the system
→ Read: [docs/README.md](docs/README.md) and [docs/ability-system/00_INDEX.md](docs/ability-system/00_INDEX.md)

### I want to test my changes
→ Read: [docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md)

### I'm a C++ developer
→ Check header comments in code for documentation links

## Development Information

For build instructions and project structure, see [AGENTS.md](AGENTS.md)

## Old Location

Previous documentation was in the root folder. Everything has been moved to `docs/`:
- ~~ABILITY_SYSTEM_INDEX.md~~ → docs/ability-system/00_INDEX.md
- ~~ADDING_ABILITIES.md~~ → docs/ability-system/01_ADDING_ABILITIES.md
- ~~MODIFYING_UNITS.md~~ → docs/ability-system/02_MODIFYING_UNITS.md
- ~~TEST_ABILITIES.md~~ → docs/ability-system/03_TESTING.md

---

**👉 Start with [docs/README.md](docs/README.md)**

# Ability System Guides Index

Quick index of all ability system documentation with descriptions and when to use each guide.

## 🎯 Start Here

Choose based on what you need:

### I want to TEST abilities immediately
→ **[QUICK_START.md](QUICK_START.md)** (5 minutes)
- Set TestHero on Unit node
- Press Q/W/E/R to test
- Troubleshooting for common issues

### I want to UNDERSTAND how abilities work
→ **[ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md)** (10 minutes)
- Visual overview of 4 slots (Q/W/E/R)
- Current TestHero setup details
- How data flows when you press a key
- File relationships

### I want to ASSIGN different abilities
→ **[ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md)** (15 minutes)
- 3-tier system explained (Effect → Definition → Unit)
- How to edit abilities in editor
- How to create custom heroes
- Examples and swap scenarios

### I want to SET UP input keys
→ **[INPUT_SETUP.md](INPUT_SETUP.md)** (5 minutes)
- Why InputMap actions were needed
- Q/W/E/R configuration
- Troubleshooting input errors
- How to customize keys

### I want STEP-BY-STEP for everything
→ **[docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md)**
- Complete testing procedures
- Expected behavior reference
- Verification checklist
- Advanced testing scenarios

---

## 📚 Detailed Guides

### For Understanding the System

| Guide | Time | Content |
|-------|------|---------|
| [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md) | 10 min | Visual overview of Q/W/E/R slots, data flow diagrams |
| [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) | 15 min | Complete 3-tier system, how to edit, examples |
| [docs/ability-system/00_INDEX.md](docs/ability-system/00_INDEX.md) | 15 min | System architecture, components, design |

### For Setting Up & Testing

| Guide | Time | Content |
|-------|------|---------|
| [QUICK_START.md](QUICK_START.md) | 5 min | Quick setup and test |
| [INPUT_SETUP.md](INPUT_SETUP.md) | 5 min | Input key configuration |
| [docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md) | 20 min | Comprehensive testing guide |

### For Creating Content

| Guide | Time | Content |
|-------|------|---------|
| [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md) | 15 min | Create new abilities step-by-step |
| [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md) | 10 min | Create and modify heroes |
| [docs/ability-system/04_LIBRARY_RELOAD.md](docs/ability-system/04_LIBRARY_RELOAD.md) | 5 min | Rebuild and reload library |

---

## 🗂️ File Organization

### Documentation Files

```
Root Level (Quick Reference):
├─ QUICK_START.md                    ← Start here for testing
├─ ABILITY_ASSIGNMENT.md             ← Understand ability system
├─ ABILITY_SLOTS_VISUAL.md           ← Visual reference (Q/W/E/R)
├─ INPUT_SETUP.md                    ← Input configuration
└─ ABILITY_GUIDES_INDEX.md           ← This file

Comprehensive Guides:
docs/ability-system/
├─ 00_INDEX.md                       ← Architecture overview
├─ 01_ADDING_ABILITIES.md            ← Create abilities
├─ 02_MODIFYING_UNITS.md             ← Create heroes
├─ 03_TESTING.md                     ← Testing procedures
└─ 04_LIBRARY_RELOAD.md              ← Build procedures

Main Documentation:
├─ README.md                         ← Root docs index
└─ docs/README.md                    ← Docs folder index
```

### Resource Files

```
GodotGame/resources/

abilities/
├─ TestDamageEffect.tres             ← Effect: 25 dmg
├─ TestAoEEffect.tres                ← Effect: 35 dmg AOE
├─ Q_Instant_Strike.tres             ← Q ability (uses TestDamageEffect)
├─ W_Fireball.tres                   ← W ability (uses TestDamageEffect)
├─ E_Beam.tres                       ← E ability (uses TestDamageEffect)
└─ R_Explosion.tres                  ← R ability (uses TestAoEEffect)

units/
├─ TestHero.tres                     ← Main test hero (Q/W/E/R setup)
└─ MageHero.tres                     ← Variant with Frost Bolt as Q
```

### Source Code Files

```
src/components/abilities/
├─ ability_effect.hpp/.cpp           ← Base effect class (now extends Resource)
├─ ability_definition.hpp/.cpp       ← Ability configuration resource
├─ ability_component.hpp/.cpp        ← Manages casting & cooldowns
├─ ability_types.hpp                 ← Enums (CastType, TargetingType)
└─ effects/
   ├─ damage_effect.hpp/.cpp         ← Single-target damage
   └─ aoe_damage_effect.hpp/.cpp     ← Area damage

src/common/
├─ unit_definition.hpp/.cpp          ← Unit with 4 ability slots
└─ unit_definition_manager.hpp/.cpp  ← Unit registry

src/input/
└─ input_manager.cpp                 ← Q/W/E/R input handling (lines 375-405)

src/core/
└─ unit.cpp                          ← Auto-loads abilities from UnitDefinition
```

---

## 🎮 Common Tasks

### Setup & First Test

| Task | Guide | Time |
|------|-------|------|
| Get abilities working | [QUICK_START.md](QUICK_START.md) | 5 min |
| Fix InputMap errors | [INPUT_SETUP.md](INPUT_SETUP.md) | 5 min |
| Understand how it works | [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md) | 10 min |

### Testing & Debugging

| Task | Guide | Time |
|------|-------|------|
| Comprehensive testing | [docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md) | 20 min |
| Q/W/E/R not working | [INPUT_SETUP.md](INPUT_SETUP.md) or [docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md) | varies |
| Abilities don't load | [QUICK_START.md](QUICK_START.md) Troubleshooting | 5 min |
| Mana not deducting | [docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md) | varies |

### Creating Content

| Task | Guide | Time |
|------|-------|------|
| Create new ability | [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md) | 15 min |
| Create custom hero | [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md) | 10 min |
| Swap abilities | [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) "Example: Swap Abilities" | 2 min |
| Change Q damage | [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) "How to Edit" | 2 min |
| Change Q cooldown | [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md) "Quick Changes" | 2 min |

### Advanced

| Task | Guide | Time |
|------|-------|------|
| Rebuild library | [docs/ability-system/04_LIBRARY_RELOAD.md](docs/ability-system/04_LIBRARY_RELOAD.md) | 5 min |
| Create custom effect | [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md) "Custom Effects" | 30 min |
| Understand code flow | [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) "Code Behind Scenes" | 10 min |

---

## 🚀 Quick Start Paths

### Path 1: "Just Let Me Test" (15 minutes)
1. Read: [QUICK_START.md](QUICK_START.md) (5 min)
2. Set TestHero on Unit (2 min)
3. Play and test Q/W/E/R (3 min)
4. Troubleshoot if needed (5 min)

### Path 2: "I Want to Understand Everything" (45 minutes)
1. Read: [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md) (10 min)
2. Read: [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) (15 min)
3. Read: [docs/ability-system/00_INDEX.md](docs/ability-system/00_INDEX.md) (15 min)
4. Test in editor (5 min)

### Path 3: "I Want to Create Custom Abilities" (90 minutes)
1. Read: [QUICK_START.md](QUICK_START.md) (5 min)
2. Test current system (5 min)
3. Read: [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) (15 min)
4. Read: [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md) (15 min)
5. Create custom ability (20 min)
6. Read: [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md) (10 min)
7. Create custom hero (15 min)
8. Test custom hero (10 min)

---

## 📖 Learning Structure

### Beginner: "Just Works"
```
Set TestHero → Press Q/W/E/R → See damage
           ↓
      Works!
```

### Intermediate: "I Understand"
```
Effect (what)
   ↓
Definition (how + effect)
   ↓
Unit (which + 4 definitions)
   ↓
Press Q/W/E/R → Casts slot 0/1/2/3
```

### Advanced: "I Can Create"
```
Create AbilityEffect
   ↓
Create AbilityDefinition
   ↓
Create UnitDefinition
   ↓
Use in scene
   ↓
Test and iterate
```

---

## 🔍 Finding Information

### "I want to know about..."

**Ability slots (Q/W/E/R)**
- Quick: [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md)
- Detailed: [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md)

**3-tier system**
- Complete: [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md)
- Architecture: [docs/ability-system/00_INDEX.md](docs/ability-system/00_INDEX.md)

**Current setup**
- TestHero details: [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md)
- How it loads: [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) "Code Behind Scenes"

**Creating abilities**
- Step-by-step: [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md)
- Examples: [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md)

**Creating heroes**
- Step-by-step: [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md)
- Quick examples: [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md)

**Testing**
- Comprehensive: [docs/ability-system/03_TESTING.md](docs/ability-system/03_TESTING.md)
- Quick test: [QUICK_START.md](QUICK_START.md)

**Input setup**
- Q/W/E/R config: [INPUT_SETUP.md](INPUT_SETUP.md)
- Troubleshooting: [INPUT_SETUP.md](INPUT_SETUP.md) "If You Still See Errors"

**Building/reloading**
- Library rebuild: [docs/ability-system/04_LIBRARY_RELOAD.md](docs/ability-system/04_LIBRARY_RELOAD.md)

---

## 📊 Content Map

```
                    ABILITY SYSTEM DOCUMENTATION
                              |
                    ┌─────────┼─────────┐
                    |         |         |
              SETUP & TEST   UNDERSTAND   CREATE
                    |         |         |
        ┌───────────┼────┐   |    ┌────┼─────────┐
        |           |    |   |    |    |         |
    QUICK_START  INPUT  TEST SLOTS VISUAL CUSTOM CUSTOM
        |         SETUP  3.md  REF  GUIDE ABILITIES HEROES
        5min      5min   20min 10min 15min  15min   10min
        
        
              MAIN RESOURCES
                    |
    ┌───────────────┼───────────────┐
    |               |               |
  Effects      Abilities        Heroes
    |               |               |
DamageEffect   Q_Strike         TestHero
AoEEffect      W_Fireball       MageHero
               E_Beam
               R_Explosion
```

---

## ✅ Checklist: Quick Start

- [ ] Read [QUICK_START.md](QUICK_START.md)
- [ ] Restart Godot
- [ ] Set TestHero on Unit node
- [ ] Play scene
- [ ] Press Q/W/E/R keys
- [ ] See abilities execute
- [ ] Check console output
- [ ] Verify mana deducts
- [ ] Test cooldowns
- [ ] Read [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md) to understand

---

## 🎯 Decision Tree

```
START
  |
  ├─ Do abilities work?
  │   ├─ NO: Follow [QUICK_START.md](QUICK_START.md)
  │   └─ YES: Continue
  │
  ├─ Do you understand how it works?
  │   ├─ NO: Read [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md)
  │   └─ YES: Continue
  │
  ├─ Do you want to modify abilities?
  │   ├─ YES: Read [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md)
  │   └─ NO: Continue
  │
  ├─ Do you want to create custom abilities?
  │   ├─ YES: Read [docs/ability-system/01_ADDING_ABILITIES.md](docs/ability-system/01_ADDING_ABILITIES.md)
  │   └─ NO: Continue
  │
  └─ Do you want to create custom heroes?
      ├─ YES: Read [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md)
      └─ NO: You're done! Explore as you like
```

---

## 📞 Quick Reference

| Question | File | Location |
|----------|------|----------|
| How do I test? | QUICK_START.md | Root |
| How do I assign abilities? | ABILITY_ASSIGNMENT.md | Root |
| What are the slots? | ABILITY_SLOTS_VISUAL.md | Root |
| How do I configure input? | INPUT_SETUP.md | Root |
| How do I create abilities? | 01_ADDING_ABILITIES.md | docs/ability-system/ |
| How do I create heroes? | 02_MODIFYING_UNITS.md | docs/ability-system/ |
| How do I test thoroughly? | 03_TESTING.md | docs/ability-system/ |
| How do I rebuild? | 04_LIBRARY_RELOAD.md | docs/ability-system/ |
| What's the system architecture? | 00_INDEX.md | docs/ability-system/ |

---

**Status:** ✅ Complete
**Last Updated:** February 1, 2026
**Total Documentation:** 12 comprehensive guides
**Estimated Learning Time:** 5-90 minutes depending on path

Start with [QUICK_START.md](QUICK_START.md) to test immediately!

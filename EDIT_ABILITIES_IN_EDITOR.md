# How to Edit Abilities in Godot Editor

Now that the abilities array is exposed in the editor, you can easily assign and swap abilities without editing .tres files manually.

## Viewing Abilities

### Method 1: Edit Through UnitDefinition Resource (Recommended)

1. **Open File Browser**
   - In Godot, click "FileSystem" tab (bottom left)

2. **Navigate to Units**
   - Double-click `res://resources/units/TestHero.tres`

3. **View Abilities in Inspector**
   - The Inspector (right side) now shows:
   ```
   TestHero.tres
   ├─ unit_name: "TestHero"
   ├─ unit_id: 1
   ├─ description: "Test hero with all 4 ability types"
   ├─ unit_type: "hero"
   └─ Abilities (Array, size=4)
      ├─ [0]: Quick Strike (Q_Instant_Strike.tres)
      ├─ [1]: Fireball (W_Fireball.tres)
      ├─ [2]: Energy Beam (E_Beam.tres)
      └─ [3]: Explosion (R_Explosion.tres)
   ```

### Method 2: View Through Unit Node in Scene

1. **Open main.tscn**
   - `GodotGame/main.tscn`

2. **Select Unit Node**
   - Click **Unit** in Scene tree

3. **View in Inspector**
   - Look for **Unit Definition** = TestHero.tres
   - Scroll down to see **Abilities** array
   - Shows same 4 slots [0], [1], [2], [3]

## Editing Abilities

### Assign an Ability to a Slot

1. **Open the UnitDefinition resource**
   - Double-click `TestHero.tres`

2. **Click the slot you want to edit**
   - Click `[0]` to edit Q ability
   - Click `[1]` to edit W ability
   - Click `[2]` to edit E ability
   - Click `[3]` to edit R ability

3. **A resource picker appears**
   - Browse to `res://resources/abilities/`
   - Select an ability (e.g., `Q_Instant_Strike.tres`)
   - Click "Open"

### Swap Two Abilities

**Example: Put Fireball on Q instead of Quick Strike**

1. Open `TestHero.tres`
2. Current setup:
   ```
   [0]: Q_Instant_Strike.tres
   [1]: W_Fireball.tres
   ```
3. Click `[0]` → Select `W_Fireball.tres`
4. Click `[1]` → Select `Q_Instant_Strike.tres`
5. Result:
   ```
   [0]: W_Fireball.tres       ← Now Fireball is on Q!
   [1]: Q_Instant_Strike.tres ← Now Quick Strike is on W!
   ```
6. Save and play
7. Press Q → Casts Fireball (with 1 second cast time)
8. Press W → Casts Quick Strike (instant)

### Clear a Slot

1. Open `TestHero.tres`
2. Click the slot (e.g., `[0]`)
3. In the property, click the "X" button
4. Slot is now empty
5. Save

**Result:** When Unit loads, that slot won't have an ability

## Creating a New Hero

### Step 1: Create New UnitDefinition

1. **Right-click in FileSystem**
   - Navigate to `res://resources/units/`
   - Right-click in empty space
   - Select "New Resource"

2. **Choose UnitDefinition**
   - Type or search for "UnitDefinition"
   - Click "Create"

3. **Save the file**
   - Name it (e.g., `MyHero.tres`)
   - Click "Save"

### Step 2: Configure Properties

1. **Inspector shows new resource**
   ```
   MyHero.tres
   ├─ unit_name: "Unit"        ← Change to "My Hero"
   ├─ unit_id: 0               ← Change to 2 (or next available)
   ├─ description: ""          ← Add description
   ├─ unit_type: "hero"        ← Keep as "hero"
   └─ Abilities (Array, size=4)
      ├─ [0]: (empty)
      ├─ [1]: (empty)
      ├─ [2]: (empty)
      └─ [3]: (empty)
   ```

2. **Edit unit_name**
   - Click value next to "unit_name"
   - Type "My Hero"
   - Press Enter

3. **Edit unit_id**
   - Click value next to "unit_id"
   - Type "2"
   - Press Enter

### Step 3: Assign Abilities

1. **Click [0]**
   - Resource picker opens
   - Navigate to `res://resources/abilities/`
   - Select an ability (e.g., `Q_Instant_Strike.tres`)
   - Click "Open"

2. **Click [1]**
   - Select different ability (e.g., `W_Fireball.tres`)
   - Click "Open"

3. **Click [2]**
   - Select ability (e.g., `E_Beam.tres`)
   - Click "Open"

4. **Click [3]**
   - Select ability (e.g., `R_Explosion.tres`)
   - Click "Open"

5. **Result:**
   ```
   MyHero.tres
   ├─ unit_name: "My Hero"
   ├─ unit_id: 2
   └─ Abilities
      ├─ [0]: Q_Instant_Strike.tres
      ├─ [1]: W_Fireball.tres
      ├─ [2]: E_Beam.tres
      └─ [3]: R_Explosion.tres
   ```

### Step 4: Use in Scene

1. **Open main.tscn**
   - `GodotGame/main.tscn`

2. **Select Unit node**
   - Click **Unit** in Scene tree

3. **Set Unit Definition**
   - In Inspector, find **Unit Definition** property
   - Click the resource picker
   - Navigate to `res://resources/units/`
   - Select `MyHero.tres`
   - Click "Open"

4. **Play and test**
   - Press F5 or click Play
   - Press Q/W/E/R to test your custom hero's abilities

## Quick Reference: Array Indices

Use these to remember which slot is which:

```
Index 0 → Q key → Q ability
Index 1 → W key → W ability
Index 2 → E key → E ability
Index 3 → R key → R ability
```

## Editor UI Elements

### When Editing an Ability Slot

```
[0]  ← Click here to edit
│
├─ Folder icon   ← Pick from file browser
├─ Resource name ← Current ability
└─ X button      ← Clear this slot
```

### Array Size

```
Abilities (Array, size=4)
          ↑                ↑
         Name             Total slots (always 4)
```

The array is always locked to 4 slots because units always have Q/W/E/R.

## Tips & Tricks

### Bulk Replace
To give the same ability to multiple heroes:
1. Create ability in one hero
2. Copy by clicking and dragging in the inspector
3. Paste into other heroes

### Duplicate Hero
To create a variant:
1. Right-click UnitDefinition file → "Duplicate"
2. Rename the duplicate (e.g., `MyHero_Variant.tres`)
3. Edit abilities as desired
4. Use both in different scenes

### Missing Ability?
If you can't find an ability:
1. Check `res://resources/abilities/` folder
2. Make sure the ability is saved as `.tres`
3. Refresh FileSystem view (F5)
4. Try again

## Common Workflow

### Daily Setup

```
1. Open FileSystem
2. Navigate to units folder
3. Double-click the hero you want
4. Scroll to Abilities array
5. Click [slot] to change ability
6. Select new ability from picker
7. Save
8. Test in scene
```

### Creating Custom Heroes

```
1. Create new UnitDefinition
2. Set unit_name
3. Set unit_id (unique number)
4. Assign abilities to [0] [1] [2] [3]
5. Save
6. Set on Unit node
7. Play and test
```

## Troubleshooting

### I don't see the Abilities array
- Reload the project: File → Reload Project
- Restart Godot completely
- The library might not have reloaded

### I can't click on the ability slot
- Make sure UnitDefinition is open (not Unit node)
- Look for "Abilities" in Inspector
- Click the small arrow to expand if collapsed

### The resource picker doesn't show abilities
- Make sure you're in `res://resources/abilities/`
- Check that ability files are `.tres` format
- Refresh: Right-click folder → "Refresh"

### I deleted an ability by mistake
- Ctrl+Z to undo
- Or select the slot and click X to re-clear, then re-add

## See Also

- [QUICK_START.md](QUICK_START.md) - Quick testing guide
- [ABILITY_ASSIGNMENT.md](ABILITY_ASSIGNMENT.md) - Complete explanation
- [ABILITY_SLOTS_VISUAL.md](ABILITY_SLOTS_VISUAL.md) - Visual reference
- [docs/ability-system/02_MODIFYING_UNITS.md](docs/ability-system/02_MODIFYING_UNITS.md) - Detailed guide

---

**Status:** ✅ Abilities array now editable in editor
**Test:** Open TestHero.tres and see the 4 ability slots
**Next:** Create your first custom hero!

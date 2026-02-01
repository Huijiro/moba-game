# Library Rebuild and Reloading

When you modify C++ code and rebuild the library (libGodotGame-d.so), Godot needs to reload it to recognize the changes.

## Problem: "Can't create sub resource of type 'DamageEffect'"

This error appears when:
1. You've rebuilt the C++ library
2. Godot is still using the old cached library
3. Resource types aren't recognized

## Solution: Full Godot Restart

### Quick Fix
1. **Close Godot Editor completely**
   - File → Exit (or close the window)
   - Wait 2-3 seconds

2. **Rebuild the library**
   ```bash
   cd /path/to/rpg-game
   ninja
   ```

3. **Reopen Godot**
   ```bash
   cd GodotGame
   godot4 -e
   ```
   or open the project from Godot's project manager

4. **Reload the project**
   - Godot may show a "Project Has Been Modified" dialog
   - Click "Reload" to refresh all resources

### Step-by-Step Example

```bash
# 1. You're in the code, make changes to ability_effect.cpp
nano src/components/abilities/ability_effect.cpp

# 2. Rebuild
cd /home/huijiro/Dev/rpg-game
ninja

# 3. Check rebuild succeeded (look for linking line)
ls -lh GodotGame/lib/Linux-x86_64/libGodotGame-d.so

# 4. Close Godot in GUI
# (File → Exit or Ctrl+Q)

# 5. Wait a moment
sleep 2

# 6. Reopen Godot
cd GodotGame
godot4 -e

# 7. If prompted, reload the project
```

## Why This Happens

Godot loads C++ extensions as shared libraries (.so files). When you:
1. Open Godot, it loads libGodotGame-d.so into memory
2. Rebuild the library, a new .so file is written to disk
3. Godot continues using the old version in memory
4. Your new classes (like DamageEffect) aren't recognized

Simply restarting Godot forces it to load the updated .so file.

## Hot Reload (Not Supported)

Godot's hot reload feature is **not recommended** for C++ GDExtensions:
- Can cause crashes
- May leave invalid state
- Not reliably supported for .so unloading/reloading
- Full restart is safer and more reliable

## Checking if Library Loaded Correctly

Once Godot restarts and loads properly:

1. Open main.tscn in editor
2. Select a Unit node
3. In Inspector, look at the ResourcePoolComponent
4. Try to set the `effect` property
5. A resource picker should appear
6. You should see DamageEffect and AoEDamageEffect as options

If you DON'T see them, the library didn't load. Try the restart procedure again.

## Troubleshooting

### Library Still Not Recognized
- Double-check ninja completed without errors
- Look for "Linking CXX" in build output
- Check file timestamp: `stat GodotGame/lib/Linux-x86_64/libGodotGame-d.so`

### Godot Crashes on Startup
- Library may have compile errors
- Check build output for errors
- Recompile and fix any issues

### Resources Still Show Parse Errors
- Godot may be using a partially-loaded library
- Try restarting again
- If persists, delete `.godot/` folder and reopen project

## Avoiding Library Changes

To avoid rebuilding:
- **Modify .tres files** in editor (abilities, units, effects)
- **No rebuild needed** for ability configuration changes
- **Rebuild only when** changing C++ code

Most ability system work doesn't require code changes—everything can be configured in the editor!

## Next Steps

After restart and successful reload:
1. Create and test your abilities
2. See [03_TESTING.md](03_TESTING.md) for verification

For more info:
- See [01_ADDING_ABILITIES.md](01_ADDING_ABILITIES.md) to create effects
- See [02_MODIFYING_UNITS.md](02_MODIFYING_UNITS.md) to assign abilities

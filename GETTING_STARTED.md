# Getting Started with MOBA Development

This guide helps you start your first session with OpenCode on the moba-game project.

## Prerequisites

1. **OpenCode installed**: https://opencode.ai/docs#install
2. **API key configured**: Run `opencode` and follow `/connect` to set up a provider
3. **Godot 4.6 installed**: For testing components in the engine
4. **CMake & Ninja**: For building C++ code

## First Time Setup

### 1. Initialize the Project
```bash
cd /path/to/moba-game
opencode
/init
```

This creates OpenCode's understanding of your project (uses AGENTS.md).

### 2. Review Key Documents
- **MOBA_ROADMAP.md** - What we're building (high-level)
- **AGENTS.md** - How the code is structured (technical)
- **OPENCODE_WORKFLOW.md** - How to use OpenCode (workflow)

## Your First Session

### Step 1: Start OpenCode
```bash
cd /path/to/moba-game
opencode
```

### Step 2: Pick Your First Task
Ask OpenCode to show available work:
```
What tasks are ready to work on from moba-game?
```

Or ask about a specific issue:
```
Tell me about issue #3
```

### Step 3: Load the Right Skill
Depending on your task:
- **Building a component** → Load `moba-component-development`
- **Need to test/build** → Load `moba-testing`
- **Understanding requirements** → Load `moba-github-workflow`
- **Want full overview** → Load `moba-workflow-overview`

```
/skill moba-component-development
```

### Step 4: Work on the Task
Follow OpenCode's guidance:
```
How do I implement a new component?
```

```
Create the AttackComponent class following the pattern
```

### Step 5: Build and Test
```
Build the code to verify it compiles
```

```
How do I test this in Godot?
```

### Step 6: Complete the Task
```
Create a PR for this work, referencing issue #3
```

OpenCode will create commits with `Fixes #3`, which auto-closes the issue when merged.

## Understanding the Workflow

### GitHub Projects (Source of Truth)
- **Board**: https://github.com/Huijiro/moba-game/projects/7
- **Issues**: Describe what needs to be done
- **Columns**: Track progress (Backlog → Ready → Done)

### Skills (Guided Development)
- **moba-github-workflow**: Track work, pick tasks, create PRs
- **moba-component-development**: Build MOBA components
- **moba-testing**: Verify code compiles and works
- **moba-workflow-overview**: See complete workflows

### Your Work (Making Changes)
1. Pick a Ready task from GitHub Projects
2. Load appropriate skill
3. Implement with OpenCode's help
4. Build and test
5. Create PR that references issue
6. When merged, issue closes automatically

## Key Concepts

### Issues and Tasks
- **FEATURE**: Larger capability (breaks into subtasks)
  - Example: #2 "Combat System Foundation"
  - Usually has 3+ subtasks
  
- **TASK**: 2-4 hour focused work
  - Example: #3 "Create AttackComponent class"
  - Can be completed in one session
  - Clear acceptance criteria

### Components
Everything in `/src/components/` extends `UnitComponent`:
- **HealthComponent**: Track unit health
- **AttackComponent**: Handle attacks and damage
- **MovementComponent**: Movement logic
- **ResourcePoolComponent**: Mana/energy

Each component:
- Has `.hpp` (header) and `.cpp` (implementation)
- Registered with Godot via `_bind_methods()`
- Gets owner Unit reference in `_enter_tree()`

### Building and Testing
```bash
# Build the C++ code
ninja

# Test in Godot
# - Open GodotGame/main.tscn
# - Press F5 or click Play
# - Verify component behavior
```

## Example First Session: Issue #3

### The Task
**Title**: Create AttackComponent class

**Description**: Implement the AttackComponent class as part of combat system foundation

**Acceptance Criteria**:
- [ ] AttackComponent.hpp/.cpp created
- [ ] Class extends UnitComponent
- [ ] Registered with Godot
- [ ] Has properties: attack_damage, attack_range, attack_cooldown
- [ ] Compiles without errors

### Your Session Flow
```
1. opencode
2. "Tell me about issue #3"
3. /skill moba-component-development
4. "Create AttackComponent following the pattern"
5. (OpenCode implements the component)
6. /skill moba-testing
7. "Build to verify it compiles"
8. (Check Godot Editor - properties visible)
9. /skill moba-github-workflow
10. "Create a PR for this work"
11. (OpenCode creates commits with Fixes #3)
12. (You review and merge)
13. Issue #3 auto-closes
```

## Useful Commands in OpenCode

```
/skill [name]          Load a skill (see full documentation)
/undo                  Undo last changes
/redo                  Redo undone changes
/share                 Share this conversation
Tab                    Switch between Plan and Build mode
@filename.txt          Reference a file in your question
```

## File Locations Quick Reference

| What | Where |
|------|-------|
| Component implementations | `src/components/` |
| Main test scene | `GodotGame/main.tscn` |
| GitHub project board | https://github.com/Huijiro/moba-game/projects/7 |
| OpenCode skills | `.opencode/skills/` |
| Architecture guide | `AGENTS.md` |
| Workflow guide | `OPENCODE_WORKFLOW.md` |
| Feature roadmap | `MOBA_ROADMAP.md` |

## Troubleshooting First Session

### "I don't know what to work on"
```
/skill moba-github-workflow
What tasks are in the Ready column?
```

### "I don't understand the requirements"
```
/skill moba-github-workflow
Tell me about issue #3 in detail
```

### "I'm not sure how to implement this"
```
/skill moba-component-development
Show me the pattern for creating a new component
```

### "The code won't compile"
```
/skill moba-testing
The build failed with this error: [paste error]
```

### "I don't know how to create a PR"
```
/skill moba-github-workflow
Create a PR for this work
```

## Next Steps

1. **Read OPENCODE_WORKFLOW.md** - Full workflow guide
2. **Check GitHub Projects** - See available tasks
3. **Pick a Ready task** - Start with #3, #4, or #5
4. **Run opencode** - Start your first session
5. **Load appropriate skill** - Follow guidance
6. **Build and test** - Verify your work
7. **Create PR** - Close the issue
8. **Repeat!** - Pick next task

## Tips for Success

1. **Start simple**: Pick task #3 first, it's foundational
2. **Load skills early**: Don't guess, use skill guidance
3. **Build often**: Don't wait until the end
4. **Test in Godot**: Verify components work in engine
5. **Commit frequently**: Small commits are easier to review
6. **Reference issues**: Use `Fixes #number` in commits
7. **Ask questions**: Use OpenCode - that's what it's for!

## Resources

- **OpenCode Docs**: https://opencode.ai/docs
- **Godot 4.6 Docs**: https://docs.godotengine.org/en/stable/
- **Project Architecture**: See AGENTS.md
- **Workflow Details**: See OPENCODE_WORKFLOW.md

---

Ready to start? Run `opencode` and ask it to show you available tasks! 🚀

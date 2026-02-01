# OpenCode + MOBA Development Workflow

This document describes how to work effectively with OpenCode on the moba-game project.

## Quick Start

### 1. Start Your Session
```bash
cd /path/to/moba-game
opencode
```

### 2. Pick What to Work On
```
You: What ready tasks are available from moba-game?

OpenCode will show you issues from the GitHub Projects board.
```

**Or pick a specific issue:**
```
You: I want to work on issue #3: Create AttackComponent
```

### 3. Work & Build
OpenCode will help you implement the feature, then test it:
```
You: I'm done, let's build and test

OpenCode will:
- Build the code with ninja
- Verify compilation succeeds
- Suggest manual testing in Godot
- Check acceptance criteria
```

### 4. Create PR & Close Issue
```
You: Create a PR for this work

OpenCode will:
- Create commits with descriptive messages
- Reference the issue: Fixes #3
- Provide PR summary
- Close the issue when merged
```

## The Skills

Load a skill by asking OpenCode about a topic, or explicitly:
```
/skill moba-github-workflow
```

### `moba-github-workflow`
**When to use:** Starting sessions, picking tasks, creating PRs

Key questions:
- "What tasks are ready from moba-game?"
- "I'm working on issue #X"
- "Create a PR for this work"

### `moba-component-development`
**When to use:** Creating or modifying UnitComponents

Key questions:
- "How do I create a new component?"
- "Show me the pattern for AttackComponent"
- "How do I bind this to Godot?"

### `moba-testing`
**When to use:** Building, testing, debugging

Key questions:
- "Build the code"
- "How do I verify this works?"
- "The build is failing, help me debug"

### `moba-workflow-overview`
**When to use:** Understanding the complete process

Key questions:
- "What's the full workflow for implementing a feature?"
- "When should I use each skill?"
- "Show me example workflows"

## GitHub Projects Board

All work is tracked in: https://github.com/Huijiro/moba-game/projects/7

**Columns:**
- **Backlog** - New ideas, not ready
- **Ready** - Clear scope, ready to work (2-4 hour chunks)
- **In Progress** - Currently being worked on
- **In Review** - PR open, waiting for merge
- **Done** - Merged and complete

## Issue Types

### FEATURE (Larger capability)
Example: #2 Combat System Foundation

- Has multiple subtasks
- Each subtask is a separate issue
- Feature issue tracks overall progress
- Work on subtasks, not the feature itself

### TASK (Ready to implement)
Example: #3 Create AttackComponent

- 2-4 hour focused scope
- Clear acceptance criteria
- Ready to start immediately
- Should have all info to complete

## Workflow by Task Type

### New Component Implementation
```
1. Load moba-component-development skill
2. Ask about the pattern
3. Implement following examples
4. Load moba-testing skill
5. Build and verify
6. Load moba-github-workflow skill
7. Create PR with issue reference
```

### Bug Fix / Enhancement
```
1. Load moba-github-workflow skill
2. Understand the issue
3. Make targeted changes
4. Load moba-testing skill
5. Build and test the fix
6. Create PR with issue reference
```

### Debugging Build Failures
```
1. Load moba-testing skill
2. Ask for help with the error
3. OpenCode diagnoses and fixes
4. Rebuild to verify
5. Commit the fix
```

## Key Patterns

### Picking Your Work
```bash
# At start of session, ask:
"What tasks are in 'Ready' for moba-game?"

# Or pick directly:
"I'm going to work on #5: Add damage calculation"
```

### Committing Changes
```bash
# Good commit format:
"Implement cooldown timer for attacks

- Add cooldown_time member variable
- Implement is_ready() check
- Reset cooldown after attack fires

Fixes #4"
```

### Creating PRs
```bash
# When work is complete:
"Create a PR for this work"

OpenCode will:
- Title references the issue
- Body includes Fixes #number
- Auto-closes when merged
```

## Best Practices

### 1. **Always Start with the Issue**
Read the acceptance criteria before coding. Understanding the requirement prevents rework.

### 2. **Work on 2-4 Hour Chunks**
If a task feels bigger, ask if it breaks into subtasks. Smaller tasks = faster feedback.

### 3. **Reference Issues in Commits**
Use `Fixes #number` in commit messages. GitHub auto-closes when merged.

```
Implement attack range checking - Fixes #5
```

### 4. **Build Frequently**
Don't wait until the end to build. Catch compilation errors early.

```
"Build the code to check for errors"
```

### 5. **Test Against Acceptance Criteria**
When done, verify each criterion from the issue:
```
Acceptance Criteria:
- [ ] AttackComponent class created and registered
- [ ] Properties exposed in Godot Inspector
- [ ] Can attack other units in range
```

### 6. **One Feature Per PR**
Don't mix features in a single PR. Makes review and debugging easier.

### 7. **Commit Early and Often**
Don't batch changes. Frequent commits = better history and easier debugging.

## Common Questions

### "What should I work on?"
Load `moba-github-workflow` and ask:
```
What tasks are in the 'Ready' column?
```

### "How do I implement a component?"
Load `moba-component-development` and ask:
```
Show me the pattern for creating a new component
```

### "The code won't compile"
Load `moba-testing` and share the error:
```
The build is failing with this error: [paste error]
```

### "I'm not sure about the requirements"
Load `moba-github-workflow`:
```
I'm confused about issue #X, what does this mean?
```

### "Is my work complete?"
Load `moba-testing`:
```
Does my implementation meet these acceptance criteria? [list them]
```

## File Reference

- **MOBA_ROADMAP.md** - High-level features and phases
- **AGENTS.md** - Architecture and project structure
- **GodotGame/main.tscn** - Main test scene
- **.opencode/skills/** - OpenCode skill definitions
- **src/components/** - Component implementations

## Git Basics

### Stage your changes
```bash
git add .
```

### View changes before committing
```bash
git diff --cached
```

### Create a commit
```bash
git commit -m "Your message referencing issue"
```

### View your work
```bash
git log --oneline
```

## Project Structure

```
moba-game/
├── src/
│   ├── components/           # Game components
│   │   ├── health/
│   │   ├── combat/
│   │   ├── movement/
│   │   └── unit_component.hpp
│   ├── core/                 # Core systems
│   ├── input/                # Input handling
│   ├── camera/               # Camera systems
│   └── register_types.cpp    # GDExtension registration
├── GodotGame/
│   ├── main.tscn            # Main test scene
│   ├── lib/                 # Built libraries
│   └── assets/
├── .opencode/skills/        # OpenCode skills
├── AGENTS.md                # Architecture guide
├── MOBA_ROADMAP.md          # Feature roadmap
└── OPENCODE_WORKFLOW.md     # This file
```

## Tips for Success

1. **Load the right skill** - Match your task to the right skill
2. **Follow existing patterns** - Copy from HealthComponent, AttackComponent
3. **Build early** - Don't wait for compilation feedback
4. **Test in Godot** - Verify components work in the engine
5. **Reference issues** - Connect commits to tracking system
6. **Commit frequently** - Small, atomic commits are better
7. **Read acceptance criteria** - Start with requirements, not assumptions

## Next Session

When you start the next session:
1. Run `opencode`
2. Ask "What tasks are ready from moba-game?"
3. Pick one and load the appropriate skill
4. Go! 🚀

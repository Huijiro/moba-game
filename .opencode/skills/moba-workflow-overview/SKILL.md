---
name: moba-workflow-overview
description: High-level overview of the MOBA development workflow and how to use all skills together
license: MIT
compatibility: opencode
metadata:
  audience: developers
  workflow: overview
  project: moba-game
---

## What I Do
- Provide a complete map of how to use OpenCode effectively on moba-game
- Connect GitHub Projects, skills, and development process
- Show you the best workflow for any development task
- Help you understand when to use which skill

## The Complete Workflow

```
Session Start
    ↓
[Load GitHub Issue] ← Use: moba-github-workflow skill
    ↓
[Understand Requirements]
    ↓
[Pick Approach] ← Depends on type:
    ├─→ New Component? ← Use: moba-component-development skill
    ├─→ Need to Build/Test? ← Use: moba-testing skill
    └─→ Standard Task? ← Use: moba-github-workflow skill
    ↓
[Implement with OpenCode]
    ↓
[Build & Test] ← Use: moba-testing skill
    ↓
[Verify Acceptance Criteria]
    ↓
[Create PR with Issue Reference] ← Use: moba-github-workflow skill
    ↓
[Merge → Issue Closes]
    ↓
Session Complete
```

## Skills Quick Reference

### moba-github-workflow
**Use when:**
- Starting a coding session
- Need to pick what to work on
- Want to understand issue requirements
- Ready to create a PR

**Key actions:**
- Show me ready tasks from moba-game
- I'm working on issue #X
- Create PR that references issue

### moba-component-development
**Use when:**
- Creating a new UnitComponent
- Implementing component methods
- Setting up Godot bindings
- Integrating with Unit class

**Key actions:**
- Create HealthComponent pattern
- Implement attack_component
- Add signal binding
- Connect to owner Unit

### moba-testing
**Use when:**
- After making code changes
- Need to build the project
- Verifying acceptance criteria
- Debugging build/runtime errors

**Key actions:**
- Build with ninja
- Run Godot test scene
- Check component in Inspector
- Verify no regressions

## Example Session Flows

### Scenario 1: Implementing a New Feature
```
1. You: "What are the ready tasks from moba-game?"
   [Uses: moba-github-workflow]
   → OpenCode lists issues from GitHub Projects

2. You: "Work on #3: Create AttackComponent"
   [Uses: moba-github-workflow + moba-component-development]
   → OpenCode loads issue details
   → Guides you through component structure
   → Explains binding pattern

3. You: "I've finished the component"
   [Uses: moba-testing]
   → OpenCode builds the code
   → Verifies Godot integration
   → Checks acceptance criteria

4. You: "Create a PR for this"
   [Uses: moba-github-workflow]
   → OpenCode creates commits with `Fixes #3`
   → Writes PR description
   → References the issue
```

### Scenario 2: Debugging an Issue
```
1. You: "The build is failing with linker errors"
   [Uses: moba-testing]
   → OpenCode diagnoses the issue
   → Checks CMakeLists.txt
   → Fixes missing source files

2. You: "Now test it in Godot"
   [Uses: moba-testing]
   → OpenCode verifies build succeeds
   → Suggests manual testing steps
   → Confirms no regressions
```

### Scenario 3: Multi-Part Feature
```
Feature: #2 Combat System Foundation
Subtasks: #3, #4, #5

Session 1:
  Work on #3 [moba-component-development]
  Build & test [moba-testing]
  Create PR [moba-github-workflow]

Session 2:
  Work on #4 [moba-component-development]
  Build & test [moba-testing]
  Create PR [moba-github-workflow]

Session 3:
  Work on #5 [moba-component-development]
  Build & test [moba-testing]
  Create PR [moba-github-workflow]
  
Feature #2 is now complete!
```

## Key Principles

### 1. GitHub Projects as Source of Truth
- All work tracked in GitHub Projects board
- Issues have clear acceptance criteria
- Board shows overall progress

### 2. Small Focused Chunks
- Tasks should be 2-4 hours
- One feature per PR
- Clear scope prevents burnout

### 3. Frequent Validation
- Build after every meaningful change
- Test against acceptance criteria
- Commit early and often

### 4. Clear Communication
- PR title references issue
- Commit messages explain why
- Acceptance criteria tied to tests

## How Each Skill Connects

```
When You Ask...          → This Skill Helps          → Output

"What should I work on?"
                         → moba-github-workflow     → Issue details & requirements

"How do I implement this?"
                         → moba-component-development
                         ┌─ code structure
                         ├─ binding pattern
                         └─ integration points

"Did I get it right?"
                         → moba-testing             ┌─ build succeeded
                         ├─ acceptance met
                         ├─ no regressions
                         └─ ready to PR

"How do I close this out?"
                         → moba-github-workflow     → PR with Fixes #N
```

## Pro Tips

### For New Features
1. Read the issue carefully (especially acceptance criteria)
2. Ask clarifying questions before coding
3. Break into small commits
4. Test each piece as you go
5. Reference issue in commits

### For Complex Components
1. Start with the interface (public methods)
2. Implement binding first (verifies signature)
3. Add implementation logic
4. Add debug logging
5. Test in Godot scene

### For Debugging
1. Always build first - catch compile errors early
2. Check Godot console output for errors
3. Verify node hierarchy in scene
4. Use debug logging to trace execution
5. Test in isolation before integration

### For Committing
1. Commit frequently (not at the end)
2. Keep commits atomic (one logical change)
3. Reference issues: `Fixes #number`
4. Write why, not what
5. Example: `"Implement cooldown tracking - Fixes #4"`

## Files to Know

- **MOBA_ROADMAP.md**: High-level feature roadmap
- **AGENTS.md**: Architecture and project structure
- **GodotGame/main.tscn**: Main test scene
- **src/components/**: Where components live
- **.opencode/skills/**: This skill directory

## Getting Help

- See **AGENTS.md** for architecture details
- Check component headers for Godot binding patterns
- Review existing components for examples
- Use Godot's official documentation for engine features
- Ask OpenCode with skill context: Load a skill then ask questions

## Next Steps

1. **Load the right skill** for your current task
2. **Follow the patterns** in existing code
3. **Build and test** early
4. **Reference issues** in your work
5. **Keep GitHub Projects updated** with progress

You're ready to build awesome MOBA features! 🚀

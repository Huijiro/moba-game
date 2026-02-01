---
name: moba-github-workflow
description: Workflow for tracking MOBA development through GitHub Projects and issues
license: MIT
compatibility: opencode
metadata:
  audience: developers
  workflow: github
  project: moba-game
---

## What I Do
- Link your OpenCode work to GitHub issues and the moba-game Project board
- Create PRs that automatically reference and close GitHub issues
- Guide task planning using GitHub Projects as source of truth
- Help organize work into 2-4 hour focused chunks

## When to Use Me

### Start of Session
Use this when beginning work on a MOBA feature:
1. **Find available tasks**: "Show me ready tasks from moba-game GitHub Projects"
2. **Load issue context**: "I'm working on issue #3 - Create AttackComponent class"
3. I'll help you understand acceptance criteria and implementation approach

### During Development
Use this when you need to:
- Break down large features into subtasks
- Reference related issues or dependencies
- Create meaningful commit messages that link to issues

### When Wrapping Up
Use this when your work is complete:
- Create PR with title that references the issue
- PR body should include: `Fixes #<issue-number>`
- This auto-closes the issue when merged

## The GitHub Projects Board Structure

**Columns:**
- **Backlog**: New features/ideas to be refined
- **Ready**: Issues ready to work on (2-4 hour scope, clear acceptance criteria)
- **In Progress**: Currently being developed
- **In Review**: PR open, awaiting merge
- **Done**: Completed and merged

**Issue Types:**
- **FEATURE**: Larger capability (breaks into 3+ subtasks)
- **TASK**: 2-4 hour chunk (ready to implement)

## How This Connects to Your Work

### When You Pick an Issue
```
You: "I want to work on #4: Implement attack cooldown system"

Me: I'll:
1. Load the issue details
2. Review acceptance criteria
3. Help you plan implementation steps
4. Use the todo list to track progress
```

### When You're Ready to Commit
```
You: "I'm done with the cooldown system"

Me: I'll:
1. Create commits with descriptive messages
2. Reference the issue: "Implement cooldown tracking - Fixes #4"
3. Help structure a clear PR when needed
```

### If an Issue Has Subtasks
Example: Issue #2 (Combat System Foundation) has subtasks #3, #4, #5
```
You: "Work on #2"

Me: I'll recognize it's a feature and suggest:
1. Complete subtask #3 first
2. Then #4, then #5
3. Each should be a separate focused session
4. Each commit references its subtask number
```

## Key Patterns

### Picking Your Next Task
```bash
# Tell me this at session start
"What should I work on from moba-game?"

# Or pick directly
"I'm working on #5: Add damage calculation"
```

### Structuring Your Commit Messages
```
# Good commit message format:
"Implement attack cooldown tracking

- Use delta time for cooldown tracking
- Add is_attack_ready() method
- Expose cooldown_duration property

Fixes #4"
```

### PR Flow
1. Complete all acceptance criteria from the issue
2. Create PR with reference to issue number
3. Include what was changed and why
4. Merge closes the issue automatically

## Tips for Effective Workflow

1. **Always start with the issue**: Read acceptance criteria first
2. **Break it down**: If it feels large, ask if there are related subtasks
3. **Reference in commits**: Use `Fixes #number` so GitHub auto-closes
4. **Update project board**: After merging, verify issue moved to Done
5. **Commit early and often**: Don't wait for perfection - iterate with feedback

## Architecture Notes for MOBA

The MOBA components follow a pattern:
- **UnitComponent** base class in `src/components/`
- Components extend UnitComponent and own their logic
- Register with Godot via `_bind_methods()`
- Component receives owner Unit reference for context

See **AGENTS.md** for detailed architecture documentation.

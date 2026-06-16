---
name: auto-push-after-changes
description: Commit and push to GitHub after every code/file change in the COMP9319 repo
metadata:
  node_type: memory
  type: feedback
---

In the COMP9319 repo (and the user's similar personal repos), after every change to
code or files, commit and push to GitHub automatically without being asked.

**Why:** the user works almost entirely through Claude Code and relies on Git history
to trace and debug changes; visible commits let them roll back and find where a bug
was introduced.

**How to apply:** one focused commit per logical change with a clear message; push to
`origin/main` right after committing; this is standing authorization to push for this
repo, so don't ask each time. Group co-changing files into one commit; split logically
distinct changes. See [[comp9319-repo]] and [[sync-memory-to-repo]].

---
name: sync-memory-to-repo
description: Mirror Claude Code memory files into the private course repo for cross-platform continuity
metadata:
  node_type: memory
  type: feedback
---

For the COMP9319 private repo (and similar personal private repos), keep a copy of the Claude Code memory files inside the repo so the user can carry CC's prior memory across platforms.

- Mirror the whole memory dir (`MEMORY.md` + every memory `.md`) into `claude-memory/` at the repo root.
- Whenever a memory file is created/updated/deleted under `~/.claude/projects/<key>/memory/`, make the same change in the repo's `claude-memory/` and commit + push.
- On a fresh clone / new machine, the canonical location is `~/.claude/projects/<project-key>/memory/`; the project key is derived from the absolute project path, so it differs per machine — copy `claude-memory/*` into the correct per-machine path.

**Why:** the user uses the repo as a cross-platform store and wants CC to retain previous memory everywhere.

See [[comp9319-repo]] and [[solution-file-convention]].

# Project: COMP9319 coursework

This repository is the user's cross-platform store for all COMP9319 (UNSW) work.

## Always commit + push after any code/file change

The user works almost entirely through Claude Code and relies on Git history to
trace and debug changes. So **after every change to code or files in this repo,
commit and push to GitHub automatically** — do not wait to be asked:

- Make one focused commit per logical change, with a clear message describing
  what changed and why (so the history is useful for rollback/debugging).
- Push to `origin/main` right after committing.
- This is standing authorization to push for this repo; no need to ask each time.
- If several files change together for one task, group them into one commit;
  use separate commits when the changes are logically distinct.

## At the start of every session

**Read all files in `claude-memory/` first** to restore Claude Code's prior memory
for this project. Start with `claude-memory/MEMORY.md` (the index), then read the
individual memory files it points to. Treat their contents as established context
for this project before doing other work.

## Keeping memory in sync

`claude-memory/` is a mirror of CC's per-project memory
(`~/.claude/projects/<project-key>/memory/`). Whenever a memory file is
created/updated/deleted, make the same change in `claude-memory/` and commit + push,
so memory stays consistent across machines.

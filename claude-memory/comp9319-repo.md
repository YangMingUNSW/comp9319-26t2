---
name: comp9319-repo
description: The COMP9319 coursework GitHub repo used as a cross-platform store
metadata:
  node_type: memory
  type: project
---

The PRIVATE GitHub repo `YangMingUNSW/comp9319` (default branch `main`) is the user's cross-platform store for all COMP9319 (Web Data Compression and Search, UNSW) assignments and projects. Set up the same way as the COMP9312 repo.

- Each assignment lives in its own top-level folder (`assignment1/`, future `assignment2/`, `project/`, …).
- `.claude/` and LaTeX build artifacts are gitignored.
- git user is YangMingUNSW / larryrowau@gmail.com.
- On a fresh clone, copy `claude-memory/*` into the per-machine CC memory dir (`~/.claude/projects/<project-key>/memory/`); the project key differs per absolute path.

See [[solution-file-convention]] for the per-assignment two-file output rule.

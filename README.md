# COMP9319

Coursework for **COMP9319 — Web Data Compression and Search** (UNSW).
A personal cross-platform workspace for everything in this course — assignments,
projects, or any other written deliverable.

This repo follows a reusable template (same framework as the COMP9312 repo): the
Claude-Code memory handling, the Markdown solution files, and the LaTeX/Overleaf
`pdf.md` files are all set up the same way so the workflow carries over to any new
course unchanged.

## Structure

Each deliverable lives in its own top-level folder, named for what it actually is —
e.g. `assignment1/`, `project1/`, `project/` — added as the course progresses.
(There is no assignment in here yet; add folders as the course gives them out.)

## `claude-memory/`

A mirror of Claude Code's persistent memory for this project, kept in the repo so
CC's prior context follows you across machines. On a new machine, after cloning,
copy these files into Claude Code's per-project memory directory
(`~/.claude/projects/<project-key>/memory/`, where the key is derived from the
project's absolute path on that machine).

## File naming convention

For every deliverable (assignment **or** project), each written-up solution comes
in two forms:

- `*_solution.md` — readable Markdown solution.
- `*_solution_pdf.md` — the same content as **LaTeX source**; copy the whole file
  into Overleaf and compile to get the PDF for submission.

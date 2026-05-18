# CLAUDE.md

This file provides guidance and important rules working with code in this repository.

## Project at a glance

CS2_External is an educational **external** cheat / overlay for Counter-Strike 2.
The program runs as its own process, attaches to `cs2.exe` via Win32 `OpenProcess` /
`ReadProcessMemory` / `WriteProcessMemory`, and renders an ImGui overlay on top of
the game (no DLL injection). Offsets come from the in-tree `cs2-dumper` submodule
plus runtime byte-signature scanning in `CS2_External/Offsets.h`.

For details, prefer the Serena memories — see below.

## When coding / building plan

 - Use a progressive disclosure approach for agent coding in this repository: start from high-level information in Serena memories, and only locate/read specific files or symbols when necessary to avoid expanding too much context at once.
 - Per the global CLAUDE.md: **do NOT run `msbuild`, `cargo`, or the cheat itself proactively.** Only build / run when the user explicitly asks.
 - Do NOT silently update the `cs2-dumper` submodule — that touches every offset consumer in `Offsets.h`.

#### Serena memories (Keep context clean)

- Prefer Serena MCP tools to understand the architecture and code hierarchy quickly.
- **ALWAYS** call Serena's `activate_project` before reading memories.
- Available memories in `.serena/memories/`:
  - `project_overview` — purpose, tech stack, layout, runtime flow, key globals (`gGame`, `ProcessMgr`, `Gui`).
  - `style_and_conventions` — C++ naming (`CGame`, `gGame`, engine field names verbatim), tabs + Allman braces, `.h` vs `.hpp` split, `StatusCode::FAILE_*` typo convention.
  - `suggested_commands` — MSBuild invocation, `cs2-dumper` submodule commands, Windows path notes for the Serena MCP shell (native `cmd`, not WSL).
  - `completion_checklist` — what "done" means in this repo (no lint/tests; just consistent code + working overlay).

#### When Memories Are Insufficient (On-Demand Querying and Reading)

- Check `README.md` for upstream feature list and project origin.
- For offsets and engine field names: look at `cs2-dumper/output/client_dll.hpp` and `cs2-dumper/output/offsets.hpp`.
- For runtime signatures: see `CS2_External/Offsets.h` (`Offset::Signatures::*`) and `CS2_External/Utils/MemorySearch.cpp`.
- For remote memory access patterns: see `CS2_External/Utils/ProcessManager.hpp` and how `ProcessMgr` is used throughout.
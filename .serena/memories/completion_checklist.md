# Task Completion Checklist — CS2_External

There is no automated test/lint/format setup in this repo. "Done" means the code is consistent,
builds, and (when relevant) actually works against cs2.exe.

## Before reporting a coding task as complete

1. **Code reads cleanly**
   - Tabs for indentation, Allman braces (match surrounding code).
   - Names match the conventions in [[style_and_conventions]] (`PascalCase` for types/functions,
     `gXxx`/short PascalCase for globals, engine field names verbatim from cs2-dumper).
   - No leftover backward-compat shims for removed engine fields (compare with recent commits like
     `a45004b Remove GetAimPunchCache`).

2. **Offsets and signatures stay consistent**
   - Anything in `CS2_External/Offsets.h` that references `cs2_dumper::schemas::*` must exist in the
     current `cs2-dumper/output/client_dll.hpp` / `offsets.hpp`. If a field was removed by the dumper,
     update or remove the consumer.
   - Runtime signature strings under `Offset::Signatures` follow the IDA-style pattern
     `"48 8B 0D ?? ?? ?? ?? ..."`.

3. **Process / memory safety**
   - All remote memory access goes through `ProcessMgr` (don't introduce raw `ReadProcessMemory` calls
     without it unless there is a clear reason).
   - Check return codes from `Attach()` (compare with `StatusCode::SUCCEED`).

4. **Build (only when the user explicitly asks)**
   - Per global CLAUDE.md: **do NOT run builds proactively.** Only build if the user requests it.
   - If asked: `msbuild CS2_External.sln /p:Configuration=Release /p:Platform=x64` from a VS x64
     Native Tools prompt, or build inside Visual Studio.
   - First-time setup: `git submodule update --init --recursive` so `cs2-dumper/output/*.hpp` exists.

5. **Manual smoke (only when the user explicitly asks)**
   - Launch `cs2.exe`, then run `x64\Release\CS2_External.exe` as administrator.
   - Confirm it prints PID, client.dll base, and the Offset:* lines without `[ERROR]`.
   - Confirm overlay attaches to the "Counter-Strike 2" window (class `SDL_app`).
   - Spot-check the touched feature in-game.

6. **No spurious files**
   - Don't add `compile_commands.json`, CMake files, formatters, or CI configs unless explicitly asked.
   - Don't commit build outputs (`x64/`, `*.vcxproj.user`) — they are gitignored.
   - Don't commit `imgui.ini` or contents under `Documents/CS2_External/`.

## What NOT to do at completion time
- Do **not** run `msbuild`, `cargo`, or the cheat itself proactively (per CLAUDE.md).
- Do **not** "modernize" headers (.h ↔ .hpp), reformat unrelated files, or rename existing types just
  for style.
- Do **not** introduce mocks, abstractions, or feature flags for hypothetical future requirements.
- Do **not** silently update the `cs2-dumper` submodule — that affects all offsets.

# Code Style & Conventions — CS2_External

This is a small, pragmatic C++ codebase. There is no clang-format or style guide file in tree;
conventions are inferred from existing code.

## Language / toolchain
- C++ with MSVC v143 (Visual Studio 2022). Default C++ language standard from VS (typically C++14/17).
- Targets x64 (and Win32 as a secondary platform).
- Heavy use of Win32 API (`<Windows.h>`, `Tlhelp32.h`, `ShlObj.h`, etc.).
- Uses `DWORD`, `DWORD64`, `HANDLE` consistently for Windows-side and remote-pointer types.

## File layout
- Headers: `.h` for class declarations with companion `.cpp`. `.hpp` for header-only modules (feature
  modules like `AimBot.hpp`, `Bunnyhop.hpp`, `AntiFlashbang.hpp`, `Render.hpp`, `Globals.hpp`,
  `MenuConfig.hpp`, `View.hpp`, util helpers `Format.hpp`, `ProcessManager.hpp`).
- One class per file typically. Several small structs may share a header (e.g. `Entity.h` has
  `PlayerController`, `PlayerPawn`, `CEntity`, `C_UTL_VECTOR`).
- All `.h` files use `#pragma once`.

## Naming
- Classes: `PascalCase` with optional `C` prefix (Hungarian-ish): `CGame`, `CView`, `CEntity`,
  `ProcessManager`, `PlayerPawn`, `PlayerController`.
- Globals: short PascalCase / lowercase-g prefix:
  - `gGame` (global `CGame`)
  - `Gui` (from OS-ImGui)
  - `ProcessMgr` (global `ProcessManager`)
- Free functions / methods: `PascalCase`: `InitAddress`, `UpdateOffsets`, `SetViewAngle`, `Format`,
  `GetClientDLLAddress`.
- Namespaces: lowercase or short PascalCase (`Cheats`, `Offset`, `Offset::Signatures`, `MenuConfig`).
- Constants for engine fields use the upstream Source-engine names verbatim (`m_iHealth`, `m_hPlayerPawn`,
  `m_aimPunchAngle`, `m_bSpotted`, etc.) for traceability with `cs2-dumper` output.
- Enums: `SCREAMING_SNAKE_CASE` values (e.g. `StatusCode::SUCCEED`, `FAILE_PROCESSID`). Note the
  upstream typo "FAILE" (instead of FAIL) — keep it to avoid breaking refs unless told otherwise.

## Formatting
- Indentation: **tabs** in the C++ source (visible in `main.cpp`, `Offsets.h`).
- Braces: Allman style (opening brace on its own line for functions, classes, namespaces, blocks).
- Pointer/reference spacing: `Type* var` / `Type& var`.
- Mixed-language comments: some Chinese comments left over from upstream (e.g. `// 进程管理` in
  `ProcessManager.hpp`). Keep existing language unless asked.

## Idioms / patterns
- Compile-time offsets pulled from `cs2_dumper::offsets::client_dll::*` and
  `cs2_dumper::schemas::client_dll::*` (cs2-dumper-generated headers in `cs2-dumper/output/`).
- Runtime offsets resolved via byte signatures (`Offset::Signatures::*` strings) and
  `MemorySearch` (see `Utils/MemorySearch.cpp`). These are kept in `Offsets.h` as `const std::string`.
- Remote memory I/O goes through the global `ProcessMgr` (see `ProcessManager.hpp`).
- `inline` globals at file scope are used (e.g. `inline CGame gGame;`, `inline DWORD ForceJump;`).
- Status checks return enum codes (`StatusCode`) rather than throwing.
- `goto END;` cleanup pattern in `main.cpp` (kept upstream style).
- No exceptions except where OS-ImGui throws `OSImGui::OSException` — caught in `main`.

## Comments / docs
- Sparse, mostly Chinese/English mix. Some doc comments use C# / VS-style triple-slash with
  `<summary>` / `<param>` tags (see `ProcessManager.hpp`). Match that style if you extend an existing
  file that already uses it; otherwise prefer minimal comments and only add a comment when the *why*
  is non-obvious (see CLAUDE.md general guidance).
- Per-interface "Purpose:" doc comments are NOT a convention here — that's for other projects with
  the `add-cpp-interface-purpose-comments` skill.

## Includes
- Win32 first: `<Windows.h>` typically, then C++ stdlib, then project headers.
- Project headers in include-form: `"Offsets.h"`, `"Cheats.h"`, `"Utils/Format.hpp"`.
- cs2-dumper headers as `<client_dll.hpp>` and `<offsets.hpp>` (resolved through the project's
  AdditionalIncludeDirectories `$(ProjectDir)..\cs2-dumper\output`).

## Backward compatibility / removed fields
- Some legacy fields are kept set to `0` with a comment when CS2 removes them, e.g.:
  `DWORD m_aimPunchCache = 0;//Not a thing since Game update(14132)`
  Recent commit `a45004b` ("Remove GetAimPunchCache") shows that fully removing such code is
  acceptable. Prefer deletion when confident a field is gone (see CLAUDE.md guidance about not keeping
  dead backwards-compat shims).

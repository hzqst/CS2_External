# CS2_External — Project Overview

## Purpose
CS2_External is an educational **external** cheat / game-overlay for Counter-Strike 2 (CS2).
Upstream: https://github.com/TKazer/CS2_External (the "TKazer" CS2 external base; a derivative of the SilentAimBot research).
It is purely educational; offsets are no longer updated upstream and must be refreshed by the developer.

External means: the program runs as its own process, attaches to `cs2.exe` via Win32 `OpenProcess` /
`ReadProcessMemory` / `WriteProcessMemory`, and renders an overlay window using ImGui on top of the game.
No code is injected into cs2.exe.

## Features (from README)
- BoneESP, BoxESP, WeaponESP, EyeLine, HeadShoot Line, Fov Line
- AimBot (with RCS)
- TriggerBot
- Radar
- Visibility Check
- OBS Bypass (overlay technique used by `OS-ImGui`)
- Bhop (Bunnyhop)
- Auto update offsets (signatures + cs2-dumper output)
- Glow color support (recent commits)
- Spotted state (recent commits)

## Tech Stack
- Language: C++17/20, MSVC (PlatformToolset v143, Visual Studio 2022). x64 / Win32.
- Build system: MSBuild via `CS2_External.sln` (no CMake).
- Rendering / UI: ImGui via the in-tree submodule `OS-ImGui` (DirectX-based overlay window).
- Windows APIs: `kernel32` (OpenProcess, RPM/WPM), `Tlhelp32`, `Shell32` (KnownFolders), `User32`.
- Offsets source: the **cs2-dumper** git submodule (`a2x/cs2-dumper`, Rust project that dumps CS2 offsets/schema headers).
  - Compile-time offsets come from `cs2-dumper/output/client_dll.hpp`, `offsets.hpp`, etc.
  - These headers are pulled in via the project's `AdditionalIncludeDirectories=$(ProjectDir)..\cs2-dumper\output`.
- Signature scanning: pattern strings in `CS2_External/Offsets.h` resolved at runtime against cs2.exe memory.

## Project layout (top level)
- `CS2_External.sln` — VS solution.
- `CS2_External/` — main C++ project (the cheat).
- `cs2-dumper/` — git submodule, Rust tool + generated `output/*.hpp` headers consumed at build time.
- `x64/` — MSBuild output (gitignored).
- `.vs/`, `.git/`, `.serena/` — IDE / VCS / Serena metadata.
- `README.md`, `LICENSE.txt`, `Image2.png`.

## Main C++ project layout (`CS2_External/CS2_External/`)
- `main.cpp` — entry point: attaches to `cs2.exe`, resolves offsets, opens overlay, calls `Cheats::Run`.
- `Offsets.h/.cpp` — central offsets table; mixes `cs2_dumper` compile-time constants with runtime
  signature-resolved offsets (`UpdateOffsets()`).
- `Game.h/.cpp` — `CGame` (singleton `gGame`): caches resolved addresses (EntityList, Matrix, ViewAngle,
  LocalController, LocalPawn, ForceJump, GlobalVars) and exposes `InitAddress`, `UpdateEntityListEntry`,
  `SetViewAngle`, `SetForceJump`, plus a `CView` member.
- `Entity.h/.cpp` — `CEntity`, `PlayerController`, `PlayerPawn`, `C_UTL_VECTOR` wrappers around remote
  process memory.
- `View.hpp` — view matrix / world-to-screen.
- `GlobalVars.h/.cpp`, `Globals.hpp` — engine `CGlobalVarsBase`-style data and shared globals/state.
- Feature modules:
  - `AimBot.hpp`
  - `TriggerBot.h/.cpp`
  - `Bunnyhop.hpp`
  - `AntiFlashbang.hpp`
  - `Bone.h/.cpp`
  - `Render.hpp`
  - `Radar/` (folder)
  - `Cheats.h/.cpp` — wires features together; exposes `Cheats::Run`, `Cheats::Menu`,
    `Cheats::RadarSetting`.
- `MenuConfig.hpp` — ImGui menu state / config path (`Documents/CS2_External`).
- `Utils/`
  - `ProcessManager.hpp` — `ProcessManager` (process attach, module base, RPM/WPM helpers); macro
    `_is_invalid`; enum `StatusCode { SUCCEED, FAILE_PROCESSID, FAILE_HPROCESS, FAILE_MODULE }`.
  - `MemorySearch.cpp` — pattern/signature scanner used for the `Signatures::*` strings in `Offsets.h`.
  - `ConfigMenu.{cpp,hpp}`, `ConfigSaver.{cpp,hpp}` — ImGui-driven settings UI and persistence.
  - `Format.hpp` — `Format(...)` printf-style helper used throughout.
- `OS-ImGui/` — vendored ImGui-based overlay library; `Gui.AttachAnotherWindow("Counter-Strike 2", "SDL_app", Cheats::Run)` is the overlay entry point.

## Runtime flow (main.cpp)
1. `ProcessMgr.Attach("cs2.exe")` — finds PID, opens handle, gets `client.dll` base.
2. Resolves `Documents/CS2_External` config folder via `SHGetFolderPathA`.
3. `Offset::UpdateOffsets()` — resolves runtime signature offsets (ForceJump etc.) and validates dumper offsets.
4. `gGame.InitAddress()` — caches absolute addresses inside cs2.exe.
5. Prints diagnostics (PID, client.dll base, key offsets).
6. `Gui.AttachAnotherWindow("Counter-Strike 2", "SDL_app", Cheats::Run)` — attaches an OS-ImGui overlay
   to the CS2 window and enters the cheat main loop via `Cheats::Run`.

## Notable upstream details
- Originating SilentAimBot writeup (CN): https://bbs.kanxue.com/thread-282616.htm
- Derivative projects mentioned in README: AimStar, Aeonix.
- Offsets are intentionally not maintained upstream; users update by re-running cs2-dumper.

## Important global / convenience instances
- `ProcessMgr` — global `ProcessManager` (from `Utils/ProcessManager.hpp`).
- `gGame` — global `CGame`.
- `Gui` — global from `OS-ImGui`.
- `MenuConfig::path` — base path for saved configs / data.

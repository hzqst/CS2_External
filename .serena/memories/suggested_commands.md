# Suggested Commands — CS2_External

Host OS: **Windows 11**. The shell Claude uses is Git Bash on Windows
(Unix-style commands like `ls`, `grep`, forward-slash paths). The Serena MCP shell, however, runs in
**native Windows cmd** for this project (note: `'ls' is not recognized` happens there — use Windows
commands or run through bash explicitly via `bash -c "..."`).

## Build (MSBuild / Visual Studio 2022, v143 toolset)
Solution: `CS2_External.sln`. Configs: `Debug|x64`, `Release|x64` (also Win32 variants, but x64 is primary).
Include dir for cs2-dumper headers is set in vcxproj to `$(ProjectDir)..\cs2-dumper\output`.

```cmd
:: From a "x64 Native Tools Command Prompt for VS 2022":
msbuild CS2_External.sln /m /p:Configuration=Release /p:Platform=x64
msbuild CS2_External.sln /m /p:Configuration=Debug   /p:Platform=x64
```

Or from PowerShell using `dotnet`-style `msbuild` via VS Developer PowerShell, or simply build inside
Visual Studio (open `CS2_External.sln`, F7).

Output binaries land in `x64\Release\` / `x64\Debug\` (and `CS2_External\x64\...`). These dirs are gitignored.

**Per CLAUDE.md: do NOT run builds proactively. Only build when the user explicitly asks.**

## Running
The program must be launched **after** `cs2.exe` is already running, ideally as **administrator**
(needs `PROCESS_ALL_ACCESS` on cs2.exe).

```cmd
x64\Release\CS2_External.exe
```

It will:
- Attach to `cs2.exe`
- Create / use `%USERPROFILE%\Documents\CS2_External\` for config
- Open an overlay window on top of the CS2 window (`SDL_app` window class)

## Offsets / cs2-dumper submodule
`cs2-dumper` is a git submodule. After cloning, initialize/update it:

```cmd
git submodule update --init --recursive
```

To regenerate offset headers (Rust toolchain required, run **with cs2 running**, **as admin**):

```cmd
cd cs2-dumper
cargo run --release
:: Output goes to cs2-dumper/output/*.{hpp,rs,cs,json,zig}
```

The C++ project consumes `cs2-dumper/output/client_dll.hpp` and `offsets.hpp` directly.

## Git
Standard Windows git. Submodules matter here.

```bash
git status
git submodule status
git submodule update --init --recursive
git submodule update --remote cs2-dumper      # pull latest offsets
git log --oneline -20
```

## File / search utilities (Git Bash on Windows — what Claude itself uses)
- List dir: `ls`, `ls -la` (use forward slashes in paths)
- Find files: prefer the Glob tool. Otherwise: `find . -name "*.hpp"`
- Search code: prefer the Grep tool. Otherwise: `grep -rn "pattern" CS2_External/`
- Edit/read files: use the `Edit` / `Read` tools, not `sed` / `cat`.

## Lint / format / test
There is **no** lint, formatter, or automated test setup in this repo. No CMake, no CI, no
`compile_commands.json`. "Completion" of a task is just: code compiles in MSBuild, runs against
cs2.exe, and feature works in the overlay.

## Things that are NOT applicable
- No Python tooling, no npm, no pip, no pytest.
- No clang-format / clang-tidy config files in tree.
- No coverage tooling.

## Path note for Serena MCP shell
Serena MCP's `execute_shell_command` here runs in **native Windows cmd**, despite CLAUDE.md's general
note about WSL paths. Use `D:\CS2_External\...` style paths in Serena shell commands, not
`/mnt/d/CS2_External/...`. Claude's own Bash tool also runs on native Windows (Git Bash).

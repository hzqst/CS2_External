---
name: bump-cs2-offsets
description: Bump CS2 offsets to the latest game build. Force-syncs the in-tree cs2-dumper submodule to upstream latest, then refreshes hard-coded offsets in CS2_External/Offsets.h that carry a `// CS2_VibeSignatures: client.dll -> <artifact>` marker against the newest CS2_VibeSignatures snapshot. Use when asked to "bump offsets", "update offsets for the new CS2 patch", or after a game update breaks the overlay.
---

# Bump CS2 Offsets

## Overview
Counter-Strike 2 ships frequent game updates that shift hard-coded struct offsets.
This skill performs two coupled updates so the overlay keeps reading the right
fields after each patch:

1. **Submodule sync** — force `cs2-dumper` (in-tree at `cs2-dumper/`) to the
   upstream default-branch tip. This regenerates the `cs2_dumper::offsets::...`
   and `cs2_dumper::schemas::...` constants consumed by `CS2_External/Offsets.h`.
2. **VibeSignatures patch** — a handful of offsets in `Offsets.h` are *not* in
   cs2-dumper's schema dump (e.g. `CClientInput::m_viewangles`,
   `CSkeletonInstance::m_modelState.m_simulationState`). These are tagged with
   a trailing comment marker:
   ```cpp
   // CS2_VibeSignatures: client.dll -> <artifact>
   ```
   The skill reads the newest snapshot from the public CS2_VibeSignatures index,
   looks up each `<artifact>` record, and rewrites the `0x...` literal on that
   line with the fresh offset.

> Per `CLAUDE.md`: do NOT silently touch the `cs2-dumper` submodule in ordinary
> tasks. This skill is the *explicit* exception — invoking it implies consent to
> bump offsets, which is exactly what touches `Offsets.h`.

## Canonical marker format
The skill only edits lines whose **existing** comment matches this shape:

```
<decl> = 0xDEAD;// CS2_VibeSignatures: client.dll -> <artifact>
<decl> = 0xDEAD; // CS2_VibeSignatures: client.dll -> <artifact>
```

- `client.dll` is the human-readable module name; the snapshot is keyed by the
  short module id `client`.
- `<artifact>` mirrors the snapshot record's `artifact` field verbatim — e.g.
  `CClientInput_m_viewangles`, `CSkeletonInstance_m_modelState_m_simulationState`.
- The skill never invents new markers; it only refreshes the hex literal in
  front of markers that already exist in `Offsets.h`.

## Snapshot data model (for reference)
- Index JSON: `https://hlnd2t.github.io/CS2_VibeSignatures/gamesymbols/index.json`
  - `versions[]` is a list of snapshots; each has `gameVersion`, `url` (relative
    filename), `sha256`, `size`, and `lastPublishTime` (ISO-8601 UTC).
  - The newest snapshot is the entry with the **largest `lastPublishTime`**.
    Do not sort by `gameVersion` — it can carry letter suffixes (e.g. `14168b`).
- Snapshot JSON: `https://hlnd2t.github.io/CS2_VibeSignatures/gamesymbols/<url>`
  - `records[]`: each record has `module`, `artifact`, `symbolName`, `platform`,
    `kind`, and `payload`. For offsets we only care about records where
    `platform == "windows"` and `kind == "structMember"`; the offset is
    `payload.offset` (a hex string like `"0x688"`). The `artifact` field is the
    key that matches the marker suffix in `Offsets.h`.

## Workflow

### 0. Preconditions
- Run from the repository root (`D:\CS2_External`).
- Ensure the working tree for `cs2-dumper/` has no uncommitted local edits — the
  skill force-resets it, which would discard them. If unsure, run
  `git -C cs2-dumper status` first.
- Activate Serena and skim the `project_overview` memory if you need a refresher
  on how `Offsets.h` feeds `gGame`/`ProcessMgr`.

### 1. Run the bundled script (preferred)
The script does all three steps — submodule sync, snapshot fetch, and
`Offsets.h` patch — in one go:

```powershell
# Preview what would change, no writes:
& .claude/skills/bump-cs2-offsets/scripts/bump_cs2_offsets.ps1 -DryRun

# Apply for real:
& .claude/skills/bump-cs2-offsets/scripts/bump_cs2_offsets.ps1
```

Parameters:
- `-OffsetsFile` (default `CS2_External/Offsets.h`) — target header to patch.
- `-SubmodulePath` (default `cs2-dumper`) — submodule directory to sync.
- `-SkipSubmodule` — skip step 1 (only refresh VibeSignatures markers).
- `-DryRun` — print the planned diff, write nothing.
- `-Force` — run even if `Offsets.h` is already up to date (otherwise the script
  exits 0 silently when no markers need changing after a sync).

### 2. Review the output
The script prints, in order:
- The submodule's old vs. new commit and the snapshot `gameVersion` it pinned.
- Each patched line: `<artifact>: 0xOLD -> 0xNEW`.
- Any marker in `Offsets.h` whose artifact was **not** found in the snapshot —
  these are left untouched and must be handled manually (the upstream symbol may
  have been renamed/removed).

### 3. Manual fallback (if the script is unavailable)
1. **Sync submodule** (from repo root):
   ```powershell
   git -C cs2-dumper fetch origin
   $head = git -C cs2-dumper symbolic-ref refs/remotes/origin/HEAD
   $branch = ($head -replace '^refs/remotes/origin/', 'origin/')
   git -C cs2-dumper reset --hard $branch
   ```
2. **Fetch the newest snapshot URL**:
   ```powershell
   $idx = Invoke-RestMethod 'https://hlnd2t.github.io/CS2_VibeSignatures/gamesymbols/index.json'
   $latest = $idx.versions | Sort-Object lastPublishTime -Descending | Select-Object -First 1
   $snap = Invoke-RestMethod ("https://hlnd2t.github.io/CS2_VibeSignatures/gamesymbols/" + $latest.url)
   ```
3. **Build the artifact -> offset map** and patch the matching lines in
   `Offsets.h` using the canonical marker regex described above. Only replace
   the `0x...` literal on the marker line; uppercase the hex digits to match the
   file's existing style (e.g. `0x1C0`, not `0x1c0`).

## Verification
- After applying: re-read `CS2_External/Offsets.h` and confirm the marker lines
  now carry the snapshot's offset for each artifact.
- Spot-check at least `CClientInput_m_viewangles` and
  `CSkeletonInstance_m_modelState_m_simulationState` against the snapshot JSON.
- Per `CLAUDE.md`: do **not** run `msbuild` or the overlay proactively — wait for
  the user to ask. Just leave the tree patched and report what changed.
- Do **not** commit unless the user explicitly asks.

## Edge cases
- **`origin/HEAD` unset** (fresh submodule clone): `git symbolic-ref` will fail.
  Have the user run `git -C cs2-dumper remote set-head origin -a` once, or pin
  a known branch (`origin/main`) via `-SubmoduleBranch` if you extend the script.
- **Marker not in snapshot**: the symbol was renamed or dropped upstream. Leave
  the line as-is and surface it in the report for a human decision — do not
  guess a replacement offset.
- **`gameVersion` suffix letters**: never numerically sort `gameVersion`. Always
  pick the newest snapshot by `lastPublishTime`.
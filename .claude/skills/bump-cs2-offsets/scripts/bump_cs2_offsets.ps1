<#
.SYNOPSIS
  Bump CS2 offsets: sync cs2-dumper submodule and refresh CS2_VibeSignatures
  markers in CS2_External/Offsets.h against the newest snapshot.

.PARAMETER OffsetsFile
  Header file containing `// CS2_VibeSignatures: client.dll -> <artifact>` markers.
  Defaults to `CS2_External/Offsets.h` (relative to repo root).

.PARAMETER SubmodulePath
  Path to the cs2-dumper submodule directory. Defaults to `cs2-dumper`.

.PARAMETER SubmoduleBranch
  Remote ref to hard-reset the submodule to. If empty, auto-detect via
  `origin/HEAD`. Falls back to `origin/main` if detection fails.

.PARAMETER SkipSubmodule
  Skip the submodule sync step (only refresh VibeSignatures markers).

.PARAMETER DryRun
  Print the planned changes and write nothing.

.PARAMETER Force
  Apply even when the patch step produces no marker changes (default: exit
  silently when nothing would change after a sync).

.EXAMPLE
  .\bump_cs2_offsets.ps1 -DryRun
  .\bump_cs2_offsets.ps1
#>
[CmdletBinding()]
param(
    [string]$OffsetsFile    = 'CS2_External/Offsets.h',
    [string]$SubmodulePath  = 'cs2-dumper',
    [string]$SubmoduleBranch = '',
    [switch]$SkipSubmodule,
    [switch]$DryRun,
    [switch]$Force
)

$ErrorActionPreference = 'Stop'

$IndexUrl      = 'https://hlnd2t.github.io/CS2_VibeSignatures/gamesymbols/index.json'
$SnapshotBase  = 'https://hlnd2t.github.io/CS2_VibeSignatures/gamesymbols/'

# Line marker: <decl> = 0xOLD ; // CS2_VibeSignatures: <module>.dll -> <artifact>
$LineRx = [regex]'^(.*?=\s*)0x[0-9A-Fa-f]+(\s*;\s*//\s*CS2_VibeSignatures:\s*\w+\.dll\s*->\s*)([A-Za-z0-9_]+)(.*)$'

function Resolve-RepoRoot {
    $p = Get-Location
    while ($p -and (Test-Path (Join-Path $p '.git')) -eq $false) {
        $parent = Split-Path $p -Parent
        if ($parent -eq $p) { return $null }
        $p = $parent
    }
    return $p
}

$repoRoot = Resolve-RepoRoot
if (-not $repoRoot) { $repoRoot = (Get-Location).Path }
$resolved = Resolve-Path (Join-Path $repoRoot $OffsetsFile) -ErrorAction SilentlyContinue
$OffsetsAbs = if ($resolved) { $resolved.Path } else { $null }
if (-not $OffsetsAbs) {
    Write-Error "Offsets file not found: $OffsetsFile (resolved root: $repoRoot)"
}

# ---------------------------------------------------------------------------
# Step 1 - Sync cs2-dumper submodule to upstream latest
# ---------------------------------------------------------------------------
if (-not $SkipSubmodule) {
    $subAbs = Join-Path $repoRoot $SubmodulePath
    if (-not (Test-Path (Join-Path $subAbs '.git'))) {
        Write-Error "Submodule not found at $subAbs (is it initialized?). Pass -SkipSubmodule to skip."
    }
    Write-Host "==> [1/3] Syncing submodule $SubmodulePath -> upstream latest" -ForegroundColor Cyan
    $_old = git -C $subAbs rev-parse HEAD 2>$null
    $oldCommit = if ($_old) { $_old.Trim() } else { '' }

    git -C $subAbs fetch origin --tags --quiet
    if (-not $?) { Write-Error "git fetch failed for submodule $SubmodulePath" }

    if ([string]::IsNullOrWhiteSpace($SubmoduleBranch)) {
        $head = $null
        try {
            $_head = git -C $subAbs symbolic-ref refs/remotes/origin/HEAD 2>$null
            $head = if ($_head) { $_head.Trim() } else { '' }
        } catch { }
        if ($head -match '^refs/remotes/origin/(.+)$') {
            $SubmoduleBranch = "origin/$($Matches[1])"
        } else {
            $SubmoduleBranch = 'origin/main'
        }
    }

    if (-not $DryRun) {
        git -C $subAbs reset --hard $SubmoduleBranch | Out-Null
        if (-not $?) { Write-Error "git reset --hard $SubmoduleBranch failed" }
    }
    $_new = git -C $subAbs rev-parse HEAD 2>$null
    $newCommit = if ($_new) { $_new.Trim() } else { '' }
    Write-Host "    submodule: $oldCommit -> $newCommit ($SubmoduleBranch)"
} else {
    Write-Host "==> [1/3] Submodule sync skipped (-SkipSubmodule)" -ForegroundColor DarkGray
}

# ---------------------------------------------------------------------------
# Step 2 - Fetch newest CS2_VibeSignatures snapshot
# ---------------------------------------------------------------------------
Write-Host "==> [2/3] Fetching newest CS2_VibeSignatures snapshot" -ForegroundColor Cyan
$idx = Invoke-RestMethod -Uri $IndexUrl
$latest = $idx.versions | Sort-Object lastPublishTime -Descending | Select-Object -First 1
$snapUrl = $SnapshotBase + $latest.url
Write-Host "    gameVersion: $($latest.gameVersion)  lastPublish: $($latest.lastPublishTime)  sha256: $($latest.sha256)"
if (-not $DryRun) { Write-Host "    snapshot: $snapUrl" }
$snap = Invoke-RestMethod -Uri $snapUrl

# Map artifact -> payload.offset for windows/client structMember records.
$offsetMap = @{}
foreach ($r in $snap.records) {
    if ($r.platform -eq 'windows' -and $r.kind -eq 'structMember' -and $r.module -eq 'client') {
        if ($r.payload.offset) {
            $offsetMap[$r.artifact] = $r.payload.offset
        }
    }
}
Write-Host "    loaded $($offsetMap.Count) client structMember offsets"

# ---------------------------------------------------------------------------
# Step 3 - Patch Offsets.h marker lines
# ---------------------------------------------------------------------------
Write-Host "==> [3/3] Patching $OffsetsFile" -ForegroundColor Cyan
$lines = Get-Content -LiteralPath $OffsetsAbs
$out = New-Object System.Collections.Generic.List[string]
$changes = @()
$unmatched = @()

for ($i = 0; $i -lt $lines.Count; $i++) {
    $line = $lines[$i]
    $m = $LineRx.Match($line)
    if (-not $m.Success) { $out.Add($line); continue }

    $artifact = $m.Groups[3].Value
    if ($offsetMap.ContainsKey($artifact)) {
        $rawHex = $offsetMap[$artifact]          # e.g. "0x1c0"
        $digits  = $rawHex -replace '^0x', ''
        $newHex  = '0x' + $digits.ToUpper()
        $oldLitMatch = ([regex]'0x[0-9A-Fa-f]+').Match($line)
        $oldHex = if ($oldLitMatch.Success) { $oldLitMatch.Value } else { '?' }
        $patched = $m.Groups[1].Value + $newHex + $m.Groups[2].Value + $m.Groups[3].Value + $m.Groups[4].Value
        $out.Add($patched)
        $changes += [pscustomobject]@{ Artifact = $artifact; Old = $oldHex; New = $newHex; Line = ($i + 1) }
    } else {
        $unmatched += [pscustomobject]@{ Artifact = $artifact; Line = ($i + 1) }
        $out.Add($line)
    }
}

if ($changes.Count -gt 0) {
    Write-Host "    changes:" -ForegroundColor Green
    foreach ($c in $changes) {
        Write-Host ("      L{0,-4} {1}: {2} -> {3}" -f $c.Line, $c.Artifact, $c.Old, $c.New)
    }
} else {
    Write-Host "    no marker offsets needed changing" -ForegroundColor DarkGray
}
if ($unmatched.Count -gt 0) {
    Write-Host "    UNMATCHED markers (left untouched):" -ForegroundColor Yellow
    foreach ($u in $unmatched) {
        Write-Host ("      L{0,-4} {1}" -f $u.Line, $u.Artifact)
    }
}

if (-not $DryRun -and $changes.Count -gt 0) {
    # Write UTF-8 *without* BOM to match the existing file and avoid spurious diffs.
    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllLines($OffsetsAbs, $out, $utf8NoBom)
    Write-Host "Applied $($changes.Count) offset patch(es) to $OffsetsFile" -ForegroundColor Green
} elseif (-not $DryRun -and $changes.Count -eq 0 -and -not $Force) {
    Write-Host "Nothing to patch; $OffsetsFile left unchanged." -ForegroundColor DarkGray
} elseif ($DryRun) {
    Write-Host "Dry run: no files written." -ForegroundColor Yellow
}

if ($unmatched.Count -gt 0) { exit 2 }
exit 0
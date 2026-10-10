$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$OutDir = Join-Path $PSScriptRoot "out"
$Exe = Join-Path $OutDir "hir_invariants.exe"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
Push-Location $Root
try {
    & gcc "-O2" "-Wall" "-Wextra" "-Iinclude" "tests/hir_invariants.c" "src/ast.c" "src/compiler/ir.c" "src/compiler/ir_lower.c" "src/compiler/cfg.c" "-o" $Exe
    if ($LASTEXITCODE -ne 0) { throw "HIR invariant test build failed." }
    & $Exe
    if ($LASTEXITCODE -ne 0) { throw "HIR invariant tests failed." }
}
finally {
    Pop-Location
}

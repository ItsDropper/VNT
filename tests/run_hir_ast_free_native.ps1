$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$OutDir = Join-Path $PSScriptRoot "out"
$Exe = Join-Path $OutDir "hir_ast_free_native.exe"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
Push-Location $Root
try {
    & gcc "-O2" "-Wall" "-Wextra" "-Iinclude" "-Isrc/compiler" "tests/hir_ast_free_native.c" "src/ast.c" "src/compiler/ir.c" "src/compiler/x86_backend.c" "-o" $Exe
    if ($LASTEXITCODE -ne 0) { throw "AST-free HIR pipeline test build failed." }
    & $Exe
    if ($LASTEXITCODE -ne 0) { throw "AST-free HIR pipeline test failed." }
}
finally {
    Pop-Location
}

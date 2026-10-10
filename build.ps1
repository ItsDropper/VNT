$ErrorActionPreference = "Stop"

$cc = "gcc"
$out = "vnt.exe"

$sources = @(
    "src/main.c",
    "src/modules.c",
    "src/lexer.c",
    "src/parser/parser.c",
    "src/parser/primary.c",
    "src/parser/unary.c",
    "src/parser/binary.c",
    "src/parser/functions.c",
    "src/parser/statements.c",
    "src/parser/simple_statements.c",
    "src/parser/control.c",
    "src/parser/loops.c",
    "src/parser/assignments.c",
    "src/ast.c",
    "src/compiler/compiler.c",
    "src/compiler/typecheck.c",
    "src/compiler/ir.c",
    "src/compiler/ir_lower.c",
    "src/compiler/cfg.c",
    "src/compiler/x86_backend.c"
)

Write-Host "Building VNT..."

& $cc "-O3" "-Iinclude" "-Isrc/compiler" @sources "-o" $out

if ($LASTEXITCODE -ne 0) {
    throw "VNT build failed."
}

Write-Host "Build successful: $out"

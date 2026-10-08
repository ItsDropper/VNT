$ErrorActionPreference = "Stop"

$cc = "gcc"
$out = "vnt.exe"

$sources = @(
    "src/main.c",
    "src/lexer.c",
    "src/parser/parser.c",
    "src/parser/expressions.c",
    "src/parser/functions.c",
    "src/parser/statements.c",
    "src/ast.c",
    "src/interpreter.c",
    "src/interpreter/expressions.c",
    "src/interpreter/functions.c",
    "src/interpreter/statements.c",
    "src/environment.c",
    "src/value.c"
)

Write-Host "Building VNT..."

& $cc "-Iinclude" @sources "-o" $out

if ($LASTEXITCODE -ne 0) {
    throw "VNT build failed."
}

Write-Host "Build successful: $out"

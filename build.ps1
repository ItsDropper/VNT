$ErrorActionPreference = "Stop"

$cc = "gcc"
$out = "vnt.exe"

$sources = @(
    "src/main.c",
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
    "src/interpreter.c",
    "src/interpreter/expressions.c",
      "src/interpreter/expression_helpers.c",
      "src/interpreter/expression_logic.c",
      "src/interpreter/expression_index.c",
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

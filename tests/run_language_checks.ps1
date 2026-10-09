$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Push-Location $Root
try {
    & .\build.ps1
    if ($LASTEXITCODE -ne 0) { throw "VNT build failed." }
    $OutDir = Join-Path $Root "tests\out"
    New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
    $Exe = Join-Path $OutDir "typed_functions.exe"
    & .\vnt.exe --compile tests\typed_functions.vnt -o $Exe
    if ($LASTEXITCODE -ne 0) { throw "Typed-function program did not compile." }
    $output = & $Exe
    if ($LASTEXITCODE -ne 0) { throw "Typed-function program exited unsuccessfully." }
    $expected = @("42", "active", "12.56", "TYPED_FUNCTIONS_OK")
    if (($output -join "|") -ne ($expected -join "|")) {
        throw "Unexpected typed-function output: $($output -join ' | ')"
    }
    foreach ($case in @(
        "tests\typed_function_argument_error.vnt",
        "tests\typed_function_return_error.vnt",
        "tests\typed_function_missing_return.vnt"
    )) {
        & .\vnt.exe --compile $case -o (Join-Path $OutDir "should_not_exist.exe") 2>&1 | Out-Null
        if ($LASTEXITCODE -eq 0) { throw "Expected type-check failure for $case, but compilation succeeded." }
    }
    Write-Host "Language checks: PASS"
}
finally {
    Pop-Location
}

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Push-Location $Root
try {
    & .\build.ps1
    if ($LASTEXITCODE -ne 0) { throw "VNT build failed." }
    & .\tests\run_hir_invariants.ps1
    if ($LASTEXITCODE -ne 0) { throw "HIR invariant suite failed." }
    & .\tests\run_hir_ast_free_native.ps1
    if ($LASTEXITCODE -ne 0) { throw "AST-free HIR pipeline suite failed." }
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
        $previousPreference = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        try {
            & .\vnt.exe --compile $case -o (Join-Path $OutDir "should_not_exist.exe") 2>&1 | Out-Null
            $compileExitCode = $LASTEXITCODE
        }
        finally {
            $ErrorActionPreference = $previousPreference
        }
        if ($compileExitCode -eq 0) { throw "Expected type-check failure for $case, but compilation succeeded." }
    }

    $ModuleExe = Join-Path $OutDir "modules.exe"
    & .\vnt.exe --compile tests\modules\main.vnt -o $ModuleExe
    if ($LASTEXITCODE -ne 0) { throw "Existing multi-file module project did not compile." }
    $moduleOutput = & $ModuleExe
    if ($LASTEXITCODE -ne 0 -or ($moduleOutput -join "|") -ne "Modules:|42|Optimized:|65") {
        throw "Unexpected multi-file module output: $($moduleOutput -join ' | ')"
    }

    $NamespacedExe = Join-Path $OutDir "namespaced_modules.exe"
    & .\vnt.exe --compile tests\modules\namespaced_main.vnt -o $NamespacedExe
    if ($LASTEXITCODE -ne 0) { throw "Namespaced cross-file function call did not compile." }
    $namespacedOutput = & $NamespacedExe
    if ($LASTEXITCODE -ne 0 -or ($namespacedOutput -join "|") -ne "42|NAMESPACED_MODULE_OK") {
        throw "Unexpected namespaced module output: $($namespacedOutput -join ' | ')"
    }

    foreach ($case in @(
        "tests\modules\circular_a.vnt",
        "tests\modules\malformed_import.vnt"
    )) {
        $previousPreference = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        try {
            & .\vnt.exe --compile $case -o (Join-Path $OutDir "invalid_module.exe") 2>&1 | Out-Null
            $compileExitCode = $LASTEXITCODE
        }
        finally {
            $ErrorActionPreference = $previousPreference
        }
        if ($compileExitCode -eq 0) { throw "Expected module-loading failure for $case, but compilation succeeded." }
    }
    Write-Host "Language and module checks: PASS"
}
finally {
    Pop-Location
}

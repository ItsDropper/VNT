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
        "tests\typed_function_missing_return.vnt",
        "tests\typecheck_invalid_ordered_comparison.vnt",
        "tests\typecheck_invalid_equality.vnt",
        "tests\typecheck_invalid_boolean_order.vnt",
        "tests\typecheck_invalid_void_return.vnt",
        "tests\typecheck_scope_if_leak.vnt",
        "tests\typecheck_scope_while_leak.vnt",
        "tests\typecheck_read_before_assignment.vnt",
        "tests\typecheck_shadow_same_scope.vnt"
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
    $UltimateExe = Join-Path $OutDir "ultimate_stress.exe"
    & .\vnt.exe --compile tests\ultimate_stress.vnt -o $UltimateExe
    if ($LASTEXITCODE -ne 0) { throw "Ultimate stress-test program did not compile." }
    $ultimateOutput = & $UltimateExe
    if ($LASTEXITCODE -ne 0) { throw "Ultimate stress-test program exited unsuccessfully." }
    $ultimateExpected = @(
        "factorial=3628800",
        "fibonacci=144",
        "sum=5050",
        "precedence=14",
        "parentheses=20",
        "classify=negative,zero,positive",
        "logic=true",
        "comparison=true",
        "float=6.25",
        "global=41,42",
        "ULTIMATE_STRESS_OK"
    )
    if (($ultimateOutput -join "|") -ne ($ultimateExpected -join "|")) {
        throw "Unexpected ultimate stress-test output: $($ultimateOutput -join ' | ')"
    }
    Write-Host "Ultimate stress test: PASS"

    $ComparisonExe = Join-Path $OutDir "typecheck_comparisons.exe"
    & .\vnt.exe --compile tests\typecheck_comparisons.vnt -o $ComparisonExe
    if ($LASTEXITCODE -ne 0) { throw "Valid comparison program did not compile." }
    $comparisonOutput = & $ComparisonExe
    if ($LASTEXITCODE -ne 0) { throw "Valid comparison program exited unsuccessfully." }
    $comparisonExpected = @("true", "true", "true", "true", "true", "TYPECHECK_COMPARISONS_OK")
    if (($comparisonOutput -join "|") -ne ($comparisonExpected -join "|")) {
        throw "Unexpected comparison output: $($comparisonOutput -join ' | ')"
    }
    Write-Host "Comparison type checks: PASS"

    $NumericExe = Join-Path $OutDir "typecheck_numeric_compatibility.exe"
    & .\vnt.exe --compile tests\typecheck_numeric_compatibility.vnt -o $NumericExe
    if ($LASTEXITCODE -ne 0) { throw "Numeric-compatible typed functions did not compile." }
    $numericOutput = & $NumericExe
    if ($LASTEXITCODE -ne 0) { throw "Numeric-compatible typed functions exited unsuccessfully." }
    if ($numericOutput[-1] -ne "NUMERIC_COMPATIBILITY_OK") {
        throw "Numeric compatibility program failed: $($numericOutput -join ' | ')"
    }
    Write-Host "Consistent numeric type compatibility: PASS"

    $ScopeExe = Join-Path $OutDir "typecheck_scopes.exe"
    & .\vnt.exe --compile tests\typecheck_scopes.vnt -o $ScopeExe
    if ($LASTEXITCODE -ne 0) { throw "Valid lexical-scope program did not compile." }
    $scopeOutput = & $ScopeExe
    if ($LASTEXITCODE -ne 0) { throw "Lexical-scope program exited unsuccessfully." }
    $scopeExpected = @("3", "2", "5", "SCOPE_CHECKS_OK")
    if (($scopeOutput -join "|") -ne ($scopeExpected -join "|")) {
        throw "Unexpected lexical-scope output: $($scopeOutput -join ' | ')"
    }
    Write-Host "Lexical scope checks: PASS"

    $ShadowExe = Join-Path $OutDir "typecheck_shadowing.exe"
    & .\vnt.exe --compile tests\typecheck_shadowing.vnt -o $ShadowExe
    if ($LASTEXITCODE -ne 0) { throw "Valid lexical-shadowing program did not compile." }
    $shadowOutput = & $ShadowExe
    if ($LASTEXITCODE -ne 0) { throw "Lexical-shadowing program exited unsuccessfully." }
    $shadowExpected = @("15", "10", "99", "10", "11", "10", "SYMBOL_SHADOW_OK")
    if (($shadowOutput -join "|") -ne ($shadowExpected -join "|")) {
        throw "Unexpected lexical-shadowing output: $($shadowOutput -join ' | ')"
    }
    Write-Host "Stable symbol identity and shadowing: PASS"

    $BranchExe = Join-Path $OutDir "typecheck_branch_merge.exe"
    & .\vnt.exe --compile tests\typecheck_branch_merge.vnt -o $BranchExe
    if ($LASTEXITCODE -ne 0) { throw "Branch type-merge program did not compile." }
    $branchOutput = & $BranchExe
    if ($LASTEXITCODE -ne 0 -or ($branchOutput -join "|") -ne "text|BRANCH_MERGE_OK") {
        throw "Unexpected branch type-merge output: $($branchOutput -join ' | ')"
    }
    Write-Host "Branch type merging: PASS"

    $ControlFlowExe = Join-Path $OutDir "optimizer_control_flow.exe"
    & .\vnt.exe --compile tests\optimizer_control_flow.vnt -o $ControlFlowExe
    if ($LASTEXITCODE -ne 0) { throw "Control-flow optimization program did not compile." }
    $controlFlowOutput = & $ControlFlowExe
    if ($LASTEXITCODE -ne 0 -or ($controlFlowOutput -join "|") -ne "CFG_OPT_OK") {
        throw "Unexpected control-flow optimization output: $($controlFlowOutput -join ' | ')"
    }
    Write-Host "Control-flow optimization: PASS"

    $ConstantPropagationExe = Join-Path $OutDir "optimizer_constant_propagation.exe"
    & .\vnt.exe --compile tests\optimizer_constant_propagation.vnt -o $ConstantPropagationExe
    if ($LASTEXITCODE -ne 0) { throw "Constant-propagation program did not compile." }
    $constantPropagationOutput = & $ConstantPropagationExe
    if ($LASTEXITCODE -ne 0) { throw "Constant-propagation program exited unsuccessfully." }
    $constantPropagationExpected = @("7", "9", "CONSTANT_BRANCH", "2", "CONSTANT_PROPAGATION_OK")
    if (($constantPropagationOutput -join "|") -ne ($constantPropagationExpected -join "|")) {
        throw "Unexpected constant-propagation output: $($constantPropagationOutput -join ' | ')"
    }
    Write-Host "HIR constant propagation: PASS"

    $StdIoExe = Join-Path $OutDir "std_io.exe"
    & .\vnt.exe --compile tests\std_io.vnt -o $StdIoExe
    if ($LASTEXITCODE -ne 0) { throw "Standard I/O library program did not compile." }
    $ioOutput = @("library input") | & $StdIoExe
    if ($LASTEXITCODE -ne 0 -or ($ioOutput -join "|") -ne "IO_OK|library input") {
        throw "Unexpected standard I/O output: $($ioOutput -join ' | ')"
    }
    Write-Host "Standard I/O library: PASS"

    $StdFsExe = Join-Path $OutDir "std_fs.exe"
    & .\vnt.exe --compile tests\std_fs.vnt -o $StdFsExe
    if ($LASTEXITCODE -ne 0) { throw "Standard filesystem library program did not compile." }
    $fsOutput = & $StdFsExe
    if ($LASTEXITCODE -ne 0 -or ($fsOutput -join "|") -ne "FS_OK|STANDARD_FS_OK") {
        throw "Unexpected standard filesystem output: $($fsOutput -join ' | ')"
    }
    Write-Host "Standard filesystem library: PASS"

    Write-Host "Language and module checks: PASS"
}
finally {
    Pop-Location
}

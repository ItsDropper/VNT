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
    $LoopsExe = Join-Path $OutDir "loops.exe"
    & .\vnt.exe --compile tests\loops.vnt -o $LoopsExe
    if ($LASTEXITCODE -ne 0) { throw "Valid break/continue loop program did not compile." }
    $loopsOutput = & $LoopsExe
    if ($LASTEXITCODE -ne 0) { throw "Valid break/continue loop program exited unsuccessfully." }
    $loopsExpected = @("break", "1", "2", "3", "4", "continue", "1", "2", "4", "5", "while", "1", "3", "4")
    if (($loopsOutput -join "|") -ne ($loopsExpected -join "|")) {
        throw "Unexpected loop-control output: $($loopsOutput -join ' | ')"
    }
    Write-Host "Valid loop-control checks: PASS"

    $NestedLoopsExe = Join-Path $OutDir "nested_loop_control.exe"
    & .\\vnt.exe --compile tests\\nested_loop_control.vnt -o $NestedLoopsExe
    if ($LASTEXITCODE -ne 0) { throw "Nested loop-control program did not compile." }
    $nestedLoopsOutput = & $NestedLoopsExe
    if ($LASTEXITCODE -ne 0) { throw "Nested loop-control program exited unsuccessfully." }
    $nestedLoopsExpected = @("nested", "1", "1", "3", "2", "1", "3", "3", "1", "3")
    if (($nestedLoopsOutput -join "|") -ne ($nestedLoopsExpected -join "|")) {
        throw "Unexpected nested loop-control output: $($nestedLoopsOutput -join ' | ')"
    }
    Write-Host "Nested loop-control checks: PASS"

    $NestedReturnsExe = Join-Path $OutDir "typed_function_nested_returns.exe"
    & .\vnt.exe --compile tests\typed_function_nested_returns.vnt -o $NestedReturnsExe
    if ($LASTEXITCODE -ne 0) { throw "Complete nested typed-function returns did not compile." }
    $nestedReturnsOutput = & $NestedReturnsExe
    if ($LASTEXITCODE -ne 0) { throw "Nested typed-function return program exited unsuccessfully." }
    $nestedReturnsExpected = @("7", "9", "NESTED_TYPED_RETURNS_OK")
    if (($nestedReturnsOutput -join "|") -ne ($nestedReturnsExpected -join "|")) {
        throw "Unexpected nested typed-function return output: $($nestedReturnsOutput -join ' | ')"
    }
    Write-Host "Nested typed-function return checks: PASS"

    $UnsafeBranchesExe = Join-Path $OutDir "typed_function_unsafe_branch_returns.exe"
    & .\vnt.exe --compile tests\typed_function_unsafe_branch_returns.vnt -o $UnsafeBranchesExe
    if ($LASTEXITCODE -ne 0) { throw "Unsafe-block branch-return program did not compile." }
    $unsafeBranchesOutput = & $UnsafeBranchesExe
    if ($LASTEXITCODE -ne 0 -or ($unsafeBranchesOutput -join "|") -ne "67|68|TYPED_UNSAFE_BRANCH_RETURNS_OK") {
        throw "Unexpected unsafe-block branch-return output: $($unsafeBranchesOutput -join ' | ')"
    }
    Write-Host "Unsafe-block branch return checks: PASS"

    $BareReturnExe = Join-Path $OutDir "void_bare_return.exe"
    & .\vnt.exe --compile tests\void_bare_return.vnt -o $BareReturnExe
    if ($LASTEXITCODE -ne 0) { throw "Bare void-return program did not compile." }
    $bareReturnOutput = & $BareReturnExe
    if ($LASTEXITCODE -ne 0 -or ($bareReturnOutput -join "|") -ne "FALLTHROUGH|VOID_BARE_RETURN_OK") {
        throw "Unexpected bare void-return output: $($bareReturnOutput -join ' | ')"
    }
    Write-Host "Bare void-return semantics: PASS"

    foreach ($case in @(
        "tests\typed_function_argument_error.vnt",
        "tests\typed_function_return_error.vnt",
        "tests\typed_function_missing_return.vnt",
        "tests\typed_function_loop_only_return.vnt",
        "tests\typed_function_nested_partial_return.vnt",
        "tests\typed_function_unsafe_partial_return.vnt",
        "tests\typecheck_break_outside_loop.vnt",
        "tests\typecheck_continue_outside_loop.vnt",
        "tests\typecheck_break_in_function_rejected.vnt",
        "tests\typecheck_continue_in_function_rejected.vnt",
        "tests\typecheck_break_in_conditional_rejected.vnt",
        "tests\typecheck_unknown_return_type.vnt",
        "tests\typecheck_void_parameter_type.vnt",
        "tests\typecheck_unknown_parameter_type.vnt",
        "tests\typecheck_duplicate_parameter_names.vnt",
        "tests\typecheck_argument_type_mismatch.vnt",
        "tests\typecheck_return_type_mismatch.vnt",
        "tests\typecheck_explicit_assignment_type_mismatch.vnt",
        "tests\typecheck_initializer_type_mismatch.vnt",
        "tests\typecheck_function_arity_mismatch.vnt",
        "tests\typecheck_function_too_many_arguments.vnt",
        "tests\typecheck_builtin_arity_mismatch.vnt",
        "tests\typecheck_builtin_too_few_arguments.vnt",
        "tests\typecheck_undeclared_function.vnt",
        "tests\typecheck_void_argument_rejected.vnt",
        "tests\typecheck_void_unary_expression_rejected.vnt",
        "tests\typecheck_void_condition_rejected.vnt",
        "tests\typecheck_bare_return_nonvoid_rejected.vnt",
        "tests\typecheck_void_while_condition_rejected.vnt",
        "tests\typecheck_void_array_element_rejected.vnt",
        "tests\typecheck_struct_constructor_arguments_rejected.vnt",
        "tests\typecheck_nested_function_cannot_break_caller_loop.vnt",
        "tests\typecheck_duplicate_struct_fields.vnt",
        "tests\typecheck_duplicate_struct_definitions.vnt",
        "tests\typecheck_duplicate_function_definitions.vnt",
        "tests\typecheck_invalid_ordered_comparison.vnt",
        "tests\typecheck_invalid_equality.vnt",
        "tests\typecheck_invalid_boolean_order.vnt",
        "tests\typecheck_invalid_void_return.vnt",
        "tests\typecheck_scope_if_leak.vnt",
        "tests\typecheck_scope_while_leak.vnt",
        "tests\typecheck_read_before_assignment.vnt",
        "tests\typecheck_shadow_same_scope.vnt"
    )) {
        $invalidOutput = Join-Path $OutDir ("invalid-" + [System.IO.Path]::GetFileNameWithoutExtension($case) + ".exe")
        if (Test-Path $invalidOutput) { Remove-Item -Force $invalidOutput }
        $previousPreference = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        try {
            & .\vnt.exe --compile $case -o $invalidOutput 2>&1 | Out-Null
            $compileExitCode = $LASTEXITCODE
        }
        finally {
            $ErrorActionPreference = $previousPreference
        }
        if ($compileExitCode -eq 0) { throw "Expected type-check failure for $case, but compilation succeeded." }
        if (Test-Path $invalidOutput) { throw "Failed compilation left an output artifact for $case." }
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
    if ($LASTEXITCODE -ne 0 -or ($fsOutput -join "|") -ne "FS_OK|FS_READ_LINES_OK|STANDARD_FS_OK") {
        throw "Unexpected standard filesystem output: $($fsOutput -join ' | ')"
    }
    Write-Host "Standard filesystem library: PASS"

    $StdTextExe = Join-Path $OutDir "std_text_advanced.exe"
    & .\vnt.exe --compile tests\std_text_advanced.vnt -o $StdTextExe
    if ($LASTEXITCODE -ne 0) { throw "Advanced text helpers did not compile." }
    $textOutput = & $StdTextExe
    if ($LASTEXITCODE -ne 0) { throw "Advanced text program exited unsuccessfully." }
    $textExpected = @(
        "4", "alpha", "true", "gamma", "true", "8", "abc", "ef",
        "value", "filename", "00042", "vnt...", "2", "0", "STD_TEXT_ADVANCED_OK"
    )
    if (($textOutput -join "|") -ne ($textExpected -join "|")) {
        throw "Unexpected advanced text output: $($textOutput -join ' | ')"
    }
    Write-Host "Advanced text helpers: PASS"

    $StdTextFsExe = Join-Path $OutDir "std_text_fs_lines.exe"
    & .\vnt.exe --compile tests\std_text_fs_lines.vnt -o $StdTextFsExe
    if ($LASTEXITCODE -ne 0) { throw "Filesystem line helpers did not compile." }
    $textFsOutput = & $StdTextFsExe
    if ($LASTEXITCODE -ne 0) { throw "Filesystem line program exited unsuccessfully." }
    $textFsExpected = @("true", "2", "first", "second", "true", "3", "third", "STD_FS_LINES_OK")
    if (($textFsOutput -join "|") -ne ($textFsExpected -join "|")) {
        throw "Unexpected filesystem line output: $($textFsOutput -join ' | ')"
    }
    Write-Host "Filesystem line helpers: PASS"

    $StdCollectionsExe = Join-Path $OutDir "std_collections_strings_numbers.exe"
    & .\vnt.exe --compile tests\std_collections_strings_numbers.vnt -o $StdCollectionsExe
    if ($LASTEXITCODE -ne 0) { throw "Standard array/string/number libraries did not compile." }
    $collectionsOutput = & $StdCollectionsExe
    if ($LASTEXITCODE -ne 0) { throw "Standard array/string/number program exited unsuccessfully." }
    $collectionsExpected = @(
        "true", "1", "2", "24", "4.8", "2", "9", "[9, 2, 7, 2, 4]",
        "vnt-stdlib-ok", "0", "[1, 2, 3, 4]", "[4, 2, 7]", "[4, 2, 7, 2, 9]", "[]",
        "[7, 2, 9]", "[]", "[4, 2, 7, 2, 9]", "[3, 1, 2]",
        "true", "true", "true", "true", "true", "1", "2", "ababab", "false",
        "one cat, two cat", "unchanged", "VNT stdlib", "",
        "true", "true", "-1", "0", "1", "720", "-1", "6", "true", "false",
        "STDLIB_COLLECTIONS_STRINGS_NUMBERS_OK"
    )
    if (($collectionsOutput -join "|") -ne ($collectionsExpected -join "|")) {
        throw "Unexpected standard array/string/number output: $($collectionsOutput -join ' | ')"
    }
    Write-Host "Standard array/string/number libraries: PASS"

    $StatisticsExe = Join-Path $OutDir "std_statistics.exe"
    & .\vnt.exe --compile tests\std_statistics.vnt -o $StatisticsExe
    if ($LASTEXITCODE -ne 0) { throw "Standard statistics library did not compile." }
    $statisticsOutput = & $StatisticsExe
    if ($LASTEXITCODE -ne 0) { throw "Standard statistics program exited unsuccessfully." }
    $statisticsExpected = @(
        "4.5", "4", "1", "9", "8", "8.75", "0", "1", "2.5", "4", "9", "1", "9",
        "[1, 3, 5, 9]", "[9, 1, 5, 3]", "0", "0", "0", "STDLIB_STATISTICS_OK"
    )
    if (($statisticsOutput -join "|") -ne ($statisticsExpected -join "|")) {
        throw "Unexpected standard statistics output: $($statisticsOutput -join ' | ')"
    }
    Write-Host "Standard statistics library: PASS"

    $ValidateExe = Join-Path $OutDir "std_validate.exe"
    & .\vnt.exe --compile tests\std_validate.vnt -o $ValidateExe
    if ($LASTEXITCODE -ne 0) { throw "Standard validation library did not compile." }
    $validateOutput = & $ValidateExe
    if ($LASTEXITCODE -ne 0) { throw "Standard validation program exited unsuccessfully." }
    $validateExpected = @(
        "true", "true", "false", "true", "false", "true", "false", "true", "false",
        "true", "true", "true", "false", "false", "true", "true", "true", "false",
        "false", "true", "false", "true", "false", "true", "false", "false",
        "true", "true", "false", "false", "false", "false", "false", "false", "true", "STDLIB_VALIDATE_OK"
    )
    if (($validateOutput -join "|") -ne ($validateExpected -join "|")) {
        throw "Unexpected standard validation output: $($validateOutput -join ' | ')"
    }
    Write-Host "Standard validation library: PASS"

    $StdFoundationExe = Join-Path $OutDir "std_foundation.exe"
    & .\vnt.exe --compile tests\std_foundation.vnt -o $StdFoundationExe
    if ($LASTEXITCODE -ne 0) { throw "Standard foundation libraries did not compile." }
    $foundationOutput = & $StdFoundationExe
    if ($LASTEXITCODE -ne 0) { throw "Standard foundation program exited unsuccessfully." }
    $foundationExpected = @("49", "10", "15", "2", "2", "true", "ready", "true", "true", "STD_FOUNDATION_OK")
    if (($foundationOutput -join "|") -ne ($foundationExpected -join "|")) {
        throw "Unexpected standard foundation output: $($foundationOutput -join ' | ')"
    }
    Write-Host "Standard math/time/OS/process libraries: PASS"

    # Compile-only checks keep networking tests offline and avoid contacting real APIs.
    $StdNetworkExe = Join-Path $OutDir "std_network_compile.exe"
    & .\vnt.exe --compile tests\std_network_compile.vnt -o $StdNetworkExe
    if ($LASTEXITCODE -ne 0) { throw "Standard networking library did not compile." }
    Write-Host "Standard HTTPS/API library compile: PASS"

    $StdLogConfigExe = Join-Path $OutDir "std_log_config_compile.exe"
    & .\vnt.exe --compile tests\std_log_config_compile.vnt -o $StdLogConfigExe
    if ($LASTEXITCODE -ne 0) { throw "Standard logging/configuration libraries did not compile." }
    Write-Host "Standard logging/configuration library compile: PASS"

    $JsonExe = Join-Path $OutDir "json_value_at.exe"
    & .\vnt.exe --compile tests\json_value_at.vnt -o $JsonExe
    if ($LASTEXITCODE -ne 0) { throw "JSON field accessor test did not compile." }
    $jsonOutput = & $JsonExe
    if ($LASTEXITCODE -ne 0 -or ($jsonOutput -join "|") -ne "Sodium|lithium|100||1|2500|JSON_VALUE_AT_OK") {
        throw "Unexpected JSON field accessor output: $($jsonOutput -join ' | ')"
    }
    Write-Host "Standard JSON field accessor: PASS"


    $CsvExe = Join-Path $OutDir "std_csv.exe"
    & .\vnt.exe --compile tests\std_csv.vnt -o $CsvExe
    if ($LASTEXITCODE -ne 0) { throw "Standard CSV library did not compile." }
    $csvOutput = & $CsvExe
    if ($LASTEXITCODE -ne 0) { throw "Standard CSV program exited unsuccessfully." }
    $csvExpected = @(
        "4", "name", "Ada", "likes, commas", "13", "a `"quote`"",
        "likes, commas", "13", "a `\"quote`\"", "[]", "[]", "[]", "1", "13", "tail", "2", "0", "2", "left", "right", '\"\"', "1", "1", "0", '\"\",middle,\"\"', "3", "0", "middle", "0", "CSV_STANDARD_LIBRARY_OK"
    )
    if (($csvOutput -join "|") -ne ($csvExpected -join "|")) {
        throw "Unexpected standard CSV output: $($csvOutput -join ' | ')"
    }
    Write-Host "Standard CSV parser/serializer: PASS"

    $StringExe = Join-Path $OutDir "std_string.exe"
    & .\vnt.exe --compile tests\std_string.vnt -o $StringExe
    if ($LASTEXITCODE -ne 0) { throw "Standard string helpers did not compile." }
    $stringOutput = & $StringExe
    if ($LASTEXITCODE -ne 0) { throw "Standard string helper program exited unsuccessfully." }
    $stringExpected = @(
        "true", "true", "true", "false", "true", "2", "4", "true", "2",
        "ababab", "1 two 1", "value", "4", "a", "0", "b", "0", "1", "0",
        "1", "0", "1", "abc", "abc", "", "ef", "abc", "compiler", "main",
        "vnt", "vnt", "007", "xaba", "x", "0", "2", "2", "STRING_STANDARD_LIBRARY_OK"
    )
    if (($stringOutput -join "|") -ne ($stringExpected -join "|")) {
        throw "Unexpected standard string output: $($stringOutput -join ' | ')"
    }
    Write-Host "Standard string helper regressions: PASS"

    $PathExe = Join-Path $OutDir "std_path.exe"
    & .\vnt.exe --compile tests\std_path.vnt -o $PathExe
    if ($LASTEXITCODE -ne 0) { throw "Standard path library did not compile." }
    $pathOutput = & $PathExe
    if ($LASTEXITCODE -ne 0) { throw "Standard path program exited unsuccessfully." }
    $pathExpected = @(
        "src/main/file.vnt", "main.vnt", "C:/Users/dropper/project", "gz",
        "archive.tar", "", ".env", ".", "", "src/main/file.vnt",
        "/absolute/file", "D:/absolute/file", "C:/", "", "false", "true", "false", "true", ".", "archive.tar.zip", "report.pdf", ".env.txt", "dir/report", "folder", "/", "C:/", "", "C:/", "C:/", "PATH_STANDARD_LIBRARY_OK"
    )
    if (($pathOutput -join "|") -ne ($pathExpected -join "|")) {
        throw "Unexpected standard path output: $($pathOutput -join ' | ')"
    }
    Write-Host "Standard portable path helpers: PASS"

    $ConfigIniExe = Join-Path $OutDir "std_config_ini.exe"
    & .\vnt.exe --compile tests\std_config_ini.vnt -o $ConfigIniExe
    if ($LASTEXITCODE -ne 0) { throw "Standard INI configuration helpers did not compile." }
    $configIniOutput = & $ConfigIniExe
    if ($LASTEXITCODE -ne 0) { throw "Standard INI configuration program exited unsuccessfully." }
    $configIniExpected = @(
        "enabled", "dark mode", "125", "a=b", "fallback",
        "true", "false", "true", "true", "false", "false", "true", "false", "true", "true", "false", "INI_CONFIG_OK"
    )
    if (($configIniOutput -join "|") -ne ($configIniExpected -join "|")) {
        throw "Unexpected standard INI configuration output: $($configIniOutput -join ' | ')"
    }
    Write-Host "Standard INI configuration parsing: PASS"

    $ImprovedModulesExe = Join-Path $OutDir "std_improved_modules.exe"
    & .\vnt.exe --compile tests\std_improved_modules.vnt -o $ImprovedModulesExe
    if ($LASTEXITCODE -ne 0) { throw "Improved standard modules did not compile." }
    $improvedModulesOutput = & $ImprovedModulesExe
    if ($LASTEXITCODE -ne 0) { throw "Improved standard modules program exited unsuccessfully." }
    $improvedModulesExpected = @(
        "120000", "3600000", "86400000", "2", "1", "1",
        "true", "true", "true", "true", "true", "false",
        "src/main.exe", "C:/VNT/main.elf", ".env.txt",
        "true", "true", "true", "true", "true", "true",
        "name=Ada&theme=dark", "", "VNT", "fallback", "true", "false", "fallback", "false",
        "true", "false", "true", "29", "false", "61", "1", "6", "-1",
        "STD_IMPROVED_MODULES_OK"
    )
    if (($improvedModulesOutput -join "|") -ne ($improvedModulesExpected -join "|")) {
        throw "Unexpected improved standard module output: $($improvedModulesOutput -join ' | ')"
    }
    Write-Host "Improved time/path/network/JSON helpers: PASS"

    $RuntimeFeaturesExe = Join-Path $OutDir "runtime_features.exe"
    & .\\vnt.exe --compile tests\\runtime_features.vnt -o $RuntimeFeaturesExe
    if ($LASTEXITCODE -ne 0) { throw "UTF-8 and atomic-write runtime tests did not compile." }
    $runtimeFeaturesOutput = & $RuntimeFeaturesExe
    if ($LASTEXITCODE -ne 0) { throw "UTF-8 and atomic-write runtime test program exited unsuccessfully." }
    $runtimeFeaturesExpected = @("4", "é中", "🙂", "", "true", "first", "true", "replacement", "true", "9", "1970", "1", "1", "0", "0", "0", "4", "1", "0", "RUNTIME_FEATURES_OK")
    if (($runtimeFeaturesOutput -join "|") -ne ($runtimeFeaturesExpected -join "|")) {
        throw "Unexpected UTF-8/atomic-write output: $($runtimeFeaturesOutput -join ' | ')"
    }
    Write-Host "UTF-8 scalar helpers and atomic filesystem writes: PASS"

    $BigIntExe = Join-Path $OutDir "std_bigint_runtime.exe"
    & .\\vnt.exe --compile tests\\std_bigint_runtime.vnt -o $BigIntExe
    if ($LASTEXITCODE -ne 0) { throw "Big integer runtime test did not compile." }
    $bigintOutput = & $BigIntExe
    if ($LASTEXITCODE -ne 0) { throw "Big integer runtime program exited unsuccessfully." }
    $bigintExpected = @(
        "1000000000000000000000", "999999999999999999999", "111111110111111111010",
        "1", "-8", "0", "-1", "0", "1", "BIGINT_RUNTIME_OK"
    )
    if (($bigintOutput -join "|") -ne ($bigintExpected -join "|")) {
        throw "Unexpected big integer output: $($bigintOutput -join ' | ')"
    }
    Write-Host "Arbitrary-precision integer runtime: PASS"

    $CryptoExe = Join-Path $OutDir "std_crypto_runtime.exe"
    & .\\vnt.exe --compile tests\\std_crypto_runtime.vnt -o $CryptoExe
    if ($LASTEXITCODE -ne 0) { throw "Crypto runtime test did not compile." }
    $cryptoOutput = & $CryptoExe
    if ($LASTEXITCODE -ne 0) { throw "Crypto runtime program exited unsuccessfully." }
    $cryptoExpected = @(
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "true", "false", "CRYPTO_RUNTIME_OK"
    )
    if (($cryptoOutput -join "|") -ne ($cryptoExpected -join "|")) {
        throw "Unexpected crypto runtime output: $($cryptoOutput -join ' | ')"
    }
    Write-Host "SHA-256 and constant-time comparison: PASS"

    $VantaOneExe = Join-Path $OutDir "vanta_one.exe"
    & .\vnt.exe --compile examples\vanta_one.vnt -o $VantaOneExe
    if ($LASTEXITCODE -ne 0) { throw "Vanta One desktop app did not compile." }
    Write-Host "Vanta One desktop app compile: PASS"

    Write-Host "Language and module checks: PASS"
}
finally {
    Pop-Location
}

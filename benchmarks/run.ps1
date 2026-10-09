param([int]$Runs = 7, [int]$Warmups = 1)
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Vnt = Join-Path $Root "vnt.exe"
$OutDir = Join-Path $PSScriptRoot "out"
if (-not (Test-Path $Vnt)) { throw "Missing vnt.exe. Build with .\build.ps1 first." }
if ($Runs -lt 3) { throw "Use at least 3 measured runs; recommended: -Runs 7." }
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$Cases = @(
    @{ Name = "integer-loop"; Source = "benchmarks\loop_sum.vnt"; Marker = "799980000"; Expected = @("799980000") },
    @{ Name = "hir-constant-folding"; Source = "tests\hir_constant_folding.vnt"; Marker = "HIR_CONSTANT_FOLDING_OK"; Expected = @("14", "4", "false", "3.75", "10", "3", "true") },
    @{ Name = "branches-logic"; Source = "benchmarks\suite\branches_logic.vnt"; Marker = "BRANCHES_LOGIC_OK"; Expected = @("133255") },
    @{ Name = "function-calls"; Source = "benchmarks\suite\function_calls.vnt"; Marker = "FUNCTION_CALLS_OK"; Expected = @("450195000") },
    @{ Name = "recursion"; Source = "benchmarks\suite\recursion.vnt"; Marker = "RECURSION_OK"; Expected = @("51680") },
    @{ Name = "strings"; Source = "benchmarks\suite\strings.vnt"; Marker = "STRINGS_OK"; Expected = @("900") },
    @{ Name = "arrays"; Source = "benchmarks\suite\arrays.vnt"; Marker = "ARRAYS_OK"; Expected = @("50055") },
    @{ Name = "floats-math"; Source = "benchmarks\suite\floats_math.vnt"; Marker = "FLOAT_MATH_OK"; Expected = @() },
    @{ Name = "references"; Source = "benchmarks\suite\references.vnt"; Marker = "REFERENCES_OK"; Expected = @("10", "42") },
    @{ Name = "structs"; Source = "benchmarks\suite\struct_members.vnt"; Marker = "STRUCTS_OK"; Expected = @("12", "30") },
    @{ Name = "many-arguments"; Source = "benchmarks\suite\arguments.vnt"; Marker = "ARGUMENTS_OK"; Expected = @("36") },
    @{ Name = "ffi"; Source = "benchmarks\suite\ffi.vnt"; Marker = "FFI_OK"; Expected = @() },
    @{ Name = "multi-file"; Source = "benchmarks\suite\multifile\main.vnt"; Marker = "MULTIFILE_OK"; Expected = @("1250275000", "102334155") }
)
function Invoke-VntCompile([string]$Source, [string]$Exe) {
    & $Vnt --compile $Source -o $Exe
    if ($LASTEXITCODE -ne 0) { throw "Compile failed for $Source (exit code $LASTEXITCODE)." }
}
function Get-ProgramOutput([string]$Exe) {
    $output = @(& $Exe 2>&1 | ForEach-Object { "$_" })
    $exitCode = $LASTEXITCODE
    if ($exitCode -ne 0) { throw "Program failed: $Exe (exit code $exitCode). Captured output: $($output -join ' | ')" }
    return ,$output
}
Push-Location $Root
try {
    $Results = @()
    foreach ($case in $Cases) {
        $exe = Join-Path $OutDir ($case.Name + ".exe")
        Write-Host ""
        Write-Host ("=== {0} ===" -f $case.Name)
        Invoke-VntCompile $case.Source $exe
        $output = Get-ProgramOutput $exe
        if (-not ($output -contains $case.Marker)) {
            throw ("Correctness failed for {0}; missing marker {1}; output: {2}" -f $case.Name, $case.Marker, ($output -join ' | '))
        }
        foreach ($expectedLine in $case.Expected) {
            if (-not ($output -contains $expectedLine)) {
                throw ("Correctness failed for {0}; expected output line {1}; output: {2}" -f $case.Name, $expectedLine, ($output -join ' | '))
            }
        }
        Write-Host "Correctness: PASS"
        for ($i = 0; $i -lt $Warmups; $i++) {
            & $exe > $null
            if ($LASTEXITCODE -ne 0) { throw "Warmup failed for $($case.Name) (exit code $LASTEXITCODE)." }
        }
        $times = @()
        for ($i = 0; $i -lt $Runs; $i++) {
            $elapsed = (Measure-Command { & $exe > $null }).TotalMilliseconds
            if ($LASTEXITCODE -ne 0) { throw "Timed run failed for $($case.Name) (exit code $LASTEXITCODE)." }
            $times += [double]$elapsed
        }
        $sorted = @($times | Sort-Object)
        $median = $sorted[[int][math]::Floor($sorted.Count / 2)]
        $minimum = $sorted[0]
        $maximum = $sorted[$sorted.Count - 1]
        $formattedRuns = ($times | ForEach-Object { "{0:N2}" -f $_ }) -join ", "
        Write-Host ("Median {0,8:N2} ms | min {1,8:N2} ms | max {2,8:N2} ms" -f $median, $minimum, $maximum)
        Write-Host ("Runs: {0}" -f $formattedRuns)
        $Results += [pscustomobject]@{ Benchmark = $case.Name; MedianMs = [math]::Round($median, 2); MinMs = [math]::Round($minimum, 2); MaxMs = [math]::Round($maximum, 2); RunsMs = $formattedRuns; Correct = "PASS" }
    }
    Write-Host ""
    Write-Host "=== SUMMARY (milliseconds; lower is faster) ==="
    $Results | Format-Table -AutoSize
    $csv = Join-Path $OutDir "results.csv"
    $Results | Export-Csv -NoTypeInformation -Encoding UTF8 -Path $csv
    Write-Host ("CSV written to: {0}" -f $csv)
}
finally { Pop-Location }

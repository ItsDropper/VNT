param([int]$Runs = 9, [int]$Warmups = 2)
$ErrorActionPreference = "Stop"
if ($Runs -lt 3) { throw "Use at least 3 measured runs; recommended: -Runs 9." }
$Root = Split-Path -Parent $PSScriptRoot
$OutDir = Join-Path $PSScriptRoot "out"
$Vnt = Join-Path $Root "vnt.exe"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
foreach ($tool in @("gcc", "rustc")) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) { throw "Required tool '$tool' was not found on PATH." }
}
if (-not (Test-Path $Vnt)) { throw "Missing vnt.exe. Build it first with .\\build.ps1." }

Push-Location $Root
try {
    $VntExe = Join-Path $OutDir "loop_sum_vnt.exe"
    $CExe = Join-Path $OutDir "loop_sum_c.exe"
    $RustExe = Join-Path $OutDir "loop_sum_rust.exe"

    & $Vnt --compile "benchmarks/loop_sum.vnt" -o $VntExe
    if ($LASTEXITCODE -ne 0) { throw "VNT benchmark compilation failed." }
    & gcc -O3 -flto "benchmarks/loop_sum.c" -o $CExe
    if ($LASTEXITCODE -ne 0) { throw "C benchmark compilation failed." }
    & rustc -C opt-level=3 -C lto=thin -C codegen-units=1 "benchmarks/loop_sum.rs" -o $RustExe
    if ($LASTEXITCODE -ne 0) { throw "Rust benchmark compilation failed." }

    $Expected = "499999500000"
    $Programs = @(
        [pscustomobject]@{ Name = "VNT"; Path = $VntExe },
        [pscustomobject]@{ Name = "C"; Path = $CExe },
        [pscustomobject]@{ Name = "Rust"; Path = $RustExe }
    )
    foreach ($program in $Programs) {
        $output = (@(& $program.Path 2>&1) | ForEach-Object { "$_" }) -join "`n"
        if ($LASTEXITCODE -ne 0) { throw "$($program.Name) benchmark exited with code $LASTEXITCODE." }
        if ($output.Trim() -ne $Expected) {
            throw "$($program.Name) produced '$output'; expected '$Expected'. No timings will be reported."
        }
        Write-Host "$($program.Name) correctness: PASS ($Expected)"
    }

    $Results = @()
    foreach ($program in $Programs) {
        for ($i = 0; $i -lt $Warmups; $i++) {
            & $program.Path > $null
            if ($LASTEXITCODE -ne 0) { throw "$($program.Name) warmup failed." }
        }
        $times = @()
        for ($i = 0; $i -lt $Runs; $i++) {
            $watch = [System.Diagnostics.Stopwatch]::StartNew()
            & $program.Path > $null
            $exitCode = $LASTEXITCODE
            $watch.Stop()
            if ($exitCode -ne 0) { throw "$($program.Name) timed run failed with exit code $exitCode." }
            $times += $watch.Elapsed.TotalMilliseconds
        }
        $sorted = @($times | Sort-Object)
        if (($sorted.Count % 2) -eq 1) { $median = $sorted[[int][math]::Floor($sorted.Count / 2)] }
        else { $middle = [int]($sorted.Count / 2); $median = ($sorted[$middle - 1] + $sorted[$middle]) / 2 }
        $Results += [pscustomobject]@{
            Language = $program.Name
            MedianMs = [math]::Round($median, 3)
            MinMs = [math]::Round($sorted[0], 3)
            MaxMs = [math]::Round($sorted[-1], 3)
            SamplesMs = (($times | ForEach-Object { [math]::Round($_, 3) }) -join ", ")
        }
    }
    Write-Host "`n=== VNT vs C vs Rust (milliseconds; lower is faster) ==="
    $Results | Format-Table -AutoSize
    $csv = Join-Path $OutDir "comparison.csv"
    $Results | Export-Csv -NoTypeInformation -Encoding UTF8 -Path $csv
    Write-Host "CSV written to: $csv"
    Write-Host "All binaries use the same 20 x 1,000,000 loop workload. Timings include process startup."
}
finally { Pop-Location }

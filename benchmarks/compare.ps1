param([int]$Runs = 9, [int]$Warmups = 2, [int]$Rounds = 20)
$ErrorActionPreference = "Stop"
if ($Runs -lt 3) { throw "Use at least 3 measured runs; recommended: -Runs 9." }
if ($Rounds -lt 1) { throw "Use at least 1 round; the default is 20. Larger values (for example -Rounds 2000) make process startup negligible." }
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

    # Each round is 1,000,000 iterations. Build per-run copies of the three sources with the
    # requested round count so every language executes exactly the same workload.
    $RoundPattern = 'round < 20'
    $RoundReplacement = "round < $Rounds"
    $VntSrc = Join-Path $OutDir "loop_sum_rounds.vnt"
    $CSrc = Join-Path $OutDir "loop_sum_rounds.c"
    $RustSrc = Join-Path $OutDir "loop_sum_rounds.rs"
    foreach ($pair in @(@("loop_sum.vnt", $VntSrc), @("loop_sum.c", $CSrc), @("loop_sum.rs", $RustSrc))) {
        $text = [System.IO.File]::ReadAllText((Join-Path $PSScriptRoot $pair[0]))
        if (-not $text.Contains($RoundPattern)) { throw "Expected '$RoundPattern' in $($pair[0]); cannot set the round count." }
        [System.IO.File]::WriteAllText($pair[1], $text.Replace($RoundPattern, $RoundReplacement))
    }

    & $Vnt --compile $VntSrc -o $VntExe
    if ($LASTEXITCODE -ne 0) { throw "VNT benchmark compilation failed." }
    $Assembly = "$VntExe.s"
    if (-not (Test-Path $Assembly)) { throw "VNT compiler did not retain generated assembly: $Assembly" }
    if (Select-String -Path $Assembly -SimpleMatch "call vnt_fn_add_value" -Quiet) {
        throw "The hot-loop helper call was not inlined."
    }
    if (Select-String -Path $Assembly -SimpleMatch "jo .Lvnt_int_add_overflow" -Quiet) {
        throw "A proven-safe counted-loop addition still has a checked-overflow branch."
    }
    $SavedRegisterPushes = @(Select-String -Path $Assembly -Pattern 'pushq %(rbx|rdi|rsi|r12|r13|r14|r15)')
    if ($SavedRegisterPushes.Count -gt 0) {
        $AssemblyLines = Get-Content $Assembly
        foreach ($match in $SavedRegisterPushes) {
            $start = [Math]::Max(0, $match.LineNumber - 5)
            $end = [Math]::Min($AssemblyLines.Count - 1, $match.LineNumber + 3)
            Write-Host "Unexpected callee-saved push near assembly line $($match.LineNumber):"
            for ($line = $start; $line -le $end; $line++) { Write-Host $AssemblyLines[$line] }
        }
        throw "A leaf scalar hot path still saves a callee-saved register."
    }
    Write-Host "VNT code generation: helper call inlined; proven-safe loop additions are unchecked."
    & gcc -O3 -flto $CSrc -o $CExe
    if ($LASTEXITCODE -ne 0) { throw "C benchmark compilation failed." }
    & rustc -C opt-level=3 -C lto=thin -C codegen-units=1 $RustSrc -o $RustExe
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
    Write-Host "All binaries use the same $Rounds x 1,000,000 loop workload. Timings include process startup."
}
finally { Pop-Location }

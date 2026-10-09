# VNT vs C vs Python microbenchmark

This benchmark repeatedly sums integers from 0 through 39,999 using nested
while loops. All three programs should print:

    799980000

This is a small, single-workload microbenchmark, not a general language ranking.
The C version uses volatile loop state to stop the optimizer from replacing
the loop with a closed-form result. Process startup and runtime overhead are
included in the timings. Use the same machine, close heavy background workloads,
and compare the median of several runs.

## Build

Run these commands from the repository root in PowerShell:

    .\build.ps1
    .\vnt.exe --compile benchmarks\loop_sum.vnt -o benchmarks\loop_sum_vnt.exe
    gcc -O2 benchmarks\loop_sum.c -o benchmarks\loop_sum_c.exe
    python --version

## Check correctness

    .\benchmarks\loop_sum_vnt.exe
    .\benchmarks\loop_sum_c.exe
    python benchmarks\loop_sum.py

All three should print 799980000.

## Measure execution time

Compilation is deliberately outside the timed region. Run each executable five
times and compare the median timings:

    1..5 | ForEach-Object {
        $t = Measure-Command { .\benchmarks\loop_sum_vnt.exe | Out-Null }
        "VNT  {0,8:N2} ms" -f $t.TotalMilliseconds
    }

    1..5 | ForEach-Object {
        $t = Measure-Command { .\benchmarks\loop_sum_c.exe | Out-Null }
        "C    {0,8:N2} ms" -f $t.TotalMilliseconds
    }

    1..5 | ForEach-Object {
        $t = Measure-Command { python benchmarks\loop_sum.py | Out-Null }
        "Python {0,8:N2} ms" -f $t.TotalMilliseconds
    }

These timings include launching each process (and the Python interpreter), so
very short runtimes can be dominated by startup cost. Treat the result as a
repeatable first benchmark, not a definitive statement about overall language
performance.

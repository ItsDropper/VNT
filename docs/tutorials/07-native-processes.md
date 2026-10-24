# Tutorial 7: Launching and Managing Child Processes

Native programs often need to call another program—for example, to run a compiler, inspect a tool's output, or launch a helper process. VNT exposes process functions through its native runtime and wraps them in `lib/std/process.vnt`.

This tutorial is written for Windows because its runnable example uses `cmd.exe`.

## 1. Start a process with explicit arguments

Create `process_demo.vnt` in the repository root:

```vnt
import "lib/std/process.vnt" as process

args = ["/c", "echo Hello from a child process"]
child = process.start("cmd.exe", args)

wait_result = process.wait(child, 5000)
print(wait_result)
print(process.stdout(child))
print(process.stderr(child))
print(process.exit_code(child))
```

Compile and run:

```powershell
.\vnt.exe --compile process_demo.vnt -o process_demo.exe
.\process_demo.exe
```

The process module wraps native runtime operations:

- `start(executable, args)` starts a child process.
- `wait(process, timeout_ms)` waits for completion or until its timeout behavior applies.
- `poll(process)` checks whether the process has completed.
- `stdout(process)` and `stderr(process)` retrieve captured output streams.
- `exit_code(process)` reads the exit code.
- `pid(process)` returns the process identifier.
- `terminate(process)` requests termination.

The process handle is an opaque runtime value. Treat it as a handle; do not try to inspect its internal representation.

## 2. Arguments are not shell command text

This distinction matters:

```vnt
args = ["/c", "echo one"]
child = process.start("cmd.exe", args)
```

VNT passes an executable and an array of arguments to the process API. It does not automatically interpret a single string as a shell command. In the example, `cmd.exe` itself interprets the argument after `/c`.

Prefer explicit executable paths and argument arrays when building tools. Do not concatenate untrusted input into shell command strings.

## 3. Check the exit code

A successful process usually reports exit code `0`; a non-zero value usually signals a failure. The exact meanings of non-zero codes depend on the program being run.

```vnt
import "lib/std/process.vnt" as process
import "lib/std/log.vnt" as log

args = ["/c", "echo build step finished"]
child = process.start("cmd.exe", args)
wait_result = process.wait(child, 5000)
code = process.exit_code(child)

if code == 0 {
    log.info("Child process completed successfully.")
} else {
    log.error("Child process failed with exit code " + code)
}
```

The standard logging module writes records to `vnt.log` and to the console. If you need robust timeout or error handling, inspect the module and runtime implementation for the exact return values before depending on them.

## 4. A practical debugging checklist

When a child process behaves unexpectedly, check these separately:

1. Does the executable exist and appear on `PATH`?
2. Are the arguments split correctly into array elements?
3. Did the process finish before the timeout?
4. What did stdout contain?
5. What did stderr contain?
6. What was the exit code?

Do not treat an empty stdout stream as proof of success. A program may write only to stderr or fail before producing output.

## Exercises

1. Print the child process PID before waiting.
2. Print whether `process.poll(child)` reports completion after waiting.
3. Change the child command to write a message to stderr.
4. Run a command that exits with a non-zero code and observe the exit code.
5. Add a log entry for the captured stdout and a warning for a non-zero exit code.

Next: [HTTP APIs and external tools](08-http-apis.md).

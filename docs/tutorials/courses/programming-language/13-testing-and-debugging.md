# 13 — Test Your Language

## Goal

Make tests repeatable and check behavior, not just whether the program starts.

## Step 1: create a test program

Create `vnx/tests/my-test.vnx`:

~~~text
say "TEST_START"
set result "ok"
when result == "ok" jump passed
say "TEST_FAILED"
end
label passed
say "TEST_PASSED"
end
~~~

Run it through the interpreter. Confirm that `TEST_PASSED` appears and `TEST_FAILED` does not.

## Step 2: automate input

For a program with no input after the source path, PowerShell can pipe the path:

~~~powershell
"vnx/tests/my-test.vnx" | .\vnx\build\vnx.exe
~~~

For the adventure, provide the path and each answer as separate lines:

~~~powershell
@("vnx/game/last_signal.vnx", "Ada", "1", "1", "3", "2", "1", "3", "5") | .\vnx\build\vnx.exe
~~~

On Linux, use `printf 'vnx/tests/my-test.vnx\n' | vnx/build/vnx` for a program with no further input.

## Step 3: inspect CI

Open [vnx-tests.yml](../../../../.github/workflows/vnx-tests.yml). It builds VNT, compiles the interpreter, checks a string-indexing regression, runs the smoke test, tests game endings, and checks invalid syntax. See the [GitHub Actions workflow](https://github.com/ItsDropper/VNT/actions/workflows/vnx-tests.yml) for run history.

## Debugging order

1. Can VNT compile the interpreter?
2. Can the executable read the VNX file?
3. Did the lexer emit the right token kinds?
4. Did the parser create the expected node?
5. Is the variable present at runtime?
6. Does the jump label exist?
7. Does output match the expected result?

## Exercise

Temporarily change `TEST_PASSED` to `TEST_BROKEN` and confirm a test that searches for the expected text would catch the regression. Restore it. Add a second test for an invalid program.

## Checkpoint

A passing build proves the source compiled. A passing behavior test proves a particular feature produced the expected result.

# 02 — Prepare VNT and Compile the Interpreter

## Goal

Compile the real VNT-native interpreter and run a VNX source file.

## Step 1: build the compiler

Windows, from the repository root:

~~~powershell
.\build.ps1
~~~

On Linux, follow the repository's documented compiler build process. The CI workflow shows the exact source files used: [vnx-tests.yml](../../../../.github/workflows/vnx-tests.yml).

## Step 2: compile the interpreter

~~~powershell
New-Item -ItemType Directory -Force vnx\build
.\vnt.exe --compile vnx\interpreter.vnt -o vnx\build\vnx.exe
~~~

This creates a native executable; it is not the game itself.

Linux:

~~~sh
mkdir -p vnx/build
./vnt --compile vnx/interpreter.vnt -o vnx/build/vnx
~~~

## Step 3: run the smoke program

~~~powershell
.\vnx\build\vnx.exe
~~~

Enter `vnx/tests/smoke.vnx` when prompted. Expect `VNX_SMOKE_OK` and `VNX_STATE_OK@.

## Step 4: find the entry point

Open [interpreter.vnt](../../../../vnx/interpreter.vnt). Find the prompt for a source path and then the calls to `lex(source)`, `parse()`, and `execute()`.

- `lex` scans source text and creates tokens.
- `parse` consumes tokens and creates instruction nodes.
- `execute` follows the nodes and changes runtime state.

The entry point checks for errors between phases. Do not execute a program if lexing or parsing failed.

## Exercise

Compile the same source to `vnx/build/vnx-debug.exe` and run it. This demonstrates that a source file and its compiled executable are different things.

## Troubleshooting

- Compiler rejects `interpreter.vnt`: VNT toolchain or VNT syntax issue.
- Interpreter cannot read the source path: check your working directory.
- Lexer/parser error: inspect the VNX source, not the VNT compiler command.

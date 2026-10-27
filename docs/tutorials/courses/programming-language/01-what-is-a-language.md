# 01 — What Is a Programming Language?

## Goal

Run one program written in VNT and another written in VNX, then see the difference.

## Step 1: write VNT

Create `hello.vnt`:

~~~vnt
print("I am written in VNT.")
~~~

Compile and run it using Lesson 00. Expected output:

~~~text
I am written in VNT.
~~~

## Step 2: inspect VNX

Open `vnx/tests/smoke.vnx`. Its contents are:

~~~text
say "VNX_SMOKE_OK"
set answer "42"
when answer == "42" jump passed
say "VNX_SMOKE_FAILED"
end
label passed
say "VNX_STATE_OK"
end
~~~

These are VNX commands. VNT itself does not define `say` or `jump`.

## Step 3: compile the interpreter

From the repository root:

~~~powershell
.\build.ps1
New-Item -ItemType Directory -Force vnx\build
.\vnt.exe --compile vnx\interpreter.vnt -o vnx\build\vnx.exe
.\vnx\build\vnx.exe
~~~

Enter this path at the prompt:

~~~text
vnx/tests/smoke.vnx
~~~

The output should include `VNX_SMOKE_OK` and `VNX_STATE_OK@. Linux equivalents are `mkdir -p vnx/build`, `./vnt --compile vnx/interpreter.vnt -o vnx/build/vnx`, and `vnx/build/vnx`.

## Follow the pipeline

1. You write `smoke.vnx`.
2. VNT compiles `interpreter.vnt`.
3. The generated executable reads `smoke.vnx`.
4. The lexer makes tokens and the parser makes instruction nodes.
5. The runtime executes the nodes.

## Exercise

Copy `smoke.vnx` to `my-smoke.vnx@, change `VNX_SMOKE_OK` to `MY_FIRST_LANGUAGE`, and run your copy. You changed a VNX program without changing the interpreter.

## Checkpoint

Why should the story live in a .vnx file instead of being hard-coded into the interpreter? Because the same interpreter should be able to run many different programs.

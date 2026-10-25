# 00 — Start Here: Make Something Run

## Goal

Compile and run a VNT program yourself. This is the toolchain you will use for the interpreter.

## Step 1: create the file

Create `lesson00.vnt` in the repository root:

~~~vnt
# Lesson 00: this is a comment
print("The compiler runs this program.")
name = "Ada"
print("Hello, " + name)
~~~

Save it.

## Step 2: compile and run

Windows PowerShell:

~~~powershell
.\build.ps1
.\vnt.exe --compile lesson00.vnt -o lesson00.exe
.\lesson00.exe
~~~

If you already built the compiler from the current source, you can skip the first command. Linux, after building the compiler:

~~~sh
./vnt --compile lesson00.vnt -o lesson00
./lesson00
~~~

Expected output:

~~~text
The compiler runs this program.
Hello, Ada
~~~

## Step 3: make a change

Change `Ada` to a different name, save, compile again, and run again. The new output should contain the changed name.

## The two source files

- `vnx/interpreter.vnt` is VNT source. VNT compiles it to an executable.
- `vnx/game/last_signal.vnx` is VNX source. The executable produced from the interpreter reads and runs it.

The VNT compiler does not directly understand VNX commands. The VNX interpreter does.

## Checkpoint

Which file is compiled by VNT? Which file is read by the interpreter? Explain the difference before continuing.

## Common problems

- **File not found:** run the commands from the repository root.
- **VNT syntax error:** compare your file to the four lines above.
- **Old output:** recompile after saving; the old executable cannot include edits you have not compiled.

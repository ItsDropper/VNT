# 02 — Prepare VNT

Use the repository's normal VNT build process. On Windows, build the compiler from the repository root with:

```powershell
.\build.ps1
```

Then compile a VNT program:

```powershell
.\vnt.exe --compile examples\global_variables.vnt -o example.exe
.\example.exe
```

On a supported Unix-like environment, use the repository's documented build process and invoke the resulting `vnt` compiler with the same `--compile source.vnt -o output` pattern.

## Start with a tiny implementation

Create a VNT file that prints a fixed message, compile it, and run it. Then change the message and repeat. This proves the toolchain works before you add parsing complexity.

```vnt
print("My interpreter is written in VNT.")
```

Do not make the interpreter depend on a second, unrelated implementation written in another language. Build automation may use shell or PowerShell to invoke tools, but the language implementation itself belongs in VNT source.

## Project organization

Keep different concerns separate:

- lexer code: characters and tokens;
- parser code: token order and syntax;
- runtime code: values and execution;
- example programs: source in your new language;
- tests: expected behavior;
- course and reference: explanations for humans.

Splitting files is useful only if the VNT module and compiler workflow can load them reliably. If imports are expanded as source, remember that this is not the same as isolated module compilation.

## Exercise

Compile and run a one-line VNT program. Record the exact command and expected output in your notes.

## Checkpoint

If the compiler fails, determine whether the error is in the VNT compiler invocation, the interpreter's VNT syntax, or the example language source. Those are three different failure layers.

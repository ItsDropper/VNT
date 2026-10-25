# VNX — a language implemented in VNT

VNX is a small line-oriented interpreted language. Its lexer, token stream, parser, AST, variable environment, label resolver, and execution loop are implemented in VNT in [interpreter.vnt](interpreter.vnt). [The Last Signal](game/last_signal.vnx) is a text adventure written in VNX.

## Build

From the repository root, build the VNT compiler using the project's normal toolchain, then compile the interpreter:

```powershell
.\build.ps1
.\vnt.exe --compile vnx\interpreter.vnt -o vnx\build\vnx.exe
.\vnx\build\vnx.exe
```

The interpreter asks for a source path; press Enter to use `vnx/game/last_signal.vnx`. Run it from the repository root so the default path resolves.

## VNX v0 syntax

Each command occupies one line. Blank lines and lines beginning with `#` are ignored.

| Form | Meaning |
|---|---|
| `say "text"` | Print text |
| `say variable` | Print a variable |
| `set name "value"` | Store a string |
| `input name "prompt"` | Read one line into a variable |
| `label name` | Define a jump target |
| `jump name` | Jump to a label |
| `when name == "value" jump label` | Conditional jump |
| `end` | Stop execution |

VNX v0 intentionally uses string values and label-based control flow. It does not yet claim numeric expressions, functions, arrays, or static typing. The interpreter reports lexer/parser errors, unknown variables and labels, and caps execution at 100,000 statements.

## Tests and course

The VNT-native interpreter is built and tested by the VNX GitHub Actions workflow. The complete beginner course is in [docs/tutorials/courses/programming-language/](../docs/tutorials/courses/programming-language/README.md).

# VNT Coding Tutorials

These tutorials teach the language by building small programs. They assume you have cloned the repository and built the compiler with `build.ps1` on Windows. Unless a tutorial says otherwise, save each program in the repository root so its relative import paths and build commands work as written.

## Learning path

1. [First program: values, variables, and output](01-first-program.md)
2. [Conditions, loops, and functions](02-control-flow-and-functions.md)
3. [Modules and text-file storage](03-modules-and-files.md)
4. [Build a small terminal journal](04-terminal-journal.md)
5. [Windows GUI basics](05-windows-gui.md)

## Compile and run a tutorial

For a source file called `lesson.vnt` in the repository root:

```powershell
.\vnt.exe --compile lesson.vnt -o lesson.exe
.\lesson.exe
```

The compiler also keeps the generated assembly beside the output executable. These tutorials describe the current implementation, not a promise that the syntax or runtime API will remain unchanged. GUI examples require Windows. If an example fails, first confirm the compiler is built from the current repository revision and run `.\tests\run_language_checks.ps1`.

# Tutorial 3: Modules and Text Files

VNT's source-level standard library is in `lib/std/`. A module is imported by a relative path and an alias. The compiler expands the imported source before parsing, so imports are convenient but are not yet isolated compilation units.

## 1. Import a module

Create `notes_demo.vnt` in the repository root:

```vnt
import "lib/std/fs.vnt" as fs

path = "tutorial_notes.txt"

if !fs.exists(path) {
    fs.write_text(path, "First note\n")
}

fs.append_text(path, "Second note\n")
print(fs.read_text(path))
```

The import path is relative to the source file that contains the import. This example assumes `notes_demo.vnt` is in the repository root.

The program uses these functions:

- `fs.exists(path)` checks whether the path exists.
- `fs.write_text(path, contents)` writes text to a file.
- `fs.append_text(path, contents)` appends text.
- `fs.read_text(path)` reads the file as text.

The functions are wrappers around native runtime operations. File APIs are for text data, not arbitrary binary files. Paths are interpreted relative to the running process's working directory unless they are absolute.

## 2. Compile and run

```powershell
.\vnt.exe --compile notes_demo.vnt -o notes_demo.exe
.\notes_demo.exe
```

Expected output:

```text
First note
Second note
```

Running the program again appends another `Second note` line because the file already exists. Delete `tutorial_notes.txt` if you want to repeat the first-run behavior.

## 3. Import other standard modules

Other source-level modules include:

- `lib/std/io.vnt` — line input, standard output, and standard error.
- `lib/std/math.vnt` — math wrappers and helpers such as `square` and `clamp`.
- `lib/std/time.vnt` — millisecond clock and sleep helpers.
- `lib/std/os.vnt` — current directory and environment variables.
- `lib/std/process.vnt` — native child-process management.
- `lib/std/net.vnt` — HTTP helpers that call the system `curl` executable.
- `lib/std/log.vnt` and `lib/std/config.vnt` — logging and plain-text configuration helpers.

Check the module source for the exact function names and return behavior before relying on a helper in a larger program. For example, the networking module requires `curl` on `PATH`; it is not a built-in HTTP implementation.

## Exercise

Change the program so it only creates the file when it does not exist, then reads and prints the file without appending. Verify that running it twice does not duplicate content.

Next: [Build a small terminal journal](04-terminal-journal.md).

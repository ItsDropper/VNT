# Tutorial 4: Build a Small Terminal Journal

This tutorial walks through the repository's `examples/vnt_desk.vnt`, a small terminal journal that stores entries in a plain-text log. Rather than duplicating the whole application here, use the example source as the complete reference implementation.

## What the application demonstrates

- A menu-driven terminal program.
- Functions and control flow.
- Reading user input through the I/O module.
- Writing and reading persistent text data.
- Inspecting runtime information such as the current directory and clock.
- Asking for confirmation before deleting journal data.

The journal writes `vnt_desk.log` in the process's current working directory. It does not upload entries. Keep in mind that deleting the log is destructive, so inspect the example's confirmation flow before changing its deletion behavior.

## 1. Read the source

Open [`examples/vnt_desk.vnt`](../../examples/vnt_desk.vnt). Find the import statements first, then identify:

1. The code that displays the menu.
2. The function that adds an entry.
3. The code that reads saved entries.
4. The code that removes the journal after confirmation.
5. The loop that keeps the application running.

Try tracing one menu selection from input to the function that handles it.

## 2. Compile and run

From the repository root on a system where the compiler is built:

```powershell
.\vnt.exe --compile examples\vnt_desk.vnt -o examples\vnt_desk.exe
.\examples\vnt_desk.exe
```

Run the executable from the directory where you want `vnt_desk.log` to be created, because the data path is relative to the current working directory. If the program cannot find a module, confirm that the import path is correct relative to `examples/vnt_desk.vnt`.

## 3. Make a small change

A safe first change is to update the menu text or add another informational menu option. Follow the existing input and branching style. Compile after each change and try both the new option and an existing one.

## Ideas for extending it

- Add a date/time label to each new entry using the time module.
- Add a menu option that prints the current working directory.
- Improve input validation for menu choices.
- Keep deletion confirmation and add a second explicit confirmation if you change the data-removal flow.

The example uses real runtime APIs. It is a small demonstration application, not a database-backed or multi-user journal.

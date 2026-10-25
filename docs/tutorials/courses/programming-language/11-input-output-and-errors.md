# 11 — Input, Output, and Diagnostics

A language becomes useful when programs can communicate with the user. Output and input should be implemented as explicit runtime operations, not special cases scattered through the parser.

## Output

Evaluate the output argument, convert the value to the language's display form, and write it. Specify whether output automatically adds a newline. Consistent output makes both programs and tests easier to read.

## Input

Input is usually text. If you later add numbers, decide how text is converted and what happens when the input is invalid. Do not pretend arbitrary text is an integer.

## Diagnostics

There are several kinds of failure:

- **Lexical error**: an unexpected character or unterminated string.
- **Syntax error**: tokens do not match the grammar.
- **Runtime error**: a program references an unknown variable or performs an invalid operation.
- **Toolchain error**: the VNT compiler cannot compile the interpreter itself.

Keep these layers distinct. A broken VNT interpreter build is not a syntax error in a program written in your new language.

## Error format

A useful diagnostic includes the file name, line number, error category, and a concise explanation. When possible, include the offending token or expected form.

## Exercise

Create one test for each error category. Confirm invalid programs return a failure status rather than pretending they succeeded.

## Checkpoint

Why should errors be reported at the layer that understands their meaning?

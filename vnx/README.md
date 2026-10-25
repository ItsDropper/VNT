# VNX — a language implemented in VNT

VNX is the learning language for the programming-language course. Its interpreter is intended to be implemented in **VNT source**, not C. The earlier C prototype was not the required architecture and is no longer used by the build.

The implementation migration is in progress. Do not treat the old C prototype's tests as proof that a VNT-native interpreter works. The VNT-native interpreter, executable game, and CI tests must land together before this project is considered runnable.

## Course

Start at [Build a Programming Language in VNT](../docs/tutorials/courses/programming-language/README.md). It is a multi-file course for complete beginners covering language design, lexing, ASTs, parsing, runtime values, control flow, a text adventure, testing, and extensions.

## Intended project layout

- `interpreter.vnt` — lexer, parser, AST, and runtime implemented in VNT.
- `game/last_signal.vnx` — the adventure written in VNX.
- `tests/` — deterministic tests for language behavior and game routes.
- `docs/tutorials/courses/programming-language/` — beginner course.

Until the VNT-native implementation and tests are complete, this directory is deliberately marked as unfinished rather than claiming the C prototype satisfies the requirement.

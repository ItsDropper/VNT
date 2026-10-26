# VBuild

VBuild is the first-party project driver for VNT. The initial implementation is
a small Python 3.11+ command-line tool using only the standard library; TOML
parsing uses `tomllib`. It delegates language semantics and code generation to
the existing VNT compiler rather than implementing a second compiler.

## Current setup

Build VNT using the repository's existing `build.ps1` or CI compiler command.
From a project directory, run the script with Python:

```sh
python /path/to/VNT/tools/vbuild.py init
python /path/to/VNT/tools/vbuild.py build
python /path/to/VNT/tools/vbuild.py check
python /path/to/VNT/tools/vbuild.py run
python /path/to/VNT/tools/vbuild.py clean
```

The current compiler is located in the VNT repository root by default. Set
`VNT_COMPILER` to override its executable path. Python 3.11 or newer is required.
VBuild currently runs the compiler from its source repository because the
compiler's native-link command references runtime source files relative to that
working directory. Source and output paths passed to the compiler are absolute,
so project directories may contain spaces.

## Manifest

VBuild reads `vbuild.toml` from the current project directory:

```toml
[project]
name = "hello"
version = "0.1.0"

[build]
sources = ["src/main.vnt"]
output = "hello"
target = "host"
```

The first release accepts one root source file. Imports are resolved by VNT's
existing source module loader; listing several independent source roots is not
yet supported because the compiler CLI accepts one source file per invocation.
The `host` target uses the host compiler/linker toolchain. Other targets are
rejected instead of being silently treated as supported.

## Commands

- `init [--name NAME]`: create a manifest and `src/main.vnt`; refuses to overwrite an existing manifest.
- `build`: validate configuration and sources, then invoke `vnt --compile`.
- `run`: build and execute the resulting host program.
- `check`: run the compiler's existing `--dump-ir` pipeline. This checks parsing, lowering, optimization, and HIR validation; it is not a separate full static-analysis mode.
- `clean`: remove only the project's generated `build/` directory.
- `test`: explicitly reports that test targets are not implemented yet. It does not claim to run tests.

Generated programs and assembly are placed under `build/`. VBuild does not
currently resolve dependencies, provide incremental builds, define profiles,
cross-compile, discover tests, or promise bit-for-bit reproducibility. Those
features should be added only after compiler invocation and project lifecycle
contracts are stable.

## Architecture and next steps

`tools/vbuild.py` owns CLI parsing, manifest validation, source-path checks,
process invocation, and generated-output cleanup. The compiler remains the
authority for parsing, type checking, module expansion, HIR validation, native
code generation, and linking.

Next milestones:

1. Move compiler runtime/toolchain discovery out of the current-working-directory assumption.
2. Add reliable integration tests for build/run/check on Linux and Windows.
3. Add explicit test-target configuration and real test execution.
4. Add debug/release profiles and target metadata once the compiler exposes supported target capabilities.
5. Add content-based incremental builds and dependency locking after dependency semantics are defined.

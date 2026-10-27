# VNT Standard Library and Runtime API Reference

This page is a navigation reference to the current implementation, not a promise of stable APIs. VNT is experimental; check the linked source when exact signatures or failure behavior matter.

## Build and execution

- **Compiler:** C implementation targeting native x86-64 assembly.
- **Linker/runtime:** GCC and VNT's native runtime.
- **Windows build:** `.\build.ps1` from the repository root; compile with `.\vnt.exe --compile source.vnt -o program.exe`.
- **Regression checks on Windows:** `.\tests\run_language_checks.ps1`.
- **Networking:** Windows uses WinHTTP; Unix-like builds use sockets and OpenSSL for HTTPS.

See [compiler architecture](compiler-architecture.md) for the pipeline and [tutorials](tutorials/README.md) for complete examples.

## Source-level standard library

The modules are implemented in ordinary VNT source using existing functions, loops, comparisons, indexing, and arithmetic; no compiler or runtime additions are needed. Import modules by relative path, for example:

```vnt
import "lib/std/fs.vnt" as fs
print(fs.exists("notes.txt"))
```

The import path is relative to the importing source file. Module functions are exported with `export fun`. The current module loader expands imported source before parsing; modules are not isolated compilation units and do not yet have private symbol isolation.

| Module | Main capabilities | Contract / limitation | Source |
| --- | --- | --- | --- |
| `io.vnt` | Read a line, write stdout/stderr | `read_line(prompt)` returns text; input is line-oriented | [io.vnt](../lib/std/io.vnt) |
| `fs.vnt` | Check paths, read/write/append text, atomic text replacement, line-oriented read/write/append, remove/rename paths, create/list directories | Text-oriented; not a binary-safe file API. Atomic replacement writes a sibling temporary file and renames it over the target; filesystem/platform guarantees vary. `read_lines` accepts LF/CRLF; `write_lines` emits LF and a trailing newline for non-empty input. Check boolean write results. `append_line` rewrites the file only when an existing file lacks a trailing newline | [fs.vnt](../lib/std/fs.vnt) |
| `math.vnt` | Math helpers and numeric wrappers | Use source for exact names and type behavior | [math.vnt](../lib/std/math.vnt) |
| `array.vnt` | Search/count, numeric aggregation, reverse, concatenate, take/drop, stable deduplication, join | `sum`/min/max/average expect numeric arrays; empty aggregates return 0; take/drop clamp counts to array bounds; unique preserves first occurrence order | [array.vnt](../lib/std/array.vnt) |
| `string.vnt` | Prefix/suffix/search, split, left/right, prefix/suffix removal, padding, replacement, trimming, line counting | Uses current runtime string indexing; no Unicode grapheme-cluster segmentation or normalization. `split` preserves trailing empty fields and returns `[value]` for an empty delimiter. Count/replace are non-overlapping; trim handles ASCII space/tab/CR/LF | [string.vnt](../lib/std/string.vnt) |
| `number.vnt` | Parity, sign, factorial, GCD, primality | `factorial(n)` returns -1 for negative input | [number.vnt](../lib/std/number.vnt) |
| `csv.vnt` | Parse CSV text into rows and serialize rows back to CSV | Supports quoted commas, doubled quote escaping, embedded newlines, LF/CRLF records; malformed quoting returns `[]`, which is indistinguishable from empty input | [csv.vnt](../lib/std/csv.vnt) |
| `path.vnt` | Portable separator normalization, basename, dirname, extension, stem, join, absolute-path detection, extension replacement | Lexical only; does not resolve `.`/`..`, symlinks, filesystem existence, or security-sensitive canonical paths; outputs `/` separators | [path.vnt](../lib/std/path.vnt) |
| `time.vnt` | Millisecond clock, sleep, duration conversions, elapsed/deadline helpers, Gregorian date utilities, and epoch-to-local/UTC calendar fields | Calendar helpers use the proleptic Gregorian calendar and years >= 1; weekday is 0=Sunday. Local conversion follows the host timezone configuration only; no arbitrary IANA zone selection or leap-second engine. | [time.vnt](../lib/std/time.vnt) |
| `os.vnt` | Current directory and environment variables | Environment variables may be absent or platform-specific | [os.vnt](../lib/std/os.vnt) |
| `process.vnt` | Start, poll, wait for, inspect, and terminate child processes; launch visible programs | Pass an executable and an array of string arguments. Arguments are not shell-parsed by VNT | [process.vnt](../lib/std/process.vnt) |
| `net.vnt` | HTTP/HTTPS, custom methods/headers, form encoding, status classifiers, binary request/response, downloads, async and stream helpers | Body helpers return an empty string on transport failure; status `000` means no HTTP status was received. Ordinary responses are buffered. `encode_form` requires equal-length key/value arrays. Download fallback routes may buffer | [net.vnt](../lib/std/net.vnt) |
| `log.vnt` | Console and file logging | Appends to `vnt.log` in the current working directory | [log.vnt](../lib/std/log.vnt) |
| `config.vnt` | Load-or-create, save/append, INI `get`, `get_bool`, `has_key`, and `section_exists` | Supports simple `[section]` headers, `key=value`, blank lines, `#`/`;` comments, and outer double quotes; no escape processing, mutation, or TOML/JSON support | [config.vnt](../lib/std/config.vnt) |
| `json.vnt` | Read a simple JSON field occurrence, fallback lookup, non-empty-value check | Still not a full JSON DOM/parser; empty-string fields are indistinguishable from missing fields in `value_or`/`has_nonempty_value` | [json.vnt](../lib/std/json.vnt) |
| `mem.vnt` | Raw pointer allocation and memory primitives | Requires explicit `unsafe { ... }`; no bounds/lifetime checking | [mem.vnt](../lib/std/mem.vnt) and [raw-memory guide](raw-memory.md) |

## Error handling conventions

VNT does not currently provide general-purpose exceptions or a standard `Result<T, E>` type. Use the return contract of each API:

- **Filesystem operations:** check boolean results from writes, appends, deletes, renames, and directory creation. A read returning an empty string can mean an empty file or a failed read; check `exists` when the distinction matters.
- **HTTP:** use `net.status(url)` or a status variant to distinguish an HTTP response from a transport failure. The result is a three-character string; `000` indicates no HTTP status was received. An empty body alone is ambiguous.
- **Child processes:** inspect completion, stdout, stderr, and exit code separately. An empty stdout stream is not proof of success.
- **Raw memory:** invalid pointer operations can terminate the program. Only use valid allocations and respect their size and lifetime.
- **Logs:** `log.info`, `log.warn`, and `log.error` append to a local log file and also write to the console or standard error.

These checks improve failure reporting but do not make every API failure recoverable. Consult the implementation before relying on undocumented edge cases.

## Native runtime built-ins

The compiler maps source-level helpers to native runtime functions. Current families include:

- **Filesystem:** `fs_exists`, `fs_read`, `fs_write`, `fs_append`, `fs_delete`, `fs_rename`, `fs_is_file`, `fs_is_dir`, `fs_list`, `dir_create`.
- **Environment and paths:** `cwd`, `env_get`, `env_set`.
- **Time:** `time_ms`, `sleep_ms`.
- **Processes:** `process_start`, `process_poll`, `process_wait`, `process_pid`, `process_terminate`, `process_stdout`, `process_stderr`, `process_exit_code`, `process_launch`.
- **Math:** `sqrt`, `sin`, `cos`, `tan`, `abs`, `floor`, `ceil`, `min`, `max`.
- **Networking:** `net_request`, `net_request_bytes`, `net_status`, `net_download`, `net_url_encode`, async and stream request operations.
- **Memory:** `mem_alloc`, `mem_free`, `mem_is_null`, `mem_realloc`, `mem_ptr_add`, `mem_read_u8`, `mem_read_u32`, `mem_write_u8`, `mem_write_u32`, `mem_copy`, `mem_set`.
- **GUI:** native GUI functions are Windows-only; inspect the [GUI demo](../examples/gui_demo.vnt) and the runtime for current signatures.

Prefer the source-level modules for application code. Direct runtime built-ins are lower-level interfaces and may change as the language evolves.

## Runnable examples and guides

- [Tutorial index](tutorials/README.md)
- [Complete local task tracker project](tutorials/09-build-a-project.md)
- [Arrays, structs, and references](tutorials/06-arrays-structs-references.md)
- [Child processes](tutorials/07-native-processes.md)
- [HTTP/HTTPS](tutorials/08-http-apis.md)
- [Raw pointers and unsafe operations](raw-memory.md)
- [GUI demo](../examples/gui_demo.vnt)
- [VNT Desk journal](../examples/vnt_desk.vnt)
- [Compiler architecture](compiler-architecture.md)

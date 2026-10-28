# VNT Semantic Checks and Regression Strategy

Copyright (c) 2026 ItsDropper

This guide describes the current semantic checks implemented by the VNT frontend. VNT is experimental; these rules describe the checked behavior, not a claim that the compiler proves every program safe.

## Compilation stages

The frontend parses source into an AST. The type checker validates declarations, expression types, function calls, return behavior, lexical scopes, and context-sensitive statements before the compiler lowers the program into HIR and generates native code.

A successful type check means the program passed the checks implemented here. It does not prove memory safety, termination, absence of runtime errors, or correctness of all backend paths.

## Loop-control rules

- `break` is legal only inside a loop body.
- `continue` is legal only inside a loop body.
- Both statements can appear under nested conditionals inside a loop.
- When loops are nested, either statement targets the innermost enclosing loop.
- A function body starts a new control-flow context. A function cannot break or continue a loop owned by its caller.
- A loop is not assumed to execute at least once when checking whether a typed function returns a value.

The parser currently lowers `for` loops to `while`-style AST nodes. The type checker therefore treats their bodies as loop bodies through the same loop-depth mechanism. Regression tests cover both ordinary and nested loop execution.

### Valid example

```vnt
for outer in range(1, 3) {
    for inner in range(1, 4) {
        if inner == 2 {
            continue
        }
        print(outer * 10 + inner)
    }
}
```

Output:

```text
11
13
21
23
```

### Invalid examples

Top-level loop control must be rejected:

```vnt
break
```

Conditional nesting does not establish a loop context:

```vnt
if true {
    continue
}
```

A function cannot target its caller's loop:

```vnt
fun invalid_control() {
    break
}
```

Each invalid case should fail compilation, and tests should check that a failed compile does not leave a usable output executable.

## Function return rules

For functions with an explicit non-void return type, the current checker requires a return on every statically recognized path. It recognizes direct returns, returns inside an unsafe block, and an `if` whose then and else branches both return. It intentionally does not treat a loop as guaranteed to execute or return.

This is a deliberately conservative analysis, not a full control-flow proof. Future work should use the control-flow graph to compute whether every reachable exit from a typed function returns a value, and should test early returns, unreachable statements, nested branches, and loops.

## Type compatibility

The checker validates explicit parameter and return annotations, initializer types, assignment compatibility, operators, indexing, and use of void-returning calls. Unknown return types and unknown or `void` parameter types are rejected; `void` is reserved for a function's no-value return contract. Duplicate parameter names are rejected so one declaration cannot silently shadow another parameter in the same signature. Calls to annotated functions are checked against parameter types, while numeric-to-numeric compatibility remains supported. Duplicate function definitions are rejected rather than silently binding calls to the first definition. Struct declarations also reject duplicate field names, preventing ambiguous member layouts, and duplicate struct names, preventing later declarations from silently resolving to the first layout. Numeric types are intentionally interoperable in the current language. Inferred variables may become dynamically typed when assignments disagree; explicit annotations remain stricter.

Unknown/dynamic types are permitted in selected contexts by design. Do not interpret that behavior as strict static typing.

## Regression locations

- `tests/loops.vnt`: basic `break` and `continue` runtime behavior.
- `tests/nested_loop_control.vnt`: nested loop behavior and innermost-loop targeting.
- `tests/typecheck_break_outside_loop.vnt`: rejects top-level `break`.
- `tests/typecheck_continue_outside_loop.vnt`: rejects top-level `continue`.
- `tests/typecheck_break_in_function_rejected.vnt`: rejects function-local invalid `break`.
- `tests/typecheck_continue_in_function_rejected.vnt`: rejects function-local invalid `continue`.
- `tests/typecheck_break_in_conditional_rejected.vnt`: rejects conditional nesting without a loop.
- `tests/typecheck_unknown_return_type.vnt`: rejects an undeclared return annotation.
- `tests/typecheck_void_parameter_type.vnt`: rejects `void` as a parameter type.
- `tests/typecheck_unknown_parameter_type.vnt`: rejects an undeclared parameter annotation.
- `tests/typecheck_duplicate_parameter_names.vnt`: rejects ambiguous duplicate parameter names.
- `tests/typecheck_argument_type_mismatch.vnt`: rejects a string passed to an `int`-annotated parameter.
- `tests/typecheck_nested_function_cannot_break_caller_loop.vnt`: verifies that a nested function cannot break the loop of its caller.
- `tests/typecheck_duplicate_struct_fields.vnt`: rejects repeated field names in one struct declaration.
- `tests/typecheck_duplicate_struct_definitions.vnt`: rejects a second declaration using an existing struct name.
- `tests/typecheck_duplicate_function_definitions.vnt`: rejects a second function definition using an existing function name.

The quick language workflow checks compile failures and verifies expected runtime output on Linux and Windows. The VNX workflow provides additional native-interpreter coverage. A workflow that is queued or in progress is not evidence of a passing test.

## Future compiler work

1. Preserve source spans through parsing, HIR lowering, and optimization so diagnostics can identify the exact source location.
2. Move control-flow guarantees toward CFG-based analysis instead of growing a separate collection of AST pattern checks.
3. Add table-driven positive and negative tests for every context-sensitive statement.
4. Keep hosted and freestanding backend support explicit, and test unsupported operations as compile-time diagnostics.
5. Treat runtime safety and optimizer correctness as separate obligations from frontend type checking.

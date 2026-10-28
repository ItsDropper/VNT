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

For functions with an explicit non-void return type, the current checker requires a return on every statically recognized path. It recognizes direct returns, returns inside an unsafe block, and an `if` whose then and else branches both return. It intentionally does not treat a loop as guaranteed to execute or return. A conditional branch that does not return is still valid when a later statement returns on every path that falls through the conditional; `tests/typed_function_fallthrough_return.vnt` protects this distinction.

This is a deliberately conservative analysis, not a full control-flow proof. Future work should use the control-flow graph to compute whether every reachable exit from a typed function returns a value, and should test early returns, unreachable statements, nested branches, and loops.

## Type compatibility

The checker validates explicit parameter and return annotations, initializer types, assignment compatibility, operators, indexing, and use of void-returning calls. Unknown return types and unknown or `void` parameter types are rejected; `void` is reserved for a function's no-value return contract. Duplicate parameter names are rejected so one declaration cannot silently shadow another parameter in the same signature. Calls to annotated functions are checked against parameter types, while numeric-to-numeric compatibility remains supported. Duplicate function definitions are rejected rather than silently binding calls to the first definition. Struct declarations also reject duplicate field names, preventing ambiguous member layouts, and duplicate struct names, preventing later declarations from silently resolving to the first layout. Numeric types are intentionally interoperable in the current language, including reassignment between `int`, `i64`, and `float`; symbol updates must use the same numeric compatibility predicate as initializers, calls, and returns. Inferred variables may become dynamically typed when assignments disagree; explicit annotations remain stricter for nonnumeric mismatches.

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
- `tests/typecheck_initializer_type_mismatch.vnt`: rejects a string initializer for an `int`-annotated variable.
- `tests/typecheck_function_arity_mismatch.vnt`: rejects calls that omit required function arguments.
- `tests/typecheck_builtin_arity_mismatch.vnt`: rejects calls with too many arguments for a builtin.
- `tests/typecheck_function_too_many_arguments.vnt`: rejects calls with excess user-function arguments.
- `tests/typecheck_builtin_too_few_arguments.vnt`: rejects calls missing required builtin arguments.
- `tests/typecheck_undeclared_function.vnt`: rejects calls to unknown function names.
- `tests/typecheck_return_type_mismatch.vnt`: rejects a string returned from an `int`-annotated function.
- `tests/typed_function_fallthrough_return.vnt`: accepts a partial conditional followed by a return covering the remaining path.
- `tests/typed_function_nested_fallthrough_return.vnt`: accepts a nested partial conditional when a later return covers every fallthrough path, and verifies all three runtime paths.
- `tests/typed_function_unsafe_partial_return.vnt`: rejects a typed function when an `unsafe` block contains a conditional return but can still fall through.
- `tests/typed_function_unsafe_branch_returns.vnt`: accepts and executes a typed function when both branches inside an `unsafe` block return values.
- `tests/typed_function_unsafe_return.vnt`: verifies a return nested inside an `unsafe` block satisfies the typed function return contract and executes correctly.
- `tests/typecheck_invalid_void_return.vnt`: rejects returning a value from a `void` function.
- `tests/typecheck_explicit_assignment_type_mismatch.vnt`: rejects assigning a string to an explicitly typed local variable with an assignment-specific expected-versus-actual diagnostic.
- `tests/typecheck_parameter_assignment_type_mismatch.vnt`: rejects assigning a string to an explicitly typed function parameter, protecting parameter bindings from incompatible reassignment.
- `tests/typecheck_explicit_assignment_numeric_compatible.vnt`: compiles and runs a valid integer reassignment to an explicitly typed local, guarding against over-rejecting compatible updates.
- `tests/typecheck_all_numeric_reassignment_compatible.vnt`: verifies `i64`-to-`float` and `float`-to-`int` reassignment follow the same numeric compatibility rule as initializers, calls, and returns.
- `tests/typecheck_void_argument_rejected.vnt`: rejects passing a void-returning call as a function argument.
- `tests/typecheck_void_unary_expression_rejected.vnt`: rejects using a void-returning call as a unary operand.
- `tests/typecheck_void_condition_rejected.vnt`: rejects using a void-returning call as an `if` condition.
- `tests/typecheck_void_while_condition_rejected.vnt`: rejects using a void-returning call as a `while` condition; both conditional loop forms emit a specific control-flow diagnostic.
- `tests/typecheck_void_array_element_rejected.vnt`: rejects storing a void-returning call in an array literal.
- `tests/typecheck_struct_constructor_arguments_rejected.vnt`: rejects arguments to the current zero-argument struct constructor syntax.
- `tests/typecheck_nested_function_cannot_break_caller_loop.vnt`: verifies that a nested function cannot break the loop of its caller.
- `tests/typecheck_duplicate_struct_fields.vnt`: rejects repeated field names in one struct declaration.
- `tests/typecheck_duplicate_struct_definitions.vnt`: rejects a second declaration using an existing struct name.
- `tests/typecheck_duplicate_function_definitions.vnt`: rejects a second function definition using an existing function name.
- `tests/typecheck_invalid_equality.vnt`: rejects equality between incompatible string and numeric values.
- `tests/typecheck_invalid_ordered_comparison.vnt`: rejects ordered comparisons between numbers and strings.
- `tests/typecheck_invalid_boolean_order.vnt`: rejects ordered comparisons on booleans.
- `tests/typecheck_scope_if_leak.vnt`: rejects references to variables declared only inside an `if` branch.
- `tests/typecheck_scope_while_leak.vnt`: rejects references to variables declared only inside a loop body.
- `tests/typecheck_read_before_assignment.vnt`: rejects reading a variable before it has been declared.
- `tests/typecheck_shadow_same_scope.vnt`: rejects redeclaring a variable in the same lexical scope.

The quick language workflow checks compile failures, verifies that failed compilation leaves no output executable, and asserts precise initializer, function-argument, and return-type diagnostics plus the callee and argument counts in function, builtin, and struct-constructor arity diagnostics. Plain identifier assignments are represented by the parser as variable-declaration AST nodes. The type checker now distinguishes an unannotated reassignment to an existing explicitly typed binding from a declaration initializer and reports the assignment target and assigned value types. It also verifies expected runtime output on Linux and Windows. The VNX workflow provides additional native-interpreter coverage. A workflow that is queued or in progress is not evidence of a passing test.

## Future compiler work

1. Preserve source spans through parsing, HIR lowering, and optimization so diagnostics can identify the exact source location.
2. Move control-flow guarantees toward CFG-based analysis instead of growing a separate collection of AST pattern checks.
3. Add table-driven positive and negative tests for every context-sensitive statement.
4. Keep hosted and freestanding backend support explicit, and test unsupported operations as compile-time diagnostics.
5. Treat runtime safety and optimizer correctness as separate obligations from frontend type checking.

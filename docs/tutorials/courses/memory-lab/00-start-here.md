# Start here

## What you will build

Build a reproducible memory lab

By the end, you should have a concrete change in the companion project, a reproducible test, and an explanation of why the implementation is correct.

Do not start by copying a large block of code. Read the relevant source, write down the invariant, then implement the smallest behavior that can be tested.

## 1. Mental model

The deliverable is an arena allocator plus an inspector, not a pointer-printing demo.

Think of the implementation as three separate responsibilities:

1. **Input contract:** what values and state may enter this operation?
2. **Decision:** which checks and transformations are needed?
3. **Commit:** what state or output may change only after validation succeeds?

Keep these responsibilities visible in the code. A compact function that mixes parsing, state changes, and printing is harder to test than a few focused helpers.

## 2. Repository reconnaissance

Before editing, inspect the actual implementation and compiler tests. VNT is experimental, so source code is the final authority when a lesson and compiler behavior differ.

1. Open `lib/std/mem.vnt` and `docs/raw-memory.md`.
2. Search for a call site that already uses the API successfully.
3. Read the declaration and the implementation, not only the function name.
4. Check whether the function is marked `unsafe` and whether the caller must opt in.
5. Record its parameter types, return type, and failure convention.
6. Find a test that exercises the same language feature.
7. Build the compiler using the root README instructions.
8. Compile an unchanged example before you make a change.
9. Save the exact compiler output if the baseline fails.
10. Do not silently invent a method because it would be convenient.

## 3. Define the invariant first

An invariant is a statement that must remain true before and after every public operation. It is stronger than saying that a demo appears to work.

For this lesson, start with: Arena state should satisfy: `0 <= cursor <= capacity`; successful reservations fit fully inside the live backing allocation; rejected reservations do not mutate state.

Write the invariant into a comment near the state it protects. Then list which operations can change that state and which checks each operation must perform.

Make failure atomic: validate all inputs and calculate all derived values in temporary variables before updating shared state.

If the operation can fail, name the failure cases explicitly. Do not use one generic fallback that hides whether the input was invalid, the resource was exhausted, or the feature is unsupported.

## 4. Implementation plan

Follow this order rather than implementing everything at once:

1. Create or open the relevant module under `examples/courses/memory-lab/`.
2. Add the function signature and document the contract in plain language.
3. Validate the simplest invalid input before touching memory or packet fields.
4. Add the smallest successful path.
5. Return a clear result that distinguishes success from failure.
6. Keep state changes at the end of the successful path.
7. Add a test for the first invalid case.
8. Add a test for the normal case.
9. Add an edge case that exercises the boundary.
10. Compile and run after each small change.
11. Inspect the diff and remove temporary debug output.
12. Only then integrate the function into the larger program.

### Why the order matters

Early validation keeps invalid data away from dangerous operations. A small function is easier to reason about because its preconditions and postconditions fit on one screen. Integration comes later, after the unit behavior is known.

## Run the current milestone

From the repository root, compile the multi-file example that exercises this course:

```powershell
.\\vnt.exe --compile examples\\courses\\memory-lab\\src\\main.vnt -o memory-lab.exe
.\\memory-lab.exe
```

Use the exact compiler output as evidence. The relevant test executables and their commands are listed in the companion project's README. Do not substitute the same standalone pointer demo for the lesson-specific change below.

## 6. Implement the feature in small steps

### Step A — establish the data

Identify the smallest state or input required. Avoid adding fields for hypothetical future features; each field creates more invariants to maintain.

Write down the meaning, units, and valid range of each value. For example, distinguish byte count from element count, cursor from payload length, and protocol length from buffer capacity.

### Step B — reject invalid input

Validate negative values, impossible lengths, unsupported modes, and resource limits before performing operations that rely on those values.

Do not assume that a value is valid because the current caller constructs it. Public module functions eventually get called from new places.

### Step C — calculate before committing

Use local variables for intermediate results. Check arithmetic overflow and bounds before storing a new cursor, changing a cache, or exposing parsed fields.

If any check fails, return the failure result and leave the prior state intact.

### Step D — perform the narrow operation

Call the smallest standard-library function that performs the needed primitive. Keep raw-memory access or byte-field extraction in one place rather than duplicating it across the project.

### Step E — verify the postcondition

After success, verify that the invariant still holds. In tests, inspect the actual output or bytes rather than only checking a success boolean.

## Worked implementation: Establish a reproducible baseline

Compile the actual multi-file example before editing; this confirms that compiler, imports, and native linker work together.

```powershell
.\\build.ps1
.\\vnt.exe --compile examples\\courses\\memory-lab\\src\\main.vnt -o memory-lab.exe
.\\memory-lab.exe
```

Run this again after every milestone. Save real compiler output; do not call a file tested merely because it exists.

## 7. Tests you must write

Do not consider this lesson complete until all of these categories have an explicit test:

- **Normal input:** the intended operation succeeds.
- **Minimum input:** the smallest valid value succeeds or is rejected according to the contract.
- **Boundary input:** exactly the last valid position, capacity, or length.
- **One past the boundary:** the operation rejects it safely.
- **Malformed input:** inconsistent fields or invalid arguments are rejected.
- **Failure path:** state and accounting remain unchanged after rejection.
- **Repeated operation:** behavior remains valid across multiple calls.
- **Integration path:** the feature works when called through the public module API.

Each test should have a stable name, explicit expected outcome, and a failure message that identifies the invariant that broke.

## 8. Debugging workflow

1. Reproduce the failure with one fixed input.
2. Record the input and the expected outcome.
3. Find the first point where actual state differs from expected state.
4. Inspect the caller and callee contracts.
5. Change one thing only.
6. Recompile and rerun the reproducer.
7. Run the earlier regression tests.
8. Keep the reproducer as a permanent test.

Do not debug native memory failures by dereferencing questionable pointers to see what happens. A crash-free execution does not establish that the access was valid.

## 9. Common mistakes and why they fail

- **Coding before reading the API:** names can hide important ownership and failure behavior.
- **Updating state early:** a later validation failure leaves the module internally inconsistent.
- **Assuming success from printed output:** logs can be misleading; assertions and exact expected values are stronger.
- **Using only happy-path tests:** most boundary bugs live on rejection paths.
- **Mixing policy with parsing:** the same parser becomes difficult to reuse and test.
- **Hiding unsupported cases:** callers may treat a partial implementation as complete.
- **Using raw addresses as identifiers:** addresses can change or be reused; offsets and explicit IDs are easier to inspect.

## 10. Integration into the project

Once the unit tests pass, call the new operation from the project entry point. Keep the entry point small: parse arguments or select a fixture, call the module, and print the report.

Keep diagnostics structured enough that tests can distinguish invalid input from resource exhaustion and unsupported functionality.

Update the example README with build commands, expected output, test commands, known limitations, and the exact point where a beginner should continue.

Do not claim that the full project works just because the current scaffold compiles. Every supported feature needs an end-to-end test.

## 11. Exercise — do not skip the reasoning

Set up a repeatable compile/run loop, record compiler revision, and make a known-good baseline before touching memory.

Then extend the exercise with one additional case that is not covered by the starter tests. Explain why it is important and what bug it would catch.

## 12. Review questions

Answer these in your own words before moving on:

1. What exact invariant does this operation preserve?
2. Which inputs are rejected, and how is rejection represented?
3. What is the last point where failure can occur before state changes?
4. Which standard-library behavior did you verify in source?
5. Which test proves the boundary is correct?
6. How would you reduce a failure to a deterministic reproducer?
7. What does this implementation explicitly not support?

## Completion checklist

- [ ] I inspected the implementation, not just the API name.
- [ ] I can explain every validation check.
- [ ] I added a successful test and at least two rejection/boundary tests.
- [ ] Failure leaves state unchanged.
- [ ] The example uses the module rather than duplicating its internals.
- [ ] I ran the relevant compiler and test commands and recorded their real result.
- [ ] I documented any unsupported behavior.

## Checkpoint

Explain the change to another beginner without saying only that it 'works'. Describe the invariant, the validation order, the success path, the failure path, and the test that would catch a regression.

**Why this matters:** A working baseline lets you tell a compiler regression from your own bug.

Continue only when you can explain both what the code does and why each dangerous or state-changing operation is safe.

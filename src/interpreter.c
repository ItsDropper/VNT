#include <vnt/interpreter.h>
#include "internal/interpreter_internal.h"

ExecutionResult execution_continue(void) {
    ExecutionResult result;

    result.signal = EXECUTION_NORMAL;
    result.value.type = VALUE_INVALID;

    return result;
}

ExecutionResult execution_return(
    Value value
) {
    ExecutionResult result;

    result.signal = EXECUTION_RETURN;
    result.value = value;

    return result;
}

ExecutionResult execution_break_signal(void) {
    ExecutionResult result;

    result.signal = EXECUTION_BREAK;
    result.value.type = VALUE_INVALID;

    return result;
}

ExecutionResult execution_continue_signal(void) {
    ExecutionResult result;

    result.signal = EXECUTION_CONTINUE;
    result.value.type = VALUE_INVALID;

    return result;
}

int interpreter_execute(
    AstNode *program,
    Environment *environment
) {
    execute_node(
        program,
        environment,
        0
    );

    if (environment->had_error) {
        return 1;
    }

    return 0;
}
#include "interpreter.h"
#include "interpreter/interpreter_internal.h"

ExecutionResult execution_continue(void) {
    ExecutionResult result;

    result.returned = 0;
    result.value.type = VALUE_INVALID;

    return result;
}

ExecutionResult execution_return(
    Value value
) {
    ExecutionResult result;

    result.returned = 1;
    result.value = value;

    return result;
}

void interpreter_execute(
    AstNode *program,
    Environment *environment
) {
    execute_node(
        program,
        environment
    );
}
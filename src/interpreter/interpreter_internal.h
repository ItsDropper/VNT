#ifndef VNT_INTERPRETER_INTERNAL_H
#define VNT_INTERPRETER_INTERNAL_H

#include "../interpreter.h"

typedef enum {
    EXECUTION_NORMAL,
    EXECUTION_RETURN,
    EXECUTION_BREAK,
    EXECUTION_CONTINUE
} ExecutionSignal;

typedef struct {
    ExecutionSignal signal;
    Value value;
} ExecutionResult;

ExecutionResult execution_continue(void);

ExecutionResult execution_return(
    Value value
);

ExecutionResult execution_break_signal(void);

ExecutionResult execution_continue_signal(void);

Value copy_value(
    Value *value
);

Value evaluate_expression(
    AstNode *expression,
    Environment *environment
);

Value execute_function_call(
    AstNode *call,
    Environment *environment
);

void print_value(
    Value *value
);

ExecutionResult execute_node(
    AstNode *node,
    Environment *environment,
    int loop_depth
);

#endif
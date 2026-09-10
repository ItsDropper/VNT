#ifndef VNT_INTERPRETER_INTERNAL_H
#define VNT_INTERPRETER_INTERNAL_H

#include "../interpreter.h"

typedef struct {
    int returned;
    Value value;
} ExecutionResult;

ExecutionResult execution_continue(void);

ExecutionResult execution_return(
    Value value
);

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
    Environment *environment
);

#endif
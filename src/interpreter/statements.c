#include "interpreter_internal.h"

#include <stdio.h>

static ExecutionResult execute_statement_list(
    AstNode *statement,
    Environment *environment,
    int loop_depth
) {
    while (statement != NULL) {
        ExecutionResult result =
            execute_node(
                statement,
                environment,
                loop_depth
            );

        if (result.signal != EXECUTION_NORMAL) {
            return result;
        }

        statement = statement->next;
    }

    return execution_continue();
}

/*
 * Resolves an expression that represents an array
 * to the actual Value stored in the environment.
 *
 * Examples:
 *
 * numbers
 * numbers[0]
 * matrix[1]
 * matrix[1][2]
 *
 * The important part is that this returns a pointer
 * to the real stored Value instead of a value_copy().
 */
static Value *resolve_array_target(
    AstNode *expression,
    Environment *environment
) {
    if (expression == NULL) {
        return NULL;
    }

    /*
     * Base case:
     *
     * numbers
     */
    if (expression->type == AST_VARIABLE) {
        Value *value =
            environment_get(
                environment,
                expression->variable.name
            );

        if (value == NULL) {
            printf(
                "Runtime error: undefined variable '%s'.\n",
                expression->variable.name
            );

            return NULL;
        }

        if (value->type != VALUE_ARRAY) {
            printf(
                "Runtime error: indexing requires an array.\n"
            );

            return NULL;
        }

        return value;
    }

    /*
     * Recursive case:
     *
     * matrix[0]
     *
     * First resolve matrix, then access element 0.
     */
    if (
        expression->type ==
        AST_INDEX_EXPRESSION
    ) {
        Value *array =
            resolve_array_target(
                expression->index_expression.array,
                environment
            );

        if (array == NULL) {
            return NULL;
        }

        Value index =
            evaluate_expression(
                expression->index_expression.index,
                environment
            );

        if (index.type == VALUE_INVALID) {
            return NULL;
        }

        if (index.type != VALUE_INTEGER) {
            value_free(&index);

            printf(
                "Runtime error: array index must be an integer.\n"
            );

            return NULL;
        }

        Value *item =
            value_array_get(
                array,
                index.integer
            );

        if (item == NULL) {
            printf(
                "Runtime error: array index %d out of bounds.\n",
                index.integer
            );

            value_free(&index);
            return NULL;
        }

        value_free(&index);

        if (item->type != VALUE_ARRAY) {
            printf(
                "Runtime error: indexing requires an array.\n"
            );

            return NULL;
        }

        return item;
    }

    printf(
        "Runtime error: invalid array target.\n"
    );

    return NULL;
}

/*
 * Assigns a value to either:
 *
 * x = value
 *
 * or:
 *
 * array[index] = value
 *
 * or nested:
 *
 * matrix[0][1] = value
 */
static int assign_to_target(
    AstNode *target,
    Value *value,
    Environment *environment
) {
    if (
        target == NULL ||
        value == NULL
    ) {
        return 0;
    }

    /*
     * Plain variable assignment.
     *
     * The parser currently represents this as
     * AST_VARIABLE_DECLARATION, so this case is mainly
     * here for completeness.
     */
    if (target->type == AST_VARIABLE) {
        if (
            !environment_define(
                environment,
                target->variable.name,
                *value
            )
        ) {
            return 0;
        }

        value->type = VALUE_INVALID;

        return 1;
    }

    /*
     * Indexed assignment.
     *
     * Example:
     *
     * numbers[1] = 42
     *
     * matrix[0][1] = 42
     */
    if (
        target->type ==
        AST_INDEX_EXPRESSION
    ) {
        AstNode *array_expression =
            target->index_expression.array;

        /*
         * Resolve the parent array to the actual stored
         * Value.
         */
        Value *array =
            resolve_array_target(
                array_expression,
                environment
            );

        if (array == NULL) {
            return 0;
        }

        Value index =
            evaluate_expression(
                target->index_expression.index,
                environment
            );

        if (index.type == VALUE_INVALID) {
            return 0;
        }

        if (index.type != VALUE_INTEGER) {
            value_free(&index);

            printf(
                "Runtime error: array index must be an integer.\n"
            );

            return 0;
        }

        /*
         * value_array_set() takes ownership of the supplied
         * Value, so we invalidate the caller's copy after
         * successful assignment.
         */
        if (
            !value_array_set(
                array,
                index.integer,
                *value
            )
        ) {
            printf(
                "Runtime error: array index %d out of bounds.\n",
                index.integer
            );

            value_free(&index);
            return 0;
        }

        value_free(&index);

        value->type = VALUE_INVALID;

        return 1;
    }

    printf(
        "Runtime error: invalid assignment target.\n"
    );

    return 0;
}

ExecutionResult execute_node(
    AstNode *node,
    Environment *environment,
    int loop_depth
) {
    if (node == NULL) {
        return execution_continue();
    }

    switch (node->type) {
        case AST_PROGRAM:
            return execute_statement_list(
                node->program.statements,
                environment,
                loop_depth
            );

        case AST_FUNCTION_DECLARATION:
            if (
                !environment_define_function(
                    environment,
                    node
                )
            ) {
                printf(
                    "Runtime error: could not register function '%s'.\n",
                    node->function_declaration.name
                );
            }

            return execution_continue();

        case AST_FUNCTION_CALL: {
            Value value =
                execute_function_call(
                    node,
                    environment
                );

            value_free(&value);

            return execution_continue();
        }

        case AST_RETURN_STATEMENT: {
            Value value =
                evaluate_expression(
                    node->return_statement.expression,
                    environment
                );

            if (value.type == VALUE_INVALID) {
                return execution_continue();
            }

            return execution_return(value);
        }

        case AST_BREAK_STATEMENT:
            if (loop_depth <= 0) {
                printf(
                    "Runtime error: 'break' outside loop.\n"
                );

                return execution_continue();
            }

            return execution_break_signal();

        case AST_CONTINUE_STATEMENT:
            if (loop_depth <= 0) {
                printf(
                    "Runtime error: 'continue' outside loop.\n"
                );

                return execution_continue();
            }

            return execution_continue_signal();

        case AST_VARIABLE_DECLARATION: {
            Value value =
                evaluate_expression(
                    node->variable_declaration.value,
                    environment
                );

            if (value.type == VALUE_INVALID) {
                return execution_continue();
            }

            if (
                !environment_define(
                    environment,
                    node->variable_declaration.name,
                    value
                )
            ) {
                value_free(&value);

                printf(
                    "Runtime error: could not store variable.\n"
                );
            }

            return execution_continue();
        }

        case AST_ASSIGNMENT: {
            Value value =
                evaluate_expression(
                    node->assignment.value,
                    environment
                );

            if (value.type == VALUE_INVALID) {
                return execution_continue();
            }

            if (
                !assign_to_target(
                    node->assignment.target,
                    &value,
                    environment
                )
            ) {
                value_free(&value);
            }

            return execution_continue();
        }

        case AST_PRINT_STATEMENT: {
            Value value =
                evaluate_expression(
                    node->print_statement.expression,
                    environment
                );

            print_value(&value);
            value_free(&value);

            return execution_continue();
        }

        case AST_IF_STATEMENT: {
            Value condition =
                evaluate_expression(
                    node->if_statement.condition,
                    environment
                );

            if (
                condition.type !=
                VALUE_BOOLEAN
            ) {
                value_free(&condition);

                printf(
                    "Runtime error: if condition must be boolean.\n"
                );

                return execution_continue();
            }

            ExecutionResult result =
                execution_continue();

            if (condition.boolean) {
                result =
                    execute_statement_list(
                        node->if_statement.then_branch,
                        environment,
                        loop_depth
                    );
            } else if (
                node->if_statement.else_branch != NULL
            ) {
                result =
                    execute_statement_list(
                        node->if_statement.else_branch,
                        environment,
                        loop_depth
                    );
            }

            value_free(&condition);

            return result;
        }

        case AST_WHILE_STATEMENT: {
            for (;;) {
                Value condition =
                    evaluate_expression(
                        node->while_statement.condition,
                        environment
                    );

                if (
                    condition.type !=
                    VALUE_BOOLEAN
                ) {
                    value_free(&condition);

                    printf(
                        "Runtime error: while condition must be boolean.\n"
                    );

                    return execution_continue();
                }

                if (!condition.boolean) {
                    value_free(&condition);
                    break;
                }

                value_free(&condition);

                ExecutionResult result =
                    execute_statement_list(
                        node->while_statement.body,
                        environment,
                        loop_depth + 1
                    );

                /*
                 * Return must escape the loop.
                 */
                if (
                    result.signal ==
                    EXECUTION_RETURN
                ) {
                    return result;
                }

                /*
                 * Break is consumed by this loop.
                 */
                if (
                    result.signal ==
                    EXECUTION_BREAK
                ) {
                    break;
                }

                /*
                 * Continue is consumed by this loop
                 * and starts the next iteration.
                 */
                if (
                    result.signal ==
                    EXECUTION_CONTINUE
                ) {
                    continue;
                }
            }

            return execution_continue();
        }

        /*
         * Expressions do not execute as statements.
         */
        case AST_STRING_LITERAL:
        case AST_INTEGER_LITERAL:
        case AST_BOOLEAN_LITERAL:
        case AST_ARRAY_LITERAL:
        case AST_VARIABLE:
        case AST_INDEX_EXPRESSION:
        case AST_BINARY_EXPRESSION:
        case AST_UNARY_EXPRESSION:
            return execution_continue();
    }

    return execution_continue();
}
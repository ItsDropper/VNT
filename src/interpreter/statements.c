#include "interpreter_internal.h"

#include <stdio.h>

ExecutionResult execute_node(
    AstNode *node,
    Environment *environment
) {
    if (node == NULL) {
        return execution_continue();
    }

    switch (node->type) {
        case AST_PROGRAM: {
            AstNode *statement =
                node->program.statements;

            while (statement != NULL) {
                ExecutionResult result =
                    execute_node(
                        statement,
                        environment
                    );

                if (result.returned) {
                    return result;
                }

                statement = statement->next;
            }

            return execution_continue();
        }

        case AST_FUNCTION_DECLARATION:
            if (!environment_define_function(
                    environment,
                    node
                )) {
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

        case AST_VARIABLE_DECLARATION: {
            Value value =
                evaluate_expression(
                    node->variable_declaration.value,
                    environment
                );

            if (value.type == VALUE_INVALID) {
                return execution_continue();
            }

            if (!environment_define(
                    environment,
                    node->variable_declaration.name,
                    value
                )) {
                value_free(&value);

                printf(
                    "Runtime error: could not store variable.\n"
                );
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

            if (condition.type != VALUE_BOOLEAN) {
                value_free(&condition);

                printf(
                    "Runtime error: if condition must be boolean.\n"
                );

                return execution_continue();
            }

            if (condition.boolean) {
                AstNode *statement =
                    node->if_statement.then_branch;

                while (statement != NULL) {
                    ExecutionResult result =
                        execute_node(
                            statement,
                            environment
                        );

                    if (result.returned) {
                        value_free(&condition);
                        return result;
                    }

                    statement = statement->next;
                }
            } else if (
                node->if_statement.else_branch != NULL
            ) {
                AstNode *statement =
                    node->if_statement.else_branch;

                while (statement != NULL) {
                    ExecutionResult result =
                        execute_node(
                            statement,
                            environment
                        );

                    if (result.returned) {
                        value_free(&condition);
                        return result;
                    }

                    statement = statement->next;
                }
            }

            value_free(&condition);

            return execution_continue();
        }

        case AST_WHILE_STATEMENT: {
            for (;;) {
                Value condition =
                    evaluate_expression(
                        node->while_statement.condition,
                        environment
                    );

                if (condition.type != VALUE_BOOLEAN) {
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

                AstNode *statement =
                    node->while_statement.body;

                while (statement != NULL) {
                    ExecutionResult result =
                        execute_node(
                            statement,
                            environment
                        );

                    if (result.returned) {
                        return result;
                    }

                    statement = statement->next;
                }
            }

            return execution_continue();
        }

        case AST_STRING_LITERAL:
        case AST_INTEGER_LITERAL:
        case AST_VARIABLE:
        case AST_BINARY_EXPRESSION:
            return execution_continue();
    }

    return execution_continue();
}
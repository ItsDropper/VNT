#include "interpreter.h"

#include <stdio.h>

static Value evaluate_expression(
    AstNode *expression,
    Environment *environment
) {
    if (expression == NULL) {
        Value invalid = {0};
        return invalid;
    }

    switch (expression->type) {
        case AST_STRING_LITERAL:
            return value_string(
                expression->string_literal.value
            );

        case AST_INTEGER_LITERAL:
            return value_integer(
                expression->integer_literal.value
            );

        case AST_VARIABLE: {
            Value *value = environment_get(
                environment,
                expression->variable.name
            );

            if (value == NULL) {
                printf(
                    "Runtime error: undefined variable '%s'.\n",
                    expression->variable.name
                );

                Value invalid = {0};
                return invalid;
            }

            if (value->type == VALUE_STRING) {
                return value_string(value->string);
            }

            if (value->type == VALUE_INTEGER) {
                return value_integer(value->integer);
            }

            if (value->type == VALUE_BOOLEAN) {
                return value_boolean(value->boolean);
            }

            Value invalid = {0};
            return invalid;
        }

        case AST_BINARY_EXPRESSION: {
            Value left = evaluate_expression(
                expression->binary_expression.left,
                environment
            );

            Value right = evaluate_expression(
                expression->binary_expression.right,
                environment
            );

            BinaryOperator operator =
                expression->binary_expression.operator;

            int is_comparison =
                operator == BINARY_EQUAL ||
                operator == BINARY_NOT_EQUAL ||
                operator == BINARY_GREATER ||
                operator == BINARY_LESS ||
                operator == BINARY_GREATER_EQUAL ||
                operator == BINARY_LESS_EQUAL;

            if (is_comparison) {
                if (
                    left.type != VALUE_INTEGER ||
                    right.type != VALUE_INTEGER
                ) {
                    value_free(&left);
                    value_free(&right);

                    printf(
                        "Runtime error: comparisons require integers.\n"
                    );

                    Value invalid = {0};
                    return invalid;
                }

                int result = 0;

                switch (operator) {
                    case BINARY_EQUAL:
                        result = left.integer == right.integer;
                        break;

                    case BINARY_NOT_EQUAL:
                        result = left.integer != right.integer;
                        break;

                    case BINARY_GREATER:
                        result = left.integer > right.integer;
                        break;

                    case BINARY_LESS:
                        result = left.integer < right.integer;
                        break;

                    case BINARY_GREATER_EQUAL:
                        result = left.integer >= right.integer;
                        break;

                    case BINARY_LESS_EQUAL:
                        result = left.integer <= right.integer;
                        break;

                    default:
                        break;
                }

                value_free(&left);
                value_free(&right);

                return value_boolean(result);
            }

            if (
                left.type != VALUE_INTEGER ||
                right.type != VALUE_INTEGER
            ) {
                value_free(&left);
                value_free(&right);

                printf(
                    "Runtime error: arithmetic requires integers.\n"
                );

                Value invalid = {0};
                return invalid;
            }

            int result = 0;

            switch (operator) {
                case BINARY_ADD:
                    result = left.integer + right.integer;
                    break;

                case BINARY_SUBTRACT:
                    result = left.integer - right.integer;
                    break;

                case BINARY_MULTIPLY:
                    result = left.integer * right.integer;
                    break;

                case BINARY_DIVIDE:
                    if (right.integer == 0) {
                        printf(
                            "Runtime error: division by zero.\n"
                        );

                        value_free(&left);
                        value_free(&right);

                        Value invalid = {0};
                        return invalid;
                    }

                    result = left.integer / right.integer;
                    break;

                default:
                    break;
            }

            value_free(&left);
            value_free(&right);

            return value_integer(result);
        }

        default:
            printf("Runtime error: invalid expression.\n");

            Value invalid = {0};
            return invalid;
    }
}

static void print_value(Value *value) {
    if (value == NULL) {
        return;
    }

    switch (value->type) {
        case VALUE_INVALID:
            break;

        case VALUE_STRING:
            printf("%s\n", value->string);
            break;

        case VALUE_INTEGER:
            printf("%d\n", value->integer);
            break;

        case VALUE_BOOLEAN:
            printf(
                "%s\n",
                value->boolean ? "true" : "false"
            );
            break;
    }
}

static void execute_node(
    AstNode *node,
    Environment *environment
) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_PROGRAM: {
            AstNode *statement = node->program.statements;

            while (statement != NULL) {
                execute_node(statement, environment);
                statement = statement->next;
            }

            break;
        }

        case AST_VARIABLE_DECLARATION: {
            Value value = evaluate_expression(
                node->variable_declaration.value,
                environment
            );

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

            break;
        }

        case AST_PRINT_STATEMENT: {
            Value value = evaluate_expression(
                node->print_statement.expression,
                environment
            );

            print_value(&value);
            value_free(&value);

            break;
        }

        case AST_IF_STATEMENT: {
            Value condition = evaluate_expression(
                node->if_statement.condition,
                environment
            );

            if (condition.type != VALUE_BOOLEAN) {
                value_free(&condition);

                printf(
                    "Runtime error: if condition must be boolean.\n"
                );

                break;
            }

            if (condition.boolean) {
                AstNode *statement =
                    node->if_statement.then_branch;

                while (statement != NULL) {
                    execute_node(statement, environment);
                    statement = statement->next;
                }
            } else if (node->if_statement.else_branch != NULL) {
                AstNode *statement =
                    node->if_statement.else_branch;

                while (statement != NULL) {
                    execute_node(statement, environment);
                    statement = statement->next;
                }
            }

            value_free(&condition);
            break;
        }

        case AST_STRING_LITERAL:
        case AST_INTEGER_LITERAL:
        case AST_VARIABLE:
        case AST_BINARY_EXPRESSION:
            break;
    }
}

void interpreter_execute(
    AstNode *program,
    Environment *environment
) {
    execute_node(program, environment);
}
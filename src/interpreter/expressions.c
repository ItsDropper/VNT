#include "interpreter_internal.h"

#include <stdio.h>

Value copy_value(Value *value) {
    if (value == NULL) {
        Value invalid = {0};
        return invalid;
    }

    switch (value->type) {
        case VALUE_STRING:
            return value_string(value->string);

        case VALUE_INTEGER:
            return value_integer(value->integer);

        case VALUE_BOOLEAN:
            return value_boolean(value->boolean);

        default: {
            Value invalid = {0};
            return invalid;
        }
    }
}

Value evaluate_expression(
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

            return copy_value(value);
        }

        case AST_FUNCTION_CALL:
            return execute_function_call(
                expression,
                environment
            );

        case AST_BINARY_EXPRESSION: {
            Value left = evaluate_expression(
                expression->binary_expression.left,
                environment
            );

            if (left.type == VALUE_INVALID) {
                return left;
            }

            Value right = evaluate_expression(
                expression->binary_expression.right,
                environment
            );

            if (right.type == VALUE_INVALID) {
                value_free(&left);
                return right;
            }

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
                        result =
                            left.integer == right.integer;
                        break;

                    case BINARY_NOT_EQUAL:
                        result =
                            left.integer != right.integer;
                        break;

                    case BINARY_GREATER:
                        result =
                            left.integer > right.integer;
                        break;

                    case BINARY_LESS:
                        result =
                            left.integer < right.integer;
                        break;

                    case BINARY_GREATER_EQUAL:
                        result =
                            left.integer >= right.integer;
                        break;

                    case BINARY_LESS_EQUAL:
                        result =
                            left.integer <= right.integer;
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
                    result =
                        left.integer + right.integer;
                    break;

                case BINARY_SUBTRACT:
                    result =
                        left.integer - right.integer;
                    break;

                case BINARY_MULTIPLY:
                    result =
                        left.integer * right.integer;
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

                    result =
                        left.integer / right.integer;
                    break;

                default:
                    break;
            }

            value_free(&left);
            value_free(&right);

            return value_integer(result);
        }

        default:
            printf(
                "Runtime error: invalid expression.\n"
            );

            {
                Value invalid = {0};
                return invalid;
            }
    }
}

void print_value(Value *value) {
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
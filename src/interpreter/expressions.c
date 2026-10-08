#include "../internal/interpreter_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

Value invalid_value(void) {
    Value invalid = {0};
    return invalid;
}

Value evaluate_expression(
    AstNode *expression,
    Environment *environment
) {
    if (expression == NULL) {
        return invalid_value();
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

        case AST_BOOLEAN_LITERAL:
            return value_boolean(
                expression->boolean_literal.value
            );

        case AST_ARRAY_LITERAL: {
            Value array =
                value_array();

            AstNode *element =
                expression->array_literal.elements;

            for (
                int i = 0;
                i < expression->array_literal.element_count;
                i++
            ) {
                if (element == NULL) {
                    value_free(&array);

                    printf(
                        "Runtime error: invalid array.\n"
                    );

                    return invalid_value();
                }

                Value item =
                    evaluate_expression(
                        element,
                        environment
                    );

                if (item.type == VALUE_INVALID) {
                    value_free(&array);
                    return item;
                }

                if (
                    !value_array_append(
                        &array,
                        item
                    )
                ) {
                    value_free(&item);
                    value_free(&array);

                    printf(
                        "Runtime error: could not append array element.\n"
                    );

                    return invalid_value();
                }

                element = element->next;
            }

            return array;
        }

        case AST_INDEX_EXPRESSION:
            return evaluate_index_expression(
                expression,
                environment
            );

        case AST_VARIABLE: {
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

                return invalid_value();
            }

            return copy_value(value);
        }

        case AST_FUNCTION_CALL:
            return execute_function_call(
                expression,
                environment
            );

        case AST_UNARY_EXPRESSION: {
            Value operand =
                evaluate_expression(
                    expression->unary_expression.operand,
                    environment
                );

            if (operand.type == VALUE_INVALID) {
                return operand;
            }

            if (
                expression->unary_expression.operator ==
                UNARY_NOT
            ) {
                if (operand.type != VALUE_BOOLEAN) {
                    value_free(&operand);

                    printf(
                        "Runtime error: '!' requires a boolean.\n"
                    );

                    return invalid_value();
                }

                int result =
                    !operand.boolean;

                value_free(&operand);

                return value_boolean(result);
            }

            if (
                expression->unary_expression.operator ==
                UNARY_NEGATE
            ) {
                if (operand.type != VALUE_INTEGER) {
                    value_free(&operand);

                    printf(
                        "Runtime error: unary '-' requires an integer.\n"
                    );

                    return invalid_value();
                }

                if (operand.integer == INT_MIN) {
                    value_free(&operand);

                    printf(
                        "Runtime error: integer overflow in unary '-'.\n"
                    );

                    return invalid_value();
                }

                int result =
                    -operand.integer;

                value_free(&operand);

                return value_integer(result);
            }

            value_free(&operand);

            printf(
                "Runtime error: invalid unary operator.\n"
            );

            return invalid_value();
        }

        case AST_BINARY_EXPRESSION: {
            BinaryOperator operator =
                expression->binary_expression.operator;

            if (
                operator == BINARY_AND ||
                operator == BINARY_OR
            ) {
                return evaluate_logical_expression(
                    expression,
                    environment
                );
            }

            Value left =
                evaluate_expression(
                    expression->binary_expression.left,
                    environment
                );

            if (left.type == VALUE_INVALID) {
                return left;
            }

            Value right =
                evaluate_expression(
                    expression->binary_expression.right,
                    environment
                );

            if (right.type == VALUE_INVALID) {
                value_free(&left);
                return right;
            }

            int is_equality =
                operator == BINARY_EQUAL ||
                operator == BINARY_NOT_EQUAL;

            int is_order_comparison =
                operator == BINARY_GREATER ||
                operator == BINARY_LESS ||
                operator == BINARY_GREATER_EQUAL ||
                operator == BINARY_LESS_EQUAL;

            if (is_equality) {
                int equal =
                    values_equal(
                        &left,
                        &right
                    );

                value_free(&left);
                value_free(&right);

                return value_boolean(
                    operator == BINARY_EQUAL
                        ? equal
                        : !equal
                );
            }

            if (is_order_comparison) {
                if (
                    left.type != VALUE_INTEGER ||
                    right.type != VALUE_INTEGER
                ) {
                    value_free(&left);
                    value_free(&right);

                    printf(
                        "Runtime error: ordered comparisons require integers.\n"
                    );

                    return invalid_value();
                }

                int result = 0;

                switch (operator) {
                    case BINARY_GREATER:
                        result =
                            left.integer >
                            right.integer;
                        break;

                    case BINARY_LESS:
                        result =
                            left.integer <
                            right.integer;
                        break;

                    case BINARY_GREATER_EQUAL:
                        result =
                            left.integer >=
                            right.integer;
                        break;

                    case BINARY_LESS_EQUAL:
                        result =
                            left.integer <=
                            right.integer;
                        break;

                    default:
                        break;
                }

                value_free(&left);
                value_free(&right);

                return value_boolean(result);
            }

            if (
                operator == BINARY_ADD &&
                left.type == VALUE_STRING &&
                right.type == VALUE_STRING
            ) {
                const char *left_string =
                    left.string != NULL
                        ? left.string
                        : "";

                const char *right_string =
                    right.string != NULL
                        ? right.string
                        : "";

                size_t left_length =
                    strlen(left_string);

                size_t right_length =
                    strlen(right_string);

                char *combined =
                    malloc(
                        left_length +
                        right_length +
                        1
                    );

                if (combined == NULL) {
                    value_free(&left);
                    value_free(&right);

                    printf(
                        "Runtime error: out of memory while concatenating strings.\n"
                    );

                    return invalid_value();
                }

                memcpy(
                    combined,
                    left_string,
                    left_length
                );

                memcpy(
                    combined + left_length,
                    right_string,
                    right_length
                );

                combined[
                    left_length +
                    right_length
                ] = '\0';

                Value result =
                    value_string(combined);

                free(combined);

                value_free(&left);
                value_free(&right);

                return result;
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

                return invalid_value();
            }

            int result = 0;

            switch (operator) {
                case BINARY_ADD:
                    result =
                        left.integer +
                        right.integer;
                    break;

                case BINARY_SUBTRACT:
                    result =
                        left.integer -
                        right.integer;
                    break;

                case BINARY_MULTIPLY:
                    result =
                        left.integer *
                        right.integer;
                    break;

                case BINARY_DIVIDE:
                    if (right.integer == 0) {
                        printf(
                            "Runtime error: division by zero.\n"
                        );

                        value_free(&left);
                        value_free(&right);

                        return invalid_value();
                    }

                    result =
                        left.integer /
                        right.integer;
                    break;

                case BINARY_MODULO:
                    if (right.integer == 0) {
                        printf(
                            "Runtime error: modulo by zero.\n"
                        );

                        value_free(&left);
                        value_free(&right);

                        return invalid_value();
                    }

                    /*
                    * INT_MIN % -1 is undefined in C because
                    * the corresponding division overflows.
                    */
                    if (
                        left.integer == INT_MIN &&
                        right.integer == -1
                    ) {
                        printf(
                            "Runtime error: modulo overflow.\n"
                        );

                        value_free(&left);
                        value_free(&right);

                        return invalid_value();
                    }

                    result =
                        left.integer %
                            right.integer;
                    break;

                default:
                    value_free(&left);
                    value_free(&right);

                    printf(
                        "Runtime error: invalid binary operator.\n"
                    );

                    return invalid_value();
            }

            value_free(&left);
            value_free(&right);

            return value_integer(result);
        }

        default:
            printf(
                "Runtime error: invalid expression.\n"
            );

            return invalid_value();
    }
}

void print_value(Value *value) {
    if (value == NULL) {
        return;
    }

    print_value_internal(value);
    printf("\n");
}
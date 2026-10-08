#include "../internal/interpreter_internal.h"
#include <stdio.h>

static Value evaluate_logical_expression(
    AstNode *expression,
    Environment *environment
) {
    BinaryOperator operator =
        expression->binary_expression.operator;

    Value left =
        evaluate_expression(
            expression->binary_expression.left,
            environment
        );

    if (left.type == VALUE_INVALID) {
        return left;
    }

    if (left.type != VALUE_BOOLEAN) {
        value_free(&left);

        printf(
            "Runtime error: logical operators require booleans.\n"
        );

        return invalid_value();
    }

    /*
     * Short-circuit AND:
     * false && anything == false
     */
    if (
        operator == BINARY_AND &&
        !left.boolean
    ) {
        value_free(&left);
        return value_boolean(0);
    }

    /*
     * Short-circuit OR:
     * true || anything == true
     */
    if (
        operator == BINARY_OR &&
        left.boolean
    ) {
        value_free(&left);
        return value_boolean(1);
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

    if (right.type != VALUE_BOOLEAN) {
        value_free(&left);
        value_free(&right);

        printf(
            "Runtime error: logical operators require booleans.\n"
        );

        return invalid_value();
    }

    int result;

    if (operator == BINARY_AND) {
        result =
            left.boolean &&
            right.boolean;
    } else {
        result =
            left.boolean ||
            right.boolean;
    }

    value_free(&left);
    value_free(&right);

    return value_boolean(result);
}


#include "../internal/interpreter_internal.h"
#include <stdio.h>

static Value evaluate_index_expression(
    AstNode *expression,
    Environment *environment
) {
    Value array =
        evaluate_expression(
            expression->index_expression.array,
            environment
        );

    if (array.type == VALUE_INVALID) {
        return array;
    }

    if (array.type != VALUE_ARRAY) {
        value_free(&array);

        printf(
            "Runtime error: indexing requires an array.\n"
        );

        return invalid_value();
    }

    Value index =
        evaluate_expression(
            expression->index_expression.index,
            environment
        );

    if (index.type == VALUE_INVALID) {
        value_free(&array);
        return index;
    }

    if (index.type != VALUE_INTEGER) {
        value_free(&array);
        value_free(&index);

        printf(
            "Runtime error: array index must be an integer.\n"
        );

        return invalid_value();
    }

    Value *item =
        value_array_get(
            &array,
            index.integer
        );

    if (item == NULL) {
        printf(
            "Runtime error: array index %d out of bounds.\n",
            index.integer
        );

        value_free(&array);
        value_free(&index);

        return invalid_value();
    }

    Value result =
        value_copy(item);

    value_free(&array);
    value_free(&index);

    return result;
}


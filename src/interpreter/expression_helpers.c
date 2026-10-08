#include "../internal/interpreter_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Value copy_value(Value *value) {
    return value_copy(value);
}

Value invalid_value(void) {
    Value invalid = {0};
    return invalid;
}

int values_equal(
    Value *left,
    Value *right
) {
    if (
        left == NULL ||
        right == NULL ||
        left->type != right->type
    ) {
        return 0;
    }

    switch (left->type) {
        case VALUE_INTEGER:
            return left->integer == right->integer;

        case VALUE_BOOLEAN:
            return left->boolean == right->boolean;

        case VALUE_STRING:
            if (
                left->string == NULL ||
                right->string == NULL
            ) {
                return left->string == right->string;
            }

            return strcmp(
                left->string,
                right->string
            ) == 0;

        case VALUE_ARRAY:
            if (
                left->array.count !=
                right->array.count
            ) {
                return 0;
            }

            for (
                int i = 0;
                i < left->array.count;
                i++
            ) {
                if (
                    !values_equal(
                        &left->array.items[i],
                        &right->array.items[i]
                    )
                ) {
                    return 0;
                }
            }

            return 1;

        default:
            return 0;
    }
}

void print_value_internal(
    const Value *value
) {
    if (value == NULL) {
        return;
    }

    switch (value->type) {
        case VALUE_INVALID:
            break;

        case VALUE_STRING:
            printf(
                "%s",
                value->string != NULL
                    ? value->string
                    : ""
            );
            break;

        case VALUE_INTEGER:
            printf(
                "%d",
                value->integer
            );
            break;

        case VALUE_BOOLEAN:
            printf(
                "%s",
                value->boolean
                    ? "true"
                    : "false"
            );
            break;

        case VALUE_ARRAY:
            printf("[");

            for (
                int i = 0;
                i < value->array.count;
                i++
            ) {
                if (i > 0) {
                    printf(", ");
                }

                print_value_internal(
                    &value->array.items[i]
                );
            }

            printf("]");
            break;
    }
}


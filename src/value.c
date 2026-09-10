#include "value.h"

#include <stdlib.h>
#include <string.h>

Value value_string(const char *string) {
    Value value;

    value.type = VALUE_STRING;
    value.string = malloc(strlen(string) + 1);

    if (value.string != NULL) {
        strcpy(value.string, string);
    }

    return value;
}

Value value_integer(int integer) {
    Value value;

    value.type = VALUE_INTEGER;
    value.integer = integer;

    return value;
}

Value value_boolean(int boolean) {
    Value value;

    value.type = VALUE_BOOLEAN;
    value.boolean = boolean ? 1 : 0;

    return value;
}

Value value_array(void) {
    Value value;

    value.type = VALUE_ARRAY;
    value.array.items = NULL;
    value.array.count = 0;
    value.array.capacity = 0;

    return value;
}

int value_array_append(
    Value *array,
    Value item
) {
    if (
        array == NULL ||
        array->type != VALUE_ARRAY
    ) {
        return 0;
    }

    if (array->array.count >= array->array.capacity) {
        int new_capacity =
            array->array.capacity == 0
                ? 4
                : array->array.capacity * 2;

        Value *new_items = realloc(
            array->array.items,
            sizeof(Value) * new_capacity
        );

        if (new_items == NULL) {
            return 0;
        }

        array->array.items = new_items;
        array->array.capacity = new_capacity;
    }

    array->array.items[
        array->array.count
    ] = item;

    array->array.count++;

    return 1;
}

Value value_copy(const Value *value) {
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

        case VALUE_ARRAY: {
            Value copy = value_array();

            for (
                int i = 0;
                i < value->array.count;
                i++
            ) {
                Value item =
                    value_copy(&value->array.items[i]);

                if (item.type == VALUE_INVALID) {
                    value_free(&copy);

                    Value invalid = {0};
                    return invalid;
                }

                if (!value_array_append(&copy, item)) {
                    value_free(&item);
                    value_free(&copy);

                    Value invalid = {0};
                    return invalid;
                }
            }

            return copy;
        }

        default: {
            Value invalid = {0};
            return invalid;
        }
    }
}

Value *value_array_get(
    Value *array,
    int index
) {
    if (
        array == NULL ||
        array->type != VALUE_ARRAY
    ) {
        return NULL;
    }

    if (
        index < 0 ||
        index >= array->array.count
    ) {
        return NULL;
    }

    return &array->array.items[index];
}

int value_array_set(
    Value *array,
    int index,
    Value item
) {
    if (
        array == NULL ||
        array->type != VALUE_ARRAY
    ) {
        return 0;
    }

    if (
        index < 0 ||
        index >= array->array.count
    ) {
        return 0;
    }

    value_free(&array->array.items[index]);
    array->array.items[index] = item;

    return 1;
}

void value_free(Value *value) {
    if (value == NULL) {
        return;
    }

    switch (value->type) {
        case VALUE_STRING:
            free(value->string);
            value->string = NULL;
            break;

        case VALUE_ARRAY:
            for (
                int i = 0;
                i < value->array.count;
                i++
            ) {
                value_free(
                    &value->array.items[i]
                );
            }

            free(value->array.items);

            value->array.items = NULL;
            value->array.count = 0;
            value->array.capacity = 0;
            break;

        default:
            break;
    }

    value->type = VALUE_INVALID;
}
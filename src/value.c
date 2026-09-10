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

void value_free(Value *value) {
    if (value == NULL) {
        return;
    }

    if (value->type == VALUE_STRING) {
        free(value->string);
        value->string = NULL;
    }

    value->type = VALUE_INVALID;
}
#ifndef VNT_VALUE_H
#define VNT_VALUE_H

typedef enum {
    VALUE_INVALID,
    VALUE_STRING,
    VALUE_INTEGER,
    VALUE_BOOLEAN
} ValueType;

typedef struct {
    ValueType type;

    union {
        char *string;
        int integer;
        int boolean;
    };
} Value;

Value value_string(const char *string);
Value value_integer(int integer);
Value value_boolean(int boolean);

void value_free(Value *value);

#endif
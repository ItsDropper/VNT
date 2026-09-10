#ifndef VNT_VALUE_H
#define VNT_VALUE_H

typedef enum {
    VALUE_INVALID,
    VALUE_STRING,
    VALUE_INTEGER,
    VALUE_BOOLEAN,
    VALUE_ARRAY
} ValueType;

typedef struct Value Value;

struct Value {
    ValueType type;

    union {
        char *string;
        int integer;
        int boolean;

        struct {
            Value *items;
            int count;
            int capacity;
        } array;
    };
};

Value value_string(const char *string);

Value value_integer(int integer);

Value value_boolean(int boolean);

Value value_array(void);

int value_array_append(
    Value *array,
    Value item
);

void value_free(Value *value);

#endif
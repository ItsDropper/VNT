#ifndef VNT_OPERATORS_H
#define VNT_OPERATORS_H

/* Shared operator IDs used by the parser AST and AST-independent HIR. */
typedef enum {
    BINARY_ADD,
    BINARY_SUBTRACT,
    BINARY_MULTIPLY,
    BINARY_DIVIDE,
    BINARY_MODULO,
    BINARY_EQUAL,
    BINARY_NOT_EQUAL,
    BINARY_GREATER,
    BINARY_LESS,
    BINARY_GREATER_EQUAL,
    BINARY_LESS_EQUAL,
    BINARY_AND,
    BINARY_OR
} BinaryOperator;

typedef enum {
    UNARY_NOT,
    UNARY_NEGATE,
    UNARY_REFERENCE,
    UNARY_DEREFERENCE
} UnaryOperator;

#endif


#ifndef VNT_AST_H
#define VNT_AST_H

typedef enum {
    AST_PROGRAM,
    AST_PRINT_STATEMENT,
    AST_IF_STATEMENT,

    AST_STRING_LITERAL,
    AST_INTEGER_LITERAL,

    AST_VARIABLE_DECLARATION,
    AST_VARIABLE,

    AST_BINARY_EXPRESSION
} AstNodeType;

typedef enum {
    BINARY_ADD,
    BINARY_SUBTRACT,
    BINARY_MULTIPLY,
    BINARY_DIVIDE,

    BINARY_EQUAL,
    BINARY_NOT_EQUAL,
    BINARY_GREATER,
    BINARY_LESS,
    BINARY_GREATER_EQUAL,
    BINARY_LESS_EQUAL
} BinaryOperator;

typedef struct AstNode {
    AstNodeType type;

    union {
        struct {
            struct AstNode *statements;
        } program;

        struct {
            struct AstNode *expression;
        } print_statement;

        struct {
            struct AstNode *condition;
            struct AstNode *then_branch;
            struct AstNode *else_branch;
        } if_statement;

        struct {
            char *value;
        } string_literal;

        struct {
            int value;
        } integer_literal;

        struct {
            char *name;
            struct AstNode *value;
        } variable_declaration;

        struct {
            char *name;
        } variable;

        struct {
            struct AstNode *left;
            struct AstNode *right;
            BinaryOperator operator;
        } binary_expression;
    };

    struct AstNode *next;
} AstNode;

AstNode *ast_create_program(AstNode *statements);

AstNode *ast_create_print(AstNode *expression);

AstNode *ast_create_if(
    AstNode *condition,
    AstNode *then_branch,
    AstNode *else_branch
);

AstNode *ast_create_string(const char *value);

AstNode *ast_create_integer(int value);

AstNode *ast_create_variable_declaration(
    const char *name,
    AstNode *value
);

AstNode *ast_create_variable(const char *name);

AstNode *ast_create_binary(
    AstNode *left,
    AstNode *right,
    BinaryOperator operator
);

void ast_append(AstNode **list, AstNode *node);

void ast_free(AstNode *node);

#endif


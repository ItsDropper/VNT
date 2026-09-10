#ifndef VNT_AST_H
#define VNT_AST_H

typedef enum {
    AST_PROGRAM,
    AST_PRINT_STATEMENT,
    AST_IF_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_FUNCTION_DECLARATION,
    AST_FUNCTION_CALL,
    AST_RETURN_STATEMENT,
    AST_BREAK_STATEMENT,
    AST_CONTINUE_STATEMENT,

    AST_STRING_LITERAL,
    AST_INTEGER_LITERAL,
    AST_BOOLEAN_LITERAL,
    AST_ARRAY_LITERAL,

    AST_VARIABLE_DECLARATION,
    AST_VARIABLE,
    AST_INDEX_EXPRESSION,
    AST_ASSIGNMENT,

    AST_BINARY_EXPRESSION,
    AST_UNARY_EXPRESSION
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
    BINARY_LESS_EQUAL,

    BINARY_AND,
    BINARY_OR
} BinaryOperator;

typedef enum {
    UNARY_NOT
} UnaryOperator;

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
            struct AstNode *condition;
            struct AstNode *body;
        } while_statement;

        struct {
            char *name;
            char **parameters;
            int parameter_count;
            struct AstNode *body;
        } function_declaration;

        struct {
            char *name;
            struct AstNode *arguments;
            int argument_count;
        } function_call;

        struct {
            struct AstNode *expression;
        } return_statement;

        struct {
            char *value;
        } string_literal;

        struct {
            int value;
        } integer_literal;

        struct {
            int value;
        } boolean_literal;

        struct {
            struct AstNode *elements;
            int element_count;
        } array_literal;

        struct {
            char *name;
            struct AstNode *value;
        } variable_declaration;

        struct {
            char *name;
        } variable;

        struct {
            struct AstNode *array;
            struct AstNode *index;
        } index_expression;

        struct {
            struct AstNode *target;
            struct AstNode *value;
        } assignment;

        struct {
            struct AstNode *left;
            struct AstNode *right;
            BinaryOperator operator;
        } binary_expression;

        struct {
            struct AstNode *operand;
            UnaryOperator operator;
        } unary_expression;
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

AstNode *ast_create_while(
    AstNode *condition,
    AstNode *body
);

AstNode *ast_create_function_declaration(
    const char *name,
    char **parameters,
    int parameter_count,
    AstNode *body
);

AstNode *ast_create_function_call(
    const char *name,
    AstNode *arguments,
    int argument_count
);

AstNode *ast_create_return(
    AstNode *expression
);

AstNode *ast_create_break(void);

AstNode *ast_create_continue(void);

AstNode *ast_create_string(const char *value);

AstNode *ast_create_integer(int value);

AstNode *ast_create_boolean(int value);

AstNode *ast_create_array(
    AstNode *elements,
    int element_count
);

AstNode *ast_create_variable_declaration(
    const char *name,
    AstNode *value
);

AstNode *ast_create_variable(const char *name);

AstNode *ast_create_index(
    AstNode *array,
    AstNode *index
);

AstNode *ast_create_assignment(
    AstNode *target,
    AstNode *value
);

AstNode *ast_create_binary(
    AstNode *left,
    AstNode *right,
    BinaryOperator operator
);

AstNode *ast_create_unary(
    AstNode *operand,
    UnaryOperator operator
);

void ast_append(AstNode **list, AstNode *node);

void ast_free(AstNode *node);

#endif
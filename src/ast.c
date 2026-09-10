
#include "ast.h"

#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *value) {
    char *copy = malloc(strlen(value) + 1);

    if (copy == NULL) {
        return NULL;
    }

    strcpy(copy, value);

    return copy;
}

AstNode *ast_create_program(AstNode *statements) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_PROGRAM;
    node->program.statements = statements;
    node->next = NULL;

    return node;
}

AstNode *ast_create_print(AstNode *expression) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_PRINT_STATEMENT;
    node->print_statement.expression = expression;
    node->next = NULL;

    return node;
}

AstNode *ast_create_if(
    AstNode *condition,
    AstNode *then_branch,
    AstNode *else_branch
) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_IF_STATEMENT;

    node->if_statement.condition = condition;
    node->if_statement.then_branch = then_branch;
    node->if_statement.else_branch = else_branch;

    node->next = NULL;

    return node;
}

AstNode *ast_create_string(const char *value) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_STRING_LITERAL;
    node->string_literal.value = copy_string(value);
    node->next = NULL;

    if (node->string_literal.value == NULL) {
        free(node);
        return NULL;
    }

    return node;
}

AstNode *ast_create_integer(int value) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_INTEGER_LITERAL;
    node->integer_literal.value = value;
    node->next = NULL;

    return node;
}

AstNode *ast_create_variable_declaration(
    const char *name,
    AstNode *value
) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_VARIABLE_DECLARATION;
    node->variable_declaration.name = copy_string(name);
    node->variable_declaration.value = value;
    node->next = NULL;

    if (node->variable_declaration.name == NULL) {
        free(node);
        return NULL;
    }

    return node;
}

AstNode *ast_create_variable(const char *name) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_VARIABLE;
    node->variable.name = copy_string(name);
    node->next = NULL;

    if (node->variable.name == NULL) {
        free(node);
        return NULL;
    }

    return node;
}

AstNode *ast_create_binary(
    AstNode *left,
    AstNode *right,
    BinaryOperator operator
) {
    AstNode *node = malloc(sizeof(AstNode));

    if (node == NULL) {
        return NULL;
    }

    node->type = AST_BINARY_EXPRESSION;

    node->binary_expression.left = left;
    node->binary_expression.right = right;
    node->binary_expression.operator = operator;

    node->next = NULL;

    return node;
}

void ast_append(AstNode **list, AstNode *node) {
    if (node == NULL) {
        return;
    }

    if (*list == NULL) {
        *list = node;
        return;
    }

    AstNode *current = *list;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = node;
}

void ast_free(AstNode *node) {
    while (node != NULL) {
        AstNode *next = node->next;

        switch (node->type) {
            case AST_PROGRAM:
                ast_free(node->program.statements);
                break;

            case AST_PRINT_STATEMENT:
                ast_free(node->print_statement.expression);
                break;

            case AST_STRING_LITERAL:
                free(node->string_literal.value);
                break;

            case AST_INTEGER_LITERAL:
                break;

            case AST_VARIABLE_DECLARATION:
                free(node->variable_declaration.name);
                ast_free(node->variable_declaration.value);
                break;

            case AST_VARIABLE:
                free(node->variable.name);
                break;

            case AST_BINARY_EXPRESSION:
                ast_free(node->binary_expression.left);
                ast_free(node->binary_expression.right);
                break;

            case AST_IF_STATEMENT:
                ast_free(node->if_statement.condition);
                ast_free(node->if_statement.then_branch);
                ast_free(node->if_statement.else_branch);
                break;
        }

        free(node);
        node = next;
    }
}


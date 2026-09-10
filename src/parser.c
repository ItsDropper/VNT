#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void advance_parser(Parser *parser) {
    parser->previous = parser->current;
    parser->current = lexer_next(parser->lexer);
}

static int check(Parser *parser, TokenType type) {
    return parser->current.type == type;
}

static int consume(
    Parser *parser,
    TokenType type,
    const char *message
) {
    if (check(parser, type)) {
        advance_parser(parser);
        return 1;
    }

    printf("Parser error: %s\n", message);
    return 0;
}

static char *token_to_string(Token token) {
    char *value = malloc(token.length + 1);

    if (value == NULL) {
        return NULL;
    }

    memcpy(value, token.start, token.length);
    value[token.length] = '\0';

    return value;
}

static int is_token(Token token, const char *text) {
    size_t length = strlen(text);

    return token.type == TOKEN_IDENTIFIER &&
           token.length == (int)length &&
           strncmp(token.start, text, length) == 0;
}

static AstNode *parse_expression(Parser *parser);

static AstNode *parse_primary(Parser *parser) {
    if (check(parser, TOKEN_INTEGER)) {
        char *value = token_to_string(parser->current);

        if (value == NULL) {
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        int integer = atoi(value);
        free(value);

        advance_parser(parser);

        return ast_create_integer(integer);
    }

    if (check(parser, TOKEN_STRING)) {
        char *value = token_to_string(parser->current);

        if (value == NULL) {
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        advance_parser(parser);

        AstNode *node = ast_create_string(value);
        free(value);

        return node;
    }

    if (check(parser, TOKEN_IDENTIFIER)) {
        char *name = token_to_string(parser->current);

        if (name == NULL) {
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        advance_parser(parser);

        AstNode *node = ast_create_variable(name);
        free(name);

        return node;
    }

    if (check(parser, TOKEN_LEFT_PAREN)) {
        advance_parser(parser);

        AstNode *expression = parse_expression(parser);

        if (expression == NULL) {
            return NULL;
        }

        if (!consume(
                parser,
                TOKEN_RIGHT_PAREN,
                "expected ')'."
            )) {
            ast_free(expression);
            return NULL;
        }

        return expression;
    }

    return NULL;
}

static AstNode *parse_multiplication(Parser *parser) {
    AstNode *left = parse_primary(parser);

    if (left == NULL) {
        printf("Parser error: expected an expression.\n");
        return NULL;
    }

    while (
        check(parser, TOKEN_STAR) ||
        check(parser, TOKEN_SLASH)
    ) {
        TokenType operator = parser->current.type;
        advance_parser(parser);

        AstNode *right = parse_primary(parser);

        if (right == NULL) {
            ast_free(left);
            printf(
                "Parser error: expected expression after operator.\n"
            );
            return NULL;
        }

        BinaryOperator binary_operator =
            operator == TOKEN_STAR
                ? BINARY_MULTIPLY
                : BINARY_DIVIDE;

        AstNode *binary = ast_create_binary(
            left,
            right,
            binary_operator
        );

        if (binary == NULL) {
            ast_free(left);
            ast_free(right);
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        left = binary;
    }

    return left;
}

static AstNode *parse_addition(Parser *parser) {
    AstNode *left = parse_multiplication(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        check(parser, TOKEN_PLUS) ||
        check(parser, TOKEN_MINUS)
    ) {
        TokenType operator = parser->current.type;
        advance_parser(parser);

        AstNode *right = parse_multiplication(parser);

        if (right == NULL) {
            ast_free(left);
            printf(
                "Parser error: expected expression after operator.\n"
            );
            return NULL;
        }

        BinaryOperator binary_operator =
            operator == TOKEN_PLUS
                ? BINARY_ADD
                : BINARY_SUBTRACT;

        AstNode *binary = ast_create_binary(
            left,
            right,
            binary_operator
        );

        if (binary == NULL) {
            ast_free(left);
            ast_free(right);
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        left = binary;
    }

    return left;
}

static AstNode *parse_comparison(Parser *parser) {
    AstNode *left = parse_addition(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        check(parser, TOKEN_GREATER) ||
        check(parser, TOKEN_LESS) ||
        check(parser, TOKEN_GREATER_EQUAL) ||
        check(parser, TOKEN_LESS_EQUAL)
    ) {
        TokenType operator = parser->current.type;
        advance_parser(parser);

        AstNode *right = parse_addition(parser);

        if (right == NULL) {
            ast_free(left);
            printf(
                "Parser error: expected expression after operator.\n"
            );
            return NULL;
        }

        BinaryOperator binary_operator;

        switch (operator) {
            case TOKEN_GREATER:
                binary_operator = BINARY_GREATER;
                break;

            case TOKEN_LESS:
                binary_operator = BINARY_LESS;
                break;

            case TOKEN_GREATER_EQUAL:
                binary_operator = BINARY_GREATER_EQUAL;
                break;

            case TOKEN_LESS_EQUAL:
                binary_operator = BINARY_LESS_EQUAL;
                break;

            default:
                ast_free(left);
                ast_free(right);
                printf(
                    "Parser error: invalid comparison operator.\n"
                );
                return NULL;
        }

        AstNode *binary = ast_create_binary(
            left,
            right,
            binary_operator
        );

        if (binary == NULL) {
            ast_free(left);
            ast_free(right);
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        left = binary;
    }

    return left;
}

static AstNode *parse_equality(Parser *parser) {
    AstNode *left = parse_comparison(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        check(parser, TOKEN_EQUAL_EQUAL) ||
        check(parser, TOKEN_BANG_EQUAL)
    ) {
        TokenType operator = parser->current.type;
        advance_parser(parser);

        AstNode *right = parse_comparison(parser);

        if (right == NULL) {
            ast_free(left);
            printf(
                "Parser error: expected expression after operator.\n"
            );
            return NULL;
        }

        BinaryOperator binary_operator =
            operator == TOKEN_EQUAL_EQUAL
                ? BINARY_EQUAL
                : BINARY_NOT_EQUAL;

        AstNode *binary = ast_create_binary(
            left,
            right,
            binary_operator
        );

        if (binary == NULL) {
            ast_free(left);
            ast_free(right);
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        left = binary;
    }

    return left;
}

static AstNode *parse_expression(Parser *parser) {
    return parse_equality(parser);
}

static AstNode *parse_statement(Parser *parser);

static AstNode *parse_block(Parser *parser) {
    AstNode *statements = NULL;

    while (
        !check(parser, TOKEN_RIGHT_BRACE) &&
        !check(parser, TOKEN_EOF)
    ) {
        AstNode *statement = parse_statement(parser);

        if (statement == NULL) {
            ast_free(statements);
            return NULL;
        }

        ast_append(&statements, statement);
    }

    if (!consume(
            parser,
            TOKEN_RIGHT_BRACE,
            "expected '}'."
        )) {
        ast_free(statements);
        return NULL;
    }

    return statements;
}

static AstNode *parse_if(Parser *parser) {
    advance_parser(parser);

    AstNode *condition = parse_expression(parser);

    if (condition == NULL) {
        return NULL;
    }

    if (!consume(
            parser,
            TOKEN_LEFT_BRACE,
            "expected '{' after if condition."
        )) {
        ast_free(condition);
        return NULL;
    }

    AstNode *then_branch = parse_block(parser);

    if (then_branch == NULL &&
        !check(parser, TOKEN_IDENTIFIER)) {
        ast_free(condition);
        return NULL;
    }

    AstNode *else_branch = NULL;

    if (is_token(parser->current, "else")) {
        advance_parser(parser);

        if (!consume(
                parser,
                TOKEN_LEFT_BRACE,
                "expected '{' after else."
            )) {
            ast_free(condition);
            ast_free(then_branch);
            return NULL;
        }

        else_branch = parse_block(parser);

        if (else_branch == NULL &&
            !check(parser, TOKEN_EOF)) {
            ast_free(condition);
            ast_free(then_branch);
            return NULL;
        }
    }

    AstNode *node = ast_create_if(
        condition,
        then_branch,
        else_branch
    );

    if (node == NULL) {
        ast_free(condition);
        ast_free(then_branch);
        ast_free(else_branch);

        printf("Parser error: out of memory.\n");
        return NULL;
    }

    return node;
}

static AstNode *parse_print(Parser *parser) {
    if (!is_token(parser->current, "print")) {
        printf("Parser error: expected 'print'.\n");
        return NULL;
    }

    advance_parser(parser);

    if (!consume(
            parser,
            TOKEN_LEFT_PAREN,
            "expected '('."
        )) {
        return NULL;
    }

    AstNode *expression = parse_expression(parser);

    if (expression == NULL) {
        return NULL;
    }

    if (!consume(
            parser,
            TOKEN_RIGHT_PAREN,
            "expected ')'."
        )) {
        ast_free(expression);
        return NULL;
    }

    AstNode *node = ast_create_print(expression);

    if (node == NULL) {
        ast_free(expression);
        printf("Parser error: out of memory.\n");
        return NULL;
    }

    return node;
}

static AstNode *parse_variable_declaration(Parser *parser) {
    Token name_token = parser->current;

    char *name = token_to_string(name_token);

    if (name == NULL) {
        printf("Parser error: out of memory.\n");
        return NULL;
    }

    advance_parser(parser);

    if (!consume(
            parser,
            TOKEN_EQUALS,
            "expected '=' after variable name."
        )) {
        free(name);
        return NULL;
    }

    AstNode *value = parse_expression(parser);

    if (value == NULL) {
        free(name);
        return NULL;
    }

    AstNode *node = ast_create_variable_declaration(
        name,
        value
    );

    free(name);

    if (node == NULL) {
        ast_free(value);
        printf("Parser error: out of memory.\n");
        return NULL;
    }

    return node;
}

static AstNode *parse_statement(Parser *parser) {
    if (is_token(parser->current, "print")) {
        return parse_print(parser);
    }

    if (is_token(parser->current, "if")) {
        return parse_if(parser);
    }

    if (check(parser, TOKEN_IDENTIFIER)) {
        return parse_variable_declaration(parser);
    }

    printf("Parser error: expected a statement.\n");
    return NULL;
}

void parser_init(Parser *parser, Lexer *lexer) {
    parser->lexer = lexer;
    parser->current.type = TOKEN_UNKNOWN;
    parser->previous.type = TOKEN_UNKNOWN;

    advance_parser(parser);
}

AstNode *parser_parse(Parser *parser) {
    AstNode *statements = NULL;

    while (!check(parser, TOKEN_EOF)) {
        AstNode *statement = parse_statement(parser);

        if (statement == NULL) {
            ast_free(statements);
            return NULL;
        }

        ast_append(&statements, statement);
    }

    AstNode *program = ast_create_program(statements);

    if (program == NULL) {
        ast_free(statements);
        printf("Parser error: out of memory.\n");
        return NULL;
    }

    return program;
}
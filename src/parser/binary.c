#include <vnt/parser.h>
#include "../internal/parser_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static AstNode *parse_multiplication(
    Parser *parser
) {
    AstNode *left =
        parse_unary(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        token_is(parser, TOKEN_STAR) ||
        token_is(parser, TOKEN_SLASH) ||
        token_is(parser, TOKEN_PERCENT)
    ) {
        TokenType operator =
            parser->current.type;

        parser_advance(parser);

        AstNode *right =
            parse_unary(parser);

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        BinaryOperator binary_operator;

        switch (operator) {
            case TOKEN_STAR:
                binary_operator =
                    BINARY_MULTIPLY;
                break;

            case TOKEN_SLASH:
                binary_operator =
                    BINARY_DIVIDE;
                break;

            case TOKEN_PERCENT:
                binary_operator =
                    BINARY_MODULO;
                break;

            default:
                ast_free(left);
                ast_free(right);
                return NULL;
        }

        AstNode *node =
            ast_create_binary(
                left,
                right,
                binary_operator
            );

        if (node == NULL) {
            ast_free(left);
            ast_free(right);
            return NULL;
        }

        left = node;
    }

    return left;
}

static AstNode *parse_addition(
    Parser *parser
) {
    AstNode *left =
        parse_multiplication(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        token_is(
            parser,
            TOKEN_PLUS
        ) ||
        token_is(
            parser,
            TOKEN_MINUS
        )
    ) {
        TokenType operator =
            parser->current.type;

        parser_advance(parser);

        AstNode *right =
            parse_multiplication(parser);

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        AstNode *node =
            ast_create_binary(
                left,
                right,
                operator == TOKEN_PLUS
                    ? BINARY_ADD
                    : BINARY_SUBTRACT
            );

        if (node == NULL) {
            ast_free(left);
            ast_free(right);
            return NULL;
        }

        left = node;
    }

    return left;
}

static AstNode *parse_comparison(
    Parser *parser
) {
    AstNode *left =
        parse_addition(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        token_is(parser, TOKEN_GREATER) ||
        token_is(parser, TOKEN_LESS) ||
        token_is(parser, TOKEN_GREATER_EQUAL) ||
        token_is(parser, TOKEN_LESS_EQUAL)
    ) {
        TokenType operator =
            parser->current.type;

        parser_advance(parser);

        AstNode *right =
            parse_addition(parser);

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        BinaryOperator binary_operator;

        switch (operator) {
            case TOKEN_GREATER:
                binary_operator =
                    BINARY_GREATER;
                break;

            case TOKEN_LESS:
                binary_operator =
                    BINARY_LESS;
                break;

            case TOKEN_GREATER_EQUAL:
                binary_operator =
                    BINARY_GREATER_EQUAL;
                break;

            case TOKEN_LESS_EQUAL:
                binary_operator =
                    BINARY_LESS_EQUAL;
                break;

            default:
                ast_free(left);
                ast_free(right);
                return NULL;
        }

        AstNode *node =
            ast_create_binary(
                left,
                right,
                binary_operator
            );

        if (node == NULL) {
            ast_free(left);
            ast_free(right);
            return NULL;
        }

        left = node;
    }

    return left;
}

static AstNode *parse_equality(
    Parser *parser
) {
    AstNode *left =
        parse_comparison(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        token_is(
            parser,
            TOKEN_EQUAL_EQUAL
        ) ||
        token_is(
            parser,
            TOKEN_BANG_EQUAL
        )
    ) {
        TokenType operator =
            parser->current.type;

        parser_advance(parser);

        AstNode *right =
            parse_comparison(parser);

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        AstNode *node =
            ast_create_binary(
                left,
                right,
                operator == TOKEN_EQUAL_EQUAL
                    ? BINARY_EQUAL
                    : BINARY_NOT_EQUAL
            );

        if (node == NULL) {
            ast_free(left);
            ast_free(right);
            return NULL;
        }

        left = node;
    }

    return left;
}

static AstNode *parse_and(
    Parser *parser
) {
    AstNode *left =
        parse_equality(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        token_is(
            parser,
            TOKEN_AND_AND
        )
    ) {
        parser_advance(parser);

        AstNode *right =
            parse_equality(parser);

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        AstNode *node =
            ast_create_binary(
                left,
                right,
                BINARY_AND
            );

        if (node == NULL) {
            ast_free(left);
            ast_free(right);
            return NULL;
        }

        left = node;
    }

    return left;
}

static AstNode *parse_or(
    Parser *parser
) {
    AstNode *left =
        parse_and(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        token_is(
            parser,
            TOKEN_OR_OR
        )
    ) {
        parser_advance(parser);

        AstNode *right =
            parse_and(parser);

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        AstNode *node =
            ast_create_binary(
                left,
                right,
                BINARY_OR
            );

        if (node == NULL) {
            ast_free(left);
            ast_free(right);
            return NULL;
        }

        left = node;
    }

    return left;
}

AstNode *parse_expression(
    Parser *parser
) {
    return parse_or(parser);
}
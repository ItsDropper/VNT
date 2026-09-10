#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

static AstNode *parse_primary(Parser *parser);

static AstNode *parse_postfix(Parser *parser) {
    AstNode *expression = parse_primary(parser);

    if (expression == NULL) {
        return NULL;
    }

    while (parser_check(parser, TOKEN_LEFT_BRACKET)) {
        parser_advance(parser);

        AstNode *index = parse_expression(parser);

        if (index == NULL) {
            ast_free(expression);

            printf(
                "Parser error: expected array index.\n"
            );

            return NULL;
        }

        if (!parser_consume(
                parser,
                TOKEN_RIGHT_BRACKET,
                "expected ']' after array index."
            )) {
            ast_free(expression);
            ast_free(index);
            return NULL;
        }

        AstNode *indexed = ast_create_index(
            expression,
            index
        );

        if (indexed == NULL) {
            ast_free(expression);
            ast_free(index);

            printf("Parser error: out of memory.\n");
            return NULL;
        }

        expression = indexed;
    }

    return expression;
}

static AstNode *parse_primary(Parser *parser) {
    if (parser_check(parser, TOKEN_INTEGER)) {
        char *value =
            parser_token_to_string(parser->current);

        if (value == NULL) {
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        int integer = atoi(value);
        free(value);

        parser_advance(parser);

        return ast_create_integer(integer);
    }

    if (parser_check(parser, TOKEN_STRING)) {
        char *value =
            parser_token_to_string(parser->current);

        if (value == NULL) {
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        parser_advance(parser);

        AstNode *node = ast_create_string(value);
        free(value);

        return node;
    }

    if (parser_check(parser, TOKEN_LEFT_BRACKET)) {
        parser_advance(parser);

        AstNode *elements = NULL;
        int element_count = 0;

        if (!parser_check(parser, TOKEN_RIGHT_BRACKET)) {
            for (;;) {
                AstNode *element =
                    parse_expression(parser);

                if (element == NULL) {
                    ast_free(elements);

                    printf(
                        "Parser error: expected array element.\n"
                    );

                    return NULL;
                }

                ast_append(&elements, element);
                element_count++;

                if (parser_check(
                        parser,
                        TOKEN_RIGHT_BRACKET
                    )) {
                    break;
                }

                if (!parser_consume(
                        parser,
                        TOKEN_COMMA,
                        "expected ',' between array elements."
                    )) {
                    ast_free(elements);
                    return NULL;
                }
            }
        }

        if (!parser_consume(
                parser,
                TOKEN_RIGHT_BRACKET,
                "expected ']' after array."
            )) {
            ast_free(elements);
            return NULL;
        }

        AstNode *array = ast_create_array(
            elements,
            element_count
        );

        if (array == NULL) {
            ast_free(elements);

            printf("Parser error: out of memory.\n");
            return NULL;
        }

        return array;
    }

    if (parser_check(parser, TOKEN_IDENTIFIER)) {
        char *name =
            parser_token_to_string(parser->current);

        if (name == NULL) {
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        parser_advance(parser);

        if (parser_check(parser, TOKEN_LEFT_PAREN)) {
            return parse_function_call(
                parser,
                name
            );
        }

        AstNode *node = ast_create_variable(name);
        free(name);

        return node;
    }

    if (parser_check(parser, TOKEN_LEFT_PAREN)) {
        parser_advance(parser);

        AstNode *expression =
            parse_expression(parser);

        if (expression == NULL) {
            return NULL;
        }

        if (!parser_consume(
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
    AstNode *left = parse_postfix(parser);

    if (left == NULL) {
        printf(
            "Parser error: expected an expression.\n"
        );

        return NULL;
    }

    while (
        parser_check(parser, TOKEN_STAR) ||
        parser_check(parser, TOKEN_SLASH)
    ) {
        TokenType operator = parser->current.type;
        parser_advance(parser);

        AstNode *right = parse_postfix(parser);

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
    AstNode *left =
        parse_multiplication(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        parser_check(parser, TOKEN_PLUS) ||
        parser_check(parser, TOKEN_MINUS)
    ) {
        TokenType operator = parser->current.type;
        parser_advance(parser);

        AstNode *right =
            parse_multiplication(parser);

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
        parser_check(parser, TOKEN_GREATER) ||
        parser_check(parser, TOKEN_LESS) ||
        parser_check(parser, TOKEN_GREATER_EQUAL) ||
        parser_check(parser, TOKEN_LESS_EQUAL)
    ) {
        TokenType operator = parser->current.type;
        parser_advance(parser);

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
    AstNode *left =
        parse_comparison(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        parser_check(parser, TOKEN_EQUAL_EQUAL) ||
        parser_check(parser, TOKEN_BANG_EQUAL)
    ) {
        TokenType operator = parser->current.type;
        parser_advance(parser);

        AstNode *right =
            parse_comparison(parser);

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

AstNode *parse_expression(Parser *parser) {
    return parse_equality(parser);
}
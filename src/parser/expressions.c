#include "../parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int token_is(
    Parser *parser,
    TokenType type
) {
    return parser->current.type == type;
}

AstNode *parse_expression(Parser *parser);

static AstNode *parse_primary(Parser *parser) {
    Token token =
        parser->current;

    if (
        token.type ==
        TOKEN_INTEGER
    ) {
        char buffer[64];

        if (token.length >= (int)sizeof(buffer)) {
            printf(
                "Parser error: integer too long.\n"
            );

            return NULL;
        }

        memcpy(
            buffer,
            token.start,
            token.length
        );

        buffer[token.length] = '\0';

        parser_advance(parser);

        return ast_create_integer(
            atoi(buffer)
        );
    }

    if (
        token.type ==
        TOKEN_STRING
    ) {
        char *value =
            malloc(token.length + 1);

        if (value == NULL) {
            printf(
                "Parser error: out of memory.\n"
            );

            return NULL;
        }

        memcpy(
            value,
            token.start,
            token.length
        );

        value[token.length] = '\0';

        parser_advance(parser);

        AstNode *node =
            ast_create_string(value);

        free(value);

        return node;
    }

    if (
        token.type ==
        TOKEN_LEFT_BRACKET
    ) {
        parser_advance(parser);

        AstNode *elements = NULL;
        int count = 0;

        if (
            !token_is(
                parser,
                TOKEN_RIGHT_BRACKET
            )
        ) {
            for (;;) {
                AstNode *element =
                    parse_expression(parser);

                if (element == NULL) {
                    ast_free(elements);
                    return NULL;
                }

                ast_append(
                    &elements,
                    element
                );

                count++;

                if (
                    token_is(
                        parser,
                        TOKEN_COMMA
                    )
                ) {
                    parser_advance(parser);

                    if (
                        token_is(
                            parser,
                            TOKEN_RIGHT_BRACKET
                        )
                    ) {
                        break;
                    }

                    continue;
                }

                break;
            }
        }

        if (
            !token_is(
                parser,
                TOKEN_RIGHT_BRACKET
            )
        ) {
            printf(
                "Parser error: expected ']'.\n"
            );

            ast_free(elements);
            return NULL;
        }

        parser_advance(parser);

        return ast_create_array(
            elements,
            count
        );
    }

    if (
        token_is(
            parser,
            TOKEN_LEFT_PAREN
        )
    ) {
        parser_advance(parser);

        AstNode *expression =
            parse_expression(parser);

        if (expression == NULL) {
            return NULL;
        }

        if (
            !token_is(
                parser,
                TOKEN_RIGHT_PAREN
            )
        ) {
            printf(
                "Parser error: expected ')'.\n"
            );

            ast_free(expression);
            return NULL;
        }

        parser_advance(parser);

        return expression;
    }

    if (
        token.type ==
        TOKEN_IDENTIFIER
    ) {
        char *name =
            malloc(token.length + 1);

        if (name == NULL) {
            printf(
                "Parser error: out of memory.\n"
            );

            return NULL;
        }

        memcpy(
            name,
            token.start,
            token.length
        );

        name[token.length] = '\0';

        parser_advance(parser);

        if (
            token_is(
                parser,
                TOKEN_LEFT_PAREN
            )
        ) {
            parser_advance(parser);

            AstNode *arguments = NULL;
            int argument_count = 0;

            if (
                !token_is(
                    parser,
                    TOKEN_RIGHT_PAREN
                )
            ) {
                for (;;) {
                    AstNode *argument =
                        parse_expression(parser);

                    if (argument == NULL) {
                        free(name);
                        ast_free(arguments);
                        return NULL;
                    }

                    ast_append(
                        &arguments,
                        argument
                    );

                    argument_count++;

                    if (
                        token_is(
                            parser,
                            TOKEN_COMMA
                        )
                    ) {
                        parser_advance(parser);

                        if (
                            token_is(
                                parser,
                                TOKEN_RIGHT_PAREN
                            )
                        ) {
                            printf(
                                "Parser error: trailing comma in function call.\n"
                            );

                            free(name);
                            ast_free(arguments);
                            return NULL;
                        }

                        continue;
                    }

                    break;
                }
            }

            if (
                !token_is(
                    parser,
                    TOKEN_RIGHT_PAREN
                )
            ) {
                printf(
                    "Parser error: expected ')'.\n"
                );

                free(name);
                ast_free(arguments);
                return NULL;
            }

            parser_advance(parser);

            AstNode *call =
                ast_create_function_call(
                    name,
                    arguments,
                    argument_count
                );

            free(name);

            return call;
        }

        if (
            strcmp(name, "true") == 0
        ) {
            free(name);

            return ast_create_boolean(1);
        }

        if (
            strcmp(name, "false") == 0
        ) {
            free(name);

            return ast_create_boolean(0);
        }

        AstNode *variable =
            ast_create_variable(name);

        free(name);

        return variable;
    }

    printf(
        "Parser error: expected expression.\n"
    );

    return NULL;
}

static AstNode *parse_postfix(Parser *parser) {
    AstNode *expression =
        parse_primary(parser);

    if (expression == NULL) {
        return NULL;
    }

    while (
        token_is(
            parser,
            TOKEN_LEFT_BRACKET
        )
    ) {
        parser_advance(parser);

        AstNode *index =
            parse_expression(parser);

        if (index == NULL) {
            ast_free(expression);
            return NULL;
        }

        if (
            !token_is(
                parser,
                TOKEN_RIGHT_BRACKET
            )
        ) {
            printf(
                "Parser error: expected ']'.\n"
            );

            ast_free(expression);
            ast_free(index);
            return NULL;
        }

        parser_advance(parser);

        AstNode *indexed =
            ast_create_index(
                expression,
                index
            );

        if (indexed == NULL) {
            ast_free(expression);
            ast_free(index);
            return NULL;
        }

        expression = indexed;
    }

    return expression;
}

static AstNode *parse_unary(Parser *parser) {
    if (
        token_is(
            parser,
            TOKEN_BANG
        )
    ) {
        parser_advance(parser);

        AstNode *operand =
            parse_unary(parser);

        if (operand == NULL) {
            return NULL;
        }

        AstNode *node =
            ast_create_unary(
                operand,
                UNARY_NOT
            );

        if (node == NULL) {
            ast_free(operand);
            return NULL;
        }

        return node;
    }

    return parse_postfix(parser);
}

static AstNode *parse_multiplication(
    Parser *parser
) {
    AstNode *left =
        parse_unary(parser);

    if (left == NULL) {
        return NULL;
    }

    while (
        token_is(
            parser,
            TOKEN_STAR
        ) ||
        token_is(
            parser,
            TOKEN_SLASH
        )
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

        AstNode *node =
            ast_create_binary(
                left,
                right,
                operator == TOKEN_STAR
                    ? BINARY_MULTIPLY
                    : BINARY_DIVIDE
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
#include <vnt/parser.h>
#include "../internal/parser_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

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

        errno = 0;
        char *end = NULL;
        int base = (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X')) ? 16 : 10;
        long value = strtol(buffer, &end, base);
        if (errno == ERANGE || end == buffer || *end != '\0' ||
            value < INT_MIN || value > INT_MAX) {
            printf("Parser error: integer literal is outside the supported range.\n");
            return NULL;
        }

        parser_advance(parser);

        AstNode *node = ast_create_integer((int)value);
        if (!node)
            printf("Parser error: out of memory.\n");
        return node;
    }

    if (
        token.type ==
        TOKEN_FLOAT
    ) {
        char buffer[64];
        if (token.length >= (int)sizeof(buffer)) {
            printf("Parser error: float too long.\n");
            return NULL;
        }
        memcpy(buffer, token.start, token.length);
        buffer[token.length] = '\0';
        parser_advance(parser);
        return ast_create_float(strtod(buffer, NULL));
    }

    if (
        token.type ==
        TOKEN_STRING
    ) {
        char *value = malloc(token.length + 1);
        if (value == NULL) {
            printf("Parser error: out of memory.\n");
            return NULL;
        }

        /*
         * Decode common string escapes here so the AST and all backends
         * receive actual characters rather than two-character sequences.
         */
        int source_index = 0;
        int target_index = 0;
        while (source_index < token.length) {
            char c = token.start[source_index++];
            if (c == '\\\\' && source_index < token.length) {
                char escaped = token.start[source_index++];
                switch (escaped) {
                    case 'n': c = '\\n'; break;
                    case 'r': c = '\\r'; break;
                    case 't': c = '\\t'; break;
                    case '\\\\': c = '\\\\'; break;
                    case '"': c = '"'; break;
                    default:
                        /* Preserve unknown escapes literally for compatibility. */
                        value[target_index++] = '\\\\';
                        c = escaped;
                        break;
                }
            }
            value[target_index++] = c;
        }
        value[target_index] = '\0';

        parser_advance(parser);
        AstNode *node = ast_create_string(value);
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

AstNode *parse_postfix(Parser *parser) {
    AstNode *expression =
        parse_primary(parser);

    if (expression == NULL) {
        return NULL;
    }

    for (;;) {
        if (token_is(parser, TOKEN_LEFT_BRACKET)) {
            parser_advance(parser);

            AstNode *index = parse_expression(parser);
            if (index == NULL) {
                ast_free(expression);
                return NULL;
            }

            if (!token_is(parser, TOKEN_RIGHT_BRACKET)) {
                printf("Parser error: expected ']'.\n");
                ast_free(expression);
                ast_free(index);
                return NULL;
            }
            parser_advance(parser);

            AstNode *indexed = ast_create_index(expression, index);
            if (indexed == NULL) {
                ast_free(expression);
                ast_free(index);
                return NULL;
            }
            expression = indexed;
            continue;
        }

        if (token_is(parser, TOKEN_DOT)) {
            parser_advance(parser);
            if (!token_is(parser, TOKEN_IDENTIFIER)) {
                printf("Parser error: expected member name after '.'.\n");
                ast_free(expression);
                return NULL;
            }
            char *member = parser_token_to_string(parser->current);
            parser_advance(parser);
            AstNode *member_node = ast_create_member(expression, member);
            free(member);
            if (member_node == NULL) {
                ast_free(expression);
                return NULL;
            }
            expression = member_node;
            if (token_is(parser, TOKEN_LEFT_PAREN) &&
                expression->member_expression.object &&
                expression->member_expression.object->type == AST_VARIABLE) {
                const char *module_name = expression->member_expression.object->variable.name;
                const char *function_name = expression->member_expression.member;
                size_t qualified_len = strlen(module_name) + strlen(function_name) + 3;
                char *qualified = malloc(qualified_len);
                if (!qualified) { ast_free(expression); return NULL; }
                snprintf(qualified, qualified_len, "%s__%s", module_name, function_name);
                parser_advance(parser);
                AstNode *arguments = NULL;
                int argument_count = 0;
                if (!token_is(parser, TOKEN_RIGHT_PAREN)) {
                    for (;;) {
                        AstNode *argument = parse_expression(parser);
                        if (!argument) { free(qualified); ast_free(arguments); ast_free(expression); return NULL; }
                        ast_append(&arguments, argument);
                        argument_count++;
                        if (token_is(parser, TOKEN_RIGHT_PAREN)) break;
                        if (!parser_consume(parser, TOKEN_COMMA, "expected ',' between arguments.")) {
                            free(qualified); ast_free(arguments); ast_free(expression); return NULL;
                        }
                    }
                }
                if (!parser_consume(parser, TOKEN_RIGHT_PAREN, "expected ')' after arguments.")) {
                    free(qualified); ast_free(arguments); ast_free(expression); return NULL;
                }
                AstNode *call = ast_create_function_call(qualified, arguments, argument_count);
                free(qualified);
                ast_free(expression);
                if (!call) { ast_free(arguments); return NULL; }
                expression = call;
            }
            continue;
        }

        break;
    }
    return expression;

}

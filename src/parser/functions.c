#include <vnt/parser.h>

#include <stdio.h>
#include <stdlib.h>

AstNode *parse_function(Parser *parser) {
    parser_advance(parser); /* consume "fun" */

    if (!parser_check(parser, TOKEN_IDENTIFIER)) {
        printf("Parser error: expected function name.\n");
        return NULL;
    }

    char *name =
        parser_token_to_string(parser->current);

    if (name == NULL) {
        printf("Parser error: out of memory.\n");
        return NULL;
    }

    parser_advance(parser);

    if (!parser_consume(
            parser,
            TOKEN_LEFT_PAREN,
            "expected '(' after function name."
        )) {
        free(name);
        return NULL;
    }

    char **parameters = NULL;
    int parameter_count = 0;
    int parameter_capacity = 0;

    if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
        for (;;) {
            if (!parser_check(
                    parser,
                    TOKEN_IDENTIFIER
                )) {
                printf(
                    "Parser error: expected parameter name.\n"
                );

                free(name);

                for (int i = 0; i < parameter_count; i++) {
                    free(parameters[i]);
                }

                free(parameters);

                return NULL;
            }

            char *parameter =
                parser_token_to_string(parser->current);

            if (parameter == NULL) {
                printf("Parser error: out of memory.\n");

                free(name);

                for (int i = 0; i < parameter_count; i++) {
                    free(parameters[i]);
                }

                free(parameters);

                return NULL;
            }

            parser_advance(parser);

            if (parameter_count >= parameter_capacity) {
                int new_capacity =
                    parameter_capacity == 0
                        ? 4
                        : parameter_capacity * 2;

                char **new_parameters = realloc(
                    parameters,
                    sizeof(char *) * new_capacity
                );

                if (new_parameters == NULL) {
                    printf(
                        "Parser error: out of memory.\n"
                    );

                    free(parameter);
                    free(name);

                    for (int i = 0; i < parameter_count; i++) {
                        free(parameters[i]);
                    }

                    free(parameters);

                    return NULL;
                }

                parameters = new_parameters;
                parameter_capacity = new_capacity;
            }

            parameters[parameter_count++] = parameter;

            if (parser_check(
                    parser,
                    TOKEN_RIGHT_PAREN
                )) {
                break;
            }

            if (!parser_consume(
                    parser,
                    TOKEN_COMMA,
                    "expected ',' between parameters."
                )) {
                free(name);

                for (int i = 0; i < parameter_count; i++) {
                    free(parameters[i]);
                }

                free(parameters);

                return NULL;
            }
        }
    }

    if (!parser_consume(
            parser,
            TOKEN_RIGHT_PAREN,
            "expected ')' after parameters."
        )) {
        free(name);

        for (int i = 0; i < parameter_count; i++) {
            free(parameters[i]);
        }

        free(parameters);

        return NULL;
    }

    if (!parser_consume(
            parser,
            TOKEN_LEFT_BRACE,
            "expected '{' before function body."
        )) {
        free(name);

        for (int i = 0; i < parameter_count; i++) {
            free(parameters[i]);
        }

        free(parameters);

        return NULL;
    }

    AstNode *body = parse_block(parser);

    if (
        body == NULL &&
        !parser_check(parser, TOKEN_EOF)
    ) {
        free(name);

        for (int i = 0; i < parameter_count; i++) {
            free(parameters[i]);
        }

        free(parameters);

        return NULL;
    }

    AstNode *node =
        ast_create_function_declaration(
            name,
            parameters,
            parameter_count,
            body
        );

    free(name);

    if (node == NULL) {
        for (int i = 0; i < parameter_count; i++) {
            free(parameters[i]);
        }

        free(parameters);
        ast_free(body);

        printf("Parser error: out of memory.\n");
        return NULL;
    }

    return node;
}

AstNode *parse_function_call(
    Parser *parser,
    char *name
) {
    if (!parser_consume(
            parser,
            TOKEN_LEFT_PAREN,
            "expected '(' after function name."
        )) {
        free(name);
        return NULL;
    }

    AstNode *arguments = NULL;
    int argument_count = 0;

    if (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
        for (;;) {
            AstNode *argument =
                parse_expression(parser);

            if (argument == NULL) {
                free(name);
                ast_free(arguments);
                return NULL;
            }

            ast_append(&arguments, argument);
            argument_count++;

            if (parser_check(
                    parser,
                    TOKEN_RIGHT_PAREN
                )) {
                break;
            }

            if (!parser_consume(
                    parser,
                    TOKEN_COMMA,
                    "expected ',' between arguments."
                )) {
                free(name);
                ast_free(arguments);
                return NULL;
            }
        }
    }

    if (!parser_consume(
            parser,
            TOKEN_RIGHT_PAREN,
            "expected ')' after arguments."
        )) {
        free(name);
        ast_free(arguments);
        return NULL;
    }

    AstNode *node =
        ast_create_function_call(
            name,
            arguments,
            argument_count
        );

    free(name);

    if (node == NULL) {
        ast_free(arguments);

        printf("Parser error: out of memory.\n");
        return NULL;
    }

    return node;
}

AstNode *parse_struct(Parser *parser) {
    parser_advance(parser);

    if (!parser_check(parser, TOKEN_IDENTIFIER)) {
        printf("Parser error: expected struct name.\n");
        return NULL;
    }

    char *name = parser_token_to_string(parser->current);
    if (!name) {
        printf("Parser error: out of memory.\n");
        return NULL;
    }
    parser_advance(parser);

    if (!parser_consume(parser, TOKEN_LEFT_BRACE, "expected '{' after struct name.")) {
        free(name);
        return NULL;
    }

    char **fields = NULL;
    int field_count = 0;
    int field_capacity = 0;

    if (!parser_check(parser, TOKEN_RIGHT_BRACE)) {
        for (;;) {
            if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                printf("Parser error: expected struct field name.\n");
                free(name);
                for (int i = 0; i < field_count; i++) free(fields[i]);
                free(fields);
                return NULL;
            }

            char *field = parser_token_to_string(parser->current);
            if (!field) {
                free(name);
                for (int i = 0; i < field_count; i++) free(fields[i]);
                free(fields);
                return NULL;
            }
            parser_advance(parser);

            if (field_count == field_capacity) {
                int cap = field_capacity ? field_capacity * 2 : 4;
                char **new_fields = realloc(fields, sizeof(char *) * cap);
                if (!new_fields) {
                    free(field);
                    free(name);
                    for (int i = 0; i < field_count; i++) free(fields[i]);
                    free(fields);
                    printf("Parser error: out of memory.\n");
                    return NULL;
                }
                fields = new_fields;
                field_capacity = cap;
            }

            fields[field_count++] = field;

            if (parser_check(parser, TOKEN_RIGHT_BRACE))
                break;

            if (!parser_consume(parser, TOKEN_COMMA, "expected ',' between struct fields.")) {
                free(name);
                for (int i = 0; i < field_count; i++) free(fields[i]);
                free(fields);
                return NULL;
            }
        }
    }

    if (!parser_consume(parser, TOKEN_RIGHT_BRACE, "expected '}' after struct definition.")) {
        free(name);
        for (int i = 0; i < field_count; i++) free(fields[i]);
        free(fields);
        return NULL;
    }

    AstNode *node = ast_create_struct_declaration(name, fields, field_count);
    free(name);

    if (!node) {
        for (int i = 0; i < field_count; i++) free(fields[i]);
        free(fields);
        printf("Parser error: out of memory.\n");
        return NULL;
    }

    return node;
}

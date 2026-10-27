#include <vnt/parser.h>

#include <stdio.h>
#include <stdlib.h>

static void free_function_parse_data(char *name, char **parameters,
    char **parameter_types, int count, char *return_type, AstNode *body) {
    free(name);
    for (int i = 0; i < count; ++i) {
        free(parameters ? parameters[i] : NULL);
        free(parameter_types ? parameter_types[i] : NULL);
    }
    free(parameters); free(parameter_types); free(return_type); ast_free(body);
}

AstNode *parse_function(Parser *parser) {
    parser_advance(parser);
    if (!parser_check(parser, TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Parser error: expected function name.\n"); return NULL;
    }
    char *name = parser_token_to_string(parser->current);
    if (!name) return NULL;
    parser_advance(parser);
    if (!parser_consume(parser, TOKEN_LEFT_PAREN, "expected '(' after function name.")) {
        free(name); return NULL;
    }
    char **parameters = NULL, **parameter_types = NULL;
    int count = 0, capacity = 0;
    char *return_type = NULL;
    AstNode *body = NULL;
    while (!parser_check(parser, TOKEN_RIGHT_PAREN)) {
        if (!parser_check(parser, TOKEN_IDENTIFIER)) {
            fprintf(stderr, "Parser error at %d:%d: expected parameter name.\n", parser->current.line, parser->current.column);
            free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
        }
        char *parameter = parser_token_to_string(parser->current);
        if (!parameter) { free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL; }
        parser_advance(parser);
        char *parameter_type = NULL;
        if (parser_check(parser, TOKEN_COLON)) {
            parser_advance(parser);
            if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                fprintf(stderr, "Parser error: expected parameter type after ':'.\n");
                free(parameter); free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
            }
            parameter_type = parser_token_to_string(parser->current);
            if (!parameter_type) { free(parameter); free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL; }
            parser_advance(parser);
        }
        if (count == capacity) {
            int next_capacity = capacity ? capacity * 2 : 4;
            char **next_parameters = realloc(parameters, sizeof(*parameters) * next_capacity);
            if (!next_parameters) {
                free(parameter); free(parameter_type); free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
            }
            parameters = next_parameters;
            char **next_types = realloc(parameter_types, sizeof(*parameter_types) * next_capacity);
            if (!next_types) {
                free(parameter); free(parameter_type); free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
            }
            parameter_types = next_types; capacity = next_capacity;
        }
        parameters[count] = parameter; parameter_types[count] = parameter_type; count++;
        if (parser_check(parser, TOKEN_RIGHT_PAREN)) break;
        if (!parser_consume(parser, TOKEN_COMMA, "expected ',' between parameters.")) {
            free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
        }
    }
    if (!parser_consume(parser, TOKEN_RIGHT_PAREN, "expected ')' after parameters.")) {
        free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
    }
    if (parser_check(parser, TOKEN_MINUS)) {
        parser_advance(parser);
        if (!parser_consume(parser, TOKEN_GREATER, "expected '>' after '-' in return type.")) {
            free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
        }
        if (!parser_check(parser, TOKEN_IDENTIFIER)) {
            fprintf(stderr, "Parser error: expected function return type.\n");
            free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
        }
        return_type = parser_token_to_string(parser->current);
        if (!return_type) { free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL; }
        parser_advance(parser);
    }
    if (!parser_consume(parser, TOKEN_LEFT_BRACE, "expected '{' before function body.")) {
        free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
    }
    body = parse_block(parser);
    if (!body && !parser_check(parser, TOKEN_EOF)) {
        free_function_parse_data(name, parameters, parameter_types, count, return_type, body); return NULL;
    }
    AstNode *node = ast_create_typed_function_declaration(name, parameters, parameter_types, count, return_type, body);
    if (!node) {
        free_function_parse_data(name, parameters, parameter_types, count, return_type, body);
        fprintf(stderr, "Parser error: out of memory while creating function.\n"); return NULL;
    }
    free(name); free(return_type);
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

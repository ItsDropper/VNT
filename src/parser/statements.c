#include <vnt/parser.h>
#include "../internal/parser_internal.h"

#include <stdio.h>
#include <stdlib.h>

AstNode *parse_block(Parser *parser) {
    AstNode *statements = NULL;

    while (
        !parser_check(parser, TOKEN_RIGHT_BRACE) &&
        !parser_check(parser, TOKEN_EOF)
    ) {
        AstNode *statement =
            parse_statement(parser);

        if (statement == NULL) {
            ast_free(statements);
            return NULL;
        }

        ast_append(
            &statements,
            statement
        );
    }

    if (!parser_consume(
            parser,
            TOKEN_RIGHT_BRACE,
            "expected '}'."
        )) {
        ast_free(statements);
        return NULL;
    }

    return statements;
}

static AstNode *parse_let_declaration(Parser *parser) {
    parser_advance(parser); /* consume let */

    if (!parser_consume(parser, TOKEN_IDENTIFIER,
                        "expected variable name after 'let'.")) {
        return NULL;
    }
    char *name = parser_token_to_string(parser->previous);
    if (!name) {
        fprintf(stderr, "Parser error: out of memory.\n");
        return NULL;
    }

    if (!parser_consume(parser, TOKEN_COLON,
                        "expected ':' after typed variable name.")) {
        free(name);
        return NULL;
    }
    if (!parser_consume(parser, TOKEN_IDENTIFIER,
                        "expected type name after ':'.")) {
        free(name);
        return NULL;
    }
    char *type = parser_token_to_string(parser->previous);
    if (!type) {
        free(name);
        fprintf(stderr, "Parser error: out of memory.\n");
        return NULL;
    }

    if (!parser_consume(parser, TOKEN_EQUALS,
                        "expected '=' after variable type.")) {
        free(name);
        free(type);
        return NULL;
    }

    AstNode *value = parse_expression(parser);
    if (!value) {
        free(name);
        free(type);
        return NULL;
    }

    AstNode *declaration =
        ast_create_typed_variable_declaration(name, type, value);
    free(name);
    free(type);
    if (!declaration) {
        ast_free(value);
        fprintf(stderr, "Parser error: out of memory.\n");
    }
    return declaration;
}

AstNode *parse_statement(Parser *parser) {
    if (parser_is_token(parser->current, "let")) return parse_let_declaration(parser);
    if (parser_is_token(parser->current, "print")) return parse_print(parser);
    if (parser_is_token(parser->current, "return")) return parse_return(parser);
    if (parser_is_token(parser->current, "break")) return parse_break(parser);
    if (parser_is_token(parser->current, "continue")) return parse_continue(parser);
    if (parser_is_token(parser->current, "if")) return parse_if(parser);
    if (parser_is_token(parser->current, "while")) return parse_while(parser);
    if (parser_is_token(parser->current, "for")) return parse_for(parser);
    if (parser_is_token(parser->current, "fun")) return parse_function(parser);
    if (parser_is_token(parser->current, "struct")) return parse_struct(parser);
    if (parser_check(parser, TOKEN_STAR)) return parse_dereference_statement(parser);
    if (parser_check(parser, TOKEN_IDENTIFIER)) return parse_identifier_statement(parser);

    fprintf(stderr, "Parser error at %d:%d: expected a statement.\n",
            parser->current.line, parser->current.column);
    return NULL;
}

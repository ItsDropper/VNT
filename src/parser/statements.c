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

static AstNode *parse_asm_statement(Parser *parser) {
    parser_advance(parser); /* consume asm */
    int is_volatile = 0;
    if (parser_is_token(parser->current, "volatile")) {
        is_volatile = 1;
        parser_advance(parser);
    }
    if (!parser_consume(parser, TOKEN_LEFT_PAREN,
                        "expected '(' after asm or asm volatile.")) return NULL;
    if (!parser_consume(parser, TOKEN_STRING,
                        "expected an assembly template string.")) return NULL;
    char *assembly = parser_token_to_string(parser->previous);
    if (!assembly) {
        fprintf(stderr, "Parser error: out of memory.\n");
        return NULL;
    }
    if (!parser_consume(parser, TOKEN_RIGHT_PAREN,
                        "expected ')' after assembly template.")) {
        free(assembly);
        return NULL;
    }
    AstNode *node = ast_create_asm_statement(assembly, is_volatile);
    free(assembly);
    if (!node) fprintf(stderr, "Parser error: out of memory.\n");
    return node;
}

static AstNode *parse_unsafe_block(Parser *parser) {
    parser_advance(parser); /* consume unsafe */
    if (parser_is_token(parser->current, "fun")) {
        AstNode *function = parse_function(parser);
        if (function) function->function_declaration.is_unsafe = 1;
        return function;
    }
    if (!parser_consume(parser, TOKEN_LEFT_BRACE, "expected '{' or 'fun' after unsafe.")) return NULL;
    AstNode *statements = parse_block(parser);
    if (!statements && parser_check(parser, TOKEN_EOF)) return NULL;
    AstNode *node = ast_create_unsafe_block(statements);
    if (!node) {
        ast_free(statements);
        fprintf(stderr, "Parser error: out of memory.\n");
    }
    return node;
}

AstNode *parse_statement(Parser *parser) {
    if (!parser_enter_nesting(parser, "statement"))
        return NULL;

    AstNode *statement = NULL;
    if (parser_is_token(parser->current, "unsafe")) statement = parse_unsafe_block(parser);
    else if (parser_is_token(parser->current, "asm")) statement = parse_asm_statement(parser);
    else if (parser_is_token(parser->current, "let")) statement = parse_let_declaration(parser);
    else if (parser_is_token(parser->current, "print")) statement = parse_print(parser);
    else if (parser_is_token(parser->current, "return")) statement = parse_return(parser);
    else if (parser_is_token(parser->current, "break")) statement = parse_break(parser);
    else if (parser_is_token(parser->current, "continue")) statement = parse_continue(parser);
    else if (parser_is_token(parser->current, "if")) statement = parse_if(parser);
    else if (parser_is_token(parser->current, "while")) statement = parse_while(parser);
    else if (parser_is_token(parser->current, "for")) statement = parse_for(parser);
    else if (parser_is_token(parser->current, "fun")) statement = parse_function(parser);
    else if (parser_is_token(parser->current, "struct")) statement = parse_struct(parser);
    else if (parser_check(parser, TOKEN_STAR)) statement = parse_dereference_statement(parser);
    else if (parser_check(parser, TOKEN_IDENTIFIER)) statement = parse_identifier_statement(parser);
    else {
        fprintf(stderr, "Parser error at %d:%d: expected a statement.\n",
                parser->current.line, parser->current.column);
    }

    if (parser->nesting_failed) {
        ast_free(statement);
        statement = NULL;
    }
    parser_leave_nesting(parser);
    return statement;
}

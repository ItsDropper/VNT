#include <vnt/parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parser_enter_nesting(Parser *parser, const char *context) {
    if (!parser || parser->nesting_depth >= VNT_PARSER_MAX_NESTING) {
        if (parser) {
            fprintf(stderr,
                    "Parser error at %d:%d: %s nesting exceeds %u levels.\\n",
                    parser->current.line, parser->current.column,
                    context ? context : "syntax",
                    VNT_PARSER_MAX_NESTING);
        }
        return 0;
    }
    ++parser->nesting_depth;
    return 1;
}

void parser_leave_nesting(Parser *parser) {
    if (parser && parser->nesting_depth > 0)
        --parser->nesting_depth;
}

void parser_advance(Parser *parser) {
    parser->previous = parser->current;
    parser->current = lexer_next(parser->lexer);
}

int parser_check(Parser *parser, TokenType type) {
    return parser->current.type == type;
}

int parser_consume(
    Parser *parser,
    TokenType type,
    const char *message
) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return 1;
    }

    fprintf(stderr, "Parser error at %d:%d: %s\n",
            parser->current.line, parser->current.column, message);
    return 0;
}

char *parser_token_to_string(Token token) {
    char *value = malloc(token.length + 1);

    if (value == NULL) {
        return NULL;
    }

    memcpy(value, token.start, token.length);
    value[token.length] = '\0';

    return value;
}

int parser_is_token(
    Token token,
    const char *text
) {
    size_t length = strlen(text);

    return token.type == TOKEN_IDENTIFIER &&
           token.length == (int)length &&
           strncmp(token.start, text, length) == 0;
}

void parser_init(Parser *parser, Lexer *lexer) {
    parser->lexer = lexer;
    parser->current.type = TOKEN_UNKNOWN;
    parser->previous.type = TOKEN_UNKNOWN;
    parser->nesting_depth = 0;

    parser_advance(parser);
}

AstNode *parser_parse(Parser *parser) {
    AstNode *statements = NULL;

    while (!parser_check(parser, TOKEN_EOF)) {
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

        fprintf(stderr, "Parser error at %d:%d: out of memory.\n",
                parser->current.line, parser->current.column);
        return NULL;
    }

    return program;
}
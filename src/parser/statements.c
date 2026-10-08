#include <vnt/parser.h>
#include "../internal/parser_internal.h"

#include <stdio.h>

AstNode *parse_statement(Parser *parser) {
    if (parser_is_token(parser->current, "print")) return parse_print(parser);
    if (parser_is_token(parser->current, "return")) return parse_return(parser);
    if (parser_is_token(parser->current, "break")) return parse_break(parser);
    if (parser_is_token(parser->current, "continue")) return parse_continue(parser);
    if (parser_is_token(parser->current, "if")) return parse_if(parser);
    if (parser_is_token(parser->current, "while")) return parse_while(parser);
    if (parser_is_token(parser->current, "for")) return parse_for(parser);
    if (parser_is_token(parser->current, "fun")) return parse_function(parser);
    if (parser_check(parser, TOKEN_IDENTIFIER)) return parse_identifier_statement(parser);

    printf("Parser error: expected a statement.\n");
    return NULL;
}

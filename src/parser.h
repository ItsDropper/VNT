
#ifndef VNT_PARSER_H
#define VNT_PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer *lexer;
    Token current;
    Token previous;
} Parser;

void parser_init(Parser *parser, Lexer *lexer);
AstNode *parser_parse(Parser *parser);

#endif


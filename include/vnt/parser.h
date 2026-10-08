#ifndef VNT_PARSER_H
#define VNT_PARSER_H

#include <vnt/lexer.h>
#include <vnt/ast.h>

typedef struct {
    Lexer *lexer;
    Token current;
    Token previous;
} Parser;

void parser_init(Parser *parser, Lexer *lexer);
AstNode *parser_parse(Parser *parser);

void parser_advance(Parser *parser);
int parser_check(Parser *parser, TokenType type);
int parser_consume(Parser *parser, TokenType type, const char *message);
char *parser_token_to_string(Token token);
int parser_is_token(Token token, const char *text);

AstNode *parse_expression(Parser *parser);
AstNode *parse_statement(Parser *parser);
AstNode *parse_block(Parser *parser);
AstNode *parse_function(Parser *parser);
AstNode *parse_function_call(Parser *parser, char *name);
AstNode *parse_return(Parser *parser);

#endif

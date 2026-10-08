#ifndef VNT_PARSER_INTERNAL_H
#define VNT_PARSER_INTERNAL_H
#include <vnt/parser.h>
AstNode *parse_if(Parser *parser); AstNode *parse_while(Parser *parser); AstNode *parse_for(Parser *parser);
AstNode *parse_print(Parser *parser); AstNode *parse_break(Parser *parser); AstNode *parse_continue(Parser *parser);
AstNode *parse_identifier_statement(Parser *parser); AstNode *parse_unary(Parser *parser); AstNode *parse_postfix(Parser *parser);
#endif

#include <vnt/parser.h>
#include "../internal/parser_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AstNode *parse_unary(Parser *parser) {
    if (
        token_is(parser, TOKEN_BANG) ||
        token_is(parser, TOKEN_MINUS) ||
        token_is(parser, TOKEN_AMPERSAND) ||
        token_is(parser, TOKEN_STAR)
    ) {
        TokenType operator = parser->current.type;

        parser_advance(parser);

        AstNode *operand =
            parse_unary(parser);

        if (operand == NULL) {
            return NULL;
        }

        AstNode *node =
            ast_create_unary(
                operand,
                operator == TOKEN_BANG
                    ? UNARY_NOT
                    : operator == TOKEN_MINUS
                        ? UNARY_NEGATE
                        : operator == TOKEN_AMPERSAND
                            ? UNARY_REFERENCE
                            : UNARY_DEREFERENCE
            );

        if (node == NULL) {
            ast_free(operand);
            return NULL;
        }

        return node;
    }

    return parse_postfix(parser);
}


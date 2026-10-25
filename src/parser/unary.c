#include <vnt/parser.h>
#include "../internal/parser_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>

AstNode *parse_unary(Parser *parser) {
    if (
        token_is(parser, TOKEN_BANG) ||
        token_is(parser, TOKEN_MINUS) ||
        token_is(parser, TOKEN_AMPERSAND) ||
        token_is(parser, TOKEN_STAR)
    ) {
        TokenType operator = parser->current.type;

        parser_advance(parser);

        /*
         * The positive magnitude of INT64_MIN is not representable as int64_t,
         * but the signed literal itself is valid. Recognize it before parsing
         * the operand as a positive integer literal.
         */
        if (operator == TOKEN_MINUS && token_is(parser, TOKEN_INTEGER)) {
            Token token = parser->current;
            char buffer[64];
            if (token.length > 0 && token.length < (int)sizeof(buffer)) {
                memcpy(buffer, token.start, token.length);
                buffer[token.length] = '\0';
                errno = 0;
                char *end = NULL;
                int base = (buffer[0] == '0' &&
                    (buffer[1] == 'x' || buffer[1] == 'X')) ? 16 : 10;
                unsigned long long magnitude = strtoull(buffer, &end, base);
                if (errno == 0 && end != buffer && *end == '\0' &&
                    magnitude == (unsigned long long)INT64_MAX + 1ULL) {
                    parser_advance(parser);
                    return ast_create_integer(INT64_MIN);
                }
            }
        }

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


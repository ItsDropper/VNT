#include "lexer.h"

#include <ctype.h>
#include <string.h>

static char peek(Lexer *lexer) {
    return lexer->source[lexer->current];
}

static char advance(Lexer *lexer) {
    return lexer->source[lexer->current++];
}

static void skip_whitespace(Lexer *lexer) {
    for (;;) {
        char c = peek(lexer);

        if (isspace((unsigned char)c)) {
            advance(lexer);
            continue;
        }

        if (c == '#') {
            while (
                peek(lexer) != '\0' &&
                peek(lexer) != '\n'
            ) {
                advance(lexer);
            }

            continue;
        }

        break;
    }
}

static Token make_token(
    Lexer *lexer,
    TokenType type,
    const char *start
) {
    Token token;

    token.type = type;
    token.start = start;
    token.length =
        (int)(lexer->source + lexer->current - start);

    return token;
}

static Token identifier(
    Lexer *lexer,
    const char *start
) {
    while (
        isalnum((unsigned char)peek(lexer)) ||
        peek(lexer) == '_'
    ) {
        advance(lexer);
    }

    return make_token(
        lexer,
        TOKEN_IDENTIFIER,
        start
    );
}

static Token number(
    Lexer *lexer,
    const char *start
) {
    while (
        isdigit((unsigned char)peek(lexer))
    ) {
        advance(lexer);
    }

    return make_token(
        lexer,
        TOKEN_INTEGER,
        start
    );
}

static Token string(
    Lexer *lexer,
    const char *start
) {
    while (
        peek(lexer) != '"' &&
        peek(lexer) != '\0'
    ) {
        advance(lexer);
    }

    if (peek(lexer) == '\0') {
        return make_token(
            lexer,
            TOKEN_UNKNOWN,
            start
        );
    }

    advance(lexer);

    Token token =
        make_token(
            lexer,
            TOKEN_STRING,
            start
        );

    token.start++;
    token.length -= 2;

    return token;
}

void lexer_init(
    Lexer *lexer,
    const char *source
) {
    lexer->source = source;
    lexer->current = 0;
}

Token lexer_next(Lexer *lexer) {
    skip_whitespace(lexer);

    const char *start =
        lexer->source + lexer->current;

    char c = advance(lexer);

    if (c == '\0') {
        return make_token(
            lexer,
            TOKEN_EOF,
            start
        );
    }

    if (
        isalpha((unsigned char)c) ||
        c == '_'
    ) {
        return identifier(
            lexer,
            start
        );
    }

    if (
        isdigit((unsigned char)c)
    ) {
        return number(
            lexer,
            start
        );
    }

    if (c == '"') {
        return string(
            lexer,
            start
        );
    }

    switch (c) {
        case '(':
            return make_token(
                lexer,
                TOKEN_LEFT_PAREN,
                start
            );

        case ')':
            return make_token(
                lexer,
                TOKEN_RIGHT_PAREN,
                start
            );

        case '{':
            return make_token(
                lexer,
                TOKEN_LEFT_BRACE,
                start
            );

        case '}':
            return make_token(
                lexer,
                TOKEN_RIGHT_BRACE,
                start
            );

        case '[':
            return make_token(
                lexer,
                TOKEN_LEFT_BRACKET,
                start
            );

        case ']':
            return make_token(
                lexer,
                TOKEN_RIGHT_BRACKET,
                start
            );

        case ',':
            return make_token(
                lexer,
                TOKEN_COMMA,
                start
            );

        case '=':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_EQUAL_EQUAL,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_EQUALS,
                start
            );

        case '!':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_BANG_EQUAL,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_BANG,
                start
            );

        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_GREATER_EQUAL,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_GREATER,
                start
            );

        case '<':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_LESS_EQUAL,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_LESS,
                start
            );

        case '+':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_PLUS_EQUALS,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_PLUS,
                start
            );

        case '-':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_MINUS_EQUALS,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_MINUS,
                start
            );

        case '*':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_STAR_EQUALS,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_STAR,
                start
            );

        case '/':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_SLASH_EQUALS,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_SLASH,
                start
            );

        case '%':
            if (peek(lexer) == '=') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_PERCENT_EQUALS,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_PERCENT,
                start
            );

        case '&':
            if (peek(lexer) == '&') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_AND_AND,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_UNKNOWN,
                start
            );

        case '|':
            if (peek(lexer) == '|') {
                advance(lexer);

                return make_token(
                    lexer,
                    TOKEN_OR_OR,
                    start
                );
            }

            return make_token(
                lexer,
                TOKEN_UNKNOWN,
                start
            );

        default:
            return make_token(
                lexer,
                TOKEN_UNKNOWN,
                start
            );
    }
}
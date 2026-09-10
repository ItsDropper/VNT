#ifndef VNT_LEXER_H
#define VNT_LEXER_H

typedef enum {
    TOKEN_EOF,

    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_INTEGER,

    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_COMMA,

    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,

    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,

    TOKEN_EQUALS,
    TOKEN_EQUAL_EQUAL,
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,

    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS_EQUAL,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    TOKEN_AND_AND,
    TOKEN_OR_OR,

    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
} Token;

typedef struct {
    const char *source;
    int current;
} Lexer;

void lexer_init(Lexer *lexer, const char *source);

Token lexer_next(Lexer *lexer);

#endif
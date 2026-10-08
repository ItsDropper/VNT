#include <vnt/parser.h>
#include <stdio.h>

static AstNode *parse_print(Parser *parser) {
    parser_advance(parser);

    if (!parser_consume(
            parser,
            TOKEN_LEFT_PAREN,
            "expected '('."
        )) {
        return NULL;
    }

    AstNode *expression =
        parse_expression(parser);

    if (expression == NULL) {
        return NULL;
    }

    if (!parser_consume(
            parser,
            TOKEN_RIGHT_PAREN,
            "expected ')'."
        )) {
        ast_free(expression);
        return NULL;
    }

    AstNode *node =
        ast_create_print(expression);

    if (node == NULL) {
        ast_free(expression);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    return node;
}

AstNode *parse_return(Parser *parser) {
    parser_advance(parser);

    AstNode *expression =
        parse_expression(parser);

    if (expression == NULL) {
        printf(
            "Parser error: expected expression after return.\n"
        );

        return NULL;
    }

    AstNode *node =
        ast_create_return(expression);

    if (node == NULL) {
        ast_free(expression);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    return node;
}

static AstNode *parse_break(Parser *parser) {
    parser_advance(parser);

    AstNode *node =
        ast_create_break();

    if (node == NULL) {
        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    return node;
}

static AstNode *parse_continue(Parser *parser) {
    parser_advance(parser);

    AstNode *node =
        ast_create_continue();

    if (node == NULL) {
        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    return node;
}


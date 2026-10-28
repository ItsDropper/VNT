#include <vnt/parser.h>
#include <stdio.h>

AstNode *parse_print(Parser *parser) {
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

    /* Newlines are not tokens, so use source locations to distinguish a
       bare return from a value expression that starts on the next line. */
    int bare_return =
        parser_check(parser, TOKEN_RIGHT_BRACE) ||
        parser_check(parser, TOKEN_EOF) ||
        parser->current.line > parser->previous.line;
    AstNode *expression = bare_return ? NULL : parse_expression(parser);

    if (!bare_return && expression == NULL) {
        fprintf(stderr,
                "Parser error at %d:%d: expected expression after return.\n",
                parser->current.line, parser->current.column);
        return NULL;
    }

    AstNode *node = ast_create_return(expression);
    if (node == NULL) {
        ast_free(expression);
        fprintf(stderr, "Parser error: out of memory.\n");
        return NULL;
    }

    return node;
}

AstNode *parse_break(Parser *parser) {
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

AstNode *parse_continue(Parser *parser) {
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


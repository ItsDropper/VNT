#include <vnt/parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AstNode *parse_if(Parser *parser) {
    parser_advance(parser);

    AstNode *condition =
        parse_expression(parser);

    if (condition == NULL) {
        return NULL;
    }

    if (!parser_consume(
            parser,
            TOKEN_LEFT_BRACE,
            "expected '{' after if condition."
        )) {
        ast_free(condition);
        return NULL;
    }

    AstNode *then_branch =
        parse_block(parser);

    if (
        then_branch == NULL &&
        !parser_check(parser, TOKEN_RIGHT_BRACE) &&
        !parser_check(parser, TOKEN_EOF)
    ) {
        ast_free(condition);
        return NULL;
    }

    AstNode *else_branch = NULL;

    if (parser_is_token(
            parser->current,
            "else"
        )) {
        parser_advance(parser);

        if (parser_is_token(
                parser->current,
                "if"
            )) {
            else_branch =
                parse_if(parser);

            if (else_branch == NULL) {
                ast_free(condition);
                ast_free(then_branch);
                return NULL;
            }
        } else {
            if (!parser_consume(
                    parser,
                    TOKEN_LEFT_BRACE,
                    "expected '{' or 'if' after else."
                )) {
                ast_free(condition);
                ast_free(then_branch);
                return NULL;
            }

            else_branch =
                parse_block(parser);

            if (
                else_branch == NULL &&
                !parser_check(parser, TOKEN_EOF)
            ) {
                ast_free(condition);
                ast_free(then_branch);
                return NULL;
            }
        }
    }

    AstNode *node =
        ast_create_if(
            condition,
            then_branch,
            else_branch
        );

    if (node == NULL) {
        ast_free(condition);
        ast_free(then_branch);
        ast_free(else_branch);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    return node;
}

AstNode *parse_while(Parser *parser) {
    parser_advance(parser);

    AstNode *condition =
        parse_expression(parser);

    if (condition == NULL) {
        return NULL;
    }

    if (!parser_consume(
            parser,
            TOKEN_LEFT_BRACE,
            "expected '{' after while condition."
        )) {
        ast_free(condition);
        return NULL;
    }

    AstNode *body =
        parse_block(parser);

    if (
        body == NULL &&
        !parser_check(parser, TOKEN_EOF)
    ) {
        ast_free(condition);
        return NULL;
    }

    AstNode *node =
        ast_create_while(
            condition,
            body
        );

    if (node == NULL) {
        ast_free(condition);
        ast_free(body);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    return node;
}


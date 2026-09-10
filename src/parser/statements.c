#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

AstNode *parse_block(Parser *parser) {
    AstNode *statements = NULL;

    while (
        !parser_check(parser, TOKEN_RIGHT_BRACE) &&
        !parser_check(parser, TOKEN_EOF)
    ) {
        AstNode *statement =
            parse_statement(parser);

        if (statement == NULL) {
            ast_free(statements);
            return NULL;
        }

        ast_append(
            &statements,
            statement
        );
    }

    if (!parser_consume(
            parser,
            TOKEN_RIGHT_BRACE,
            "expected '}'."
        )) {
        ast_free(statements);
        return NULL;
    }

    return statements;
}

static AstNode *parse_if(Parser *parser) {
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

        /*
         * Support:
         *
         * else {
         * }
         *
         * and:
         *
         * else if (...) {
         * }
         */
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

static AstNode *parse_while(Parser *parser) {
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

AstNode *parse_statement(Parser *parser) {
    if (parser_is_token(
            parser->current,
            "print"
        )) {
        return parse_print(parser);
    }

    if (parser_is_token(
            parser->current,
            "return"
        )) {
        return parse_return(parser);
    }

    if (parser_is_token(
            parser->current,
            "break"
        )) {
        return parse_break(parser);
    }

    if (parser_is_token(
            parser->current,
            "continue"
        )) {
        return parse_continue(parser);
    }

    if (parser_is_token(
            parser->current,
            "if"
        )) {
        return parse_if(parser);
    }

    if (parser_is_token(
            parser->current,
            "while"
        )) {
        return parse_while(parser);
    }

    if (parser_is_token(
            parser->current,
            "fun"
        )) {
        return parse_function(parser);
    }

    if (parser_check(
            parser,
            TOKEN_IDENTIFIER
        )) {
        char *name =
            parser_token_to_string(
                parser->current
            );

        if (name == NULL) {
            printf(
                "Parser error: out of memory.\n"
            );

            return NULL;
        }

        parser_advance(parser);

        if (parser_check(
                parser,
                TOKEN_LEFT_PAREN
            )) {
            return parse_function_call(
                parser,
                name
            );
        }

        AstNode *target =
            ast_create_variable(name);

        free(name);

        if (target == NULL) {
            printf(
                "Parser error: out of memory.\n"
            );

            return NULL;
        }

        /*
         * Parse any number of indexing operations.
         *
         * Example:
         *
         * numbers[0]
         * matrix[1][2]
         */
        while (
            parser_check(
                parser,
                TOKEN_LEFT_BRACKET
            )
        ) {
            parser_advance(parser);

            AstNode *index =
                parse_expression(parser);

            if (index == NULL) {
                ast_free(target);

                printf(
                    "Parser error: expected array index.\n"
                );

                return NULL;
            }

            if (!parser_consume(
                    parser,
                    TOKEN_RIGHT_BRACKET,
                    "expected ']' after array index."
                )) {
                ast_free(target);
                ast_free(index);
                return NULL;
            }

            AstNode *indexed =
                ast_create_index(
                    target,
                    index
                );

            if (indexed == NULL) {
                ast_free(target);
                ast_free(index);

                printf(
                    "Parser error: out of memory.\n"
                );

                return NULL;
            }

            target = indexed;
        }

        if (!parser_consume(
                parser,
                TOKEN_EQUALS,
                "expected '='."
            )) {
            ast_free(target);
            return NULL;
        }

        AstNode *value =
            parse_expression(parser);

        if (value == NULL) {
            ast_free(target);
            return NULL;
        }

        /*
         * Plain identifier assignment uses the existing
         * variable-declaration representation.
         */
        if (target->type == AST_VARIABLE) {
            AstNode *node =
                ast_create_variable_declaration(
                    target->variable.name,
                    value
                );

            ast_free(target);

            if (node == NULL) {
                ast_free(value);

                printf(
                    "Parser error: out of memory.\n"
                );

                return NULL;
            }

            return node;
        }

        /*
         * Indexed assignment:
         *
         * numbers[1] = 42
         */
        AstNode *node =
            ast_create_assignment(
                target,
                value
            );

        if (node == NULL) {
            ast_free(target);
            ast_free(value);

            printf(
                "Parser error: out of memory.\n"
            );

            return NULL;
        }

        return node;
    }

    printf(
        "Parser error: expected a statement.\n"
    );

    return NULL;
}
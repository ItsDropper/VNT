#include <vnt/parser.h>
#include "../internal/parser_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
AstNode *parse_for(Parser *parser) {
    /*
     * Syntax:
     *
     * for item in expression {
     *     ...
     * }
     *
     * Example:
     *
     * for i in range(10) {
     *     print(i)
     * }
     *
     * This is lowered into existing VNT AST nodes:
     *
     * items = expression
     * index = 0
     *
     * while index < len(items) {
     *     item = items[index]
     *     body
     *     index = index + 1
     * }
     */

    parser_advance(parser);

    if (!parser_check(
            parser,
            TOKEN_IDENTIFIER
        )) {
        printf(
            "Parser error: expected loop variable after for.\n"
        );

        return NULL;
    }

    char *loop_variable =
        parser_token_to_string(
            parser->current
        );

    if (loop_variable == NULL) {
        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    parser_advance(parser);

    if (!parser_is_token(
            parser->current,
            "in"
        )) {
        free(loop_variable);

        printf(
            "Parser error: expected 'in' after for variable.\n"
        );

        return NULL;
    }

    parser_advance(parser);

    AstNode *iterable =
        parse_expression(parser);

    if (iterable == NULL) {
        free(loop_variable);
        return NULL;
    }

    if (!parser_consume(
            parser,
            TOKEN_LEFT_BRACE,
            "expected '{' after for expression."
        )) {
        free(loop_variable);
        ast_free(iterable);
        return NULL;
    }

    AstNode *body =
        parse_block(parser);

    if (
        body == NULL &&
        !parser_check(parser, TOKEN_EOF)
    ) {
        free(loop_variable);
        ast_free(iterable);
        return NULL;
    }

    for_counter++;

    char items_name[64];
    char index_name[64];

    snprintf(
        items_name,
        sizeof(items_name),
        "__vnt_for_items_%d",
        for_counter
    );

    snprintf(
        index_name,
        sizeof(index_name),
        "__vnt_for_index_%d",
        for_counter
    );

    /*
     * items = iterable
     */
    AstNode *items_declaration =
        ast_create_variable_declaration(
            items_name,
            iterable
        );

    if (items_declaration == NULL) {
        free(loop_variable);
        ast_free(body);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    /*
     * index = -1
     *
     * The generated loop increments the index before
     * executing the user's body. This makes continue
     * safe: the increment is not skipped by the continue
     * signal.
     */
    AstNode *index_zero =
        ast_create_integer(-1);

    AstNode *index_declaration =
        ast_create_variable_declaration(
            index_name,
            index_zero
        );

    if (index_declaration == NULL) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(body);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    /*
     * len(items)
     */
    AstNode *length_arguments = NULL;

    AstNode *items_variable =
        ast_create_variable(items_name);

    if (items_variable == NULL) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(body);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    ast_append(
        &length_arguments,
        items_variable
    );

    AstNode *length_call =
        ast_create_function_call(
            "len",
            length_arguments,
            1
        );

    if (length_call == NULL) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(body);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    /*
     * index + 1 < len(items)
     *
     * The index starts at -1 and is incremented before
     * each user iteration.
     */
    AstNode *condition_index =
        ast_create_variable(index_name);

    AstNode *condition_step =
        ast_create_integer(1);

    if (
        condition_index == NULL ||
        condition_step == NULL
    ) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(length_call);
        ast_free(condition_index);
        ast_free(condition_step);
        ast_free(body);

        return NULL;
    }

    AstNode *condition_left =
        ast_create_binary(
            condition_index,
            condition_step,
            BINARY_ADD
        );

    if (condition_left == NULL) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(length_call);
        ast_free(body);

        return NULL;
    }

    AstNode *condition =
        ast_create_binary(
            condition_left,
            length_call,
            BINARY_LESS
        );

    if (condition == NULL) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(body);

        return NULL;
    }

    /*
     * items[index]
     */
    AstNode *array_variable =
        ast_create_variable(items_name);

    AstNode *index_variable =
        ast_create_variable(index_name);

    if (
        array_variable == NULL ||
        index_variable == NULL
    ) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(condition);
        ast_free(array_variable);
        ast_free(index_variable);
        ast_free(body);

        return NULL;
    }

    AstNode *item_expression =
        ast_create_index(
            array_variable,
            index_variable
        );

    if (item_expression == NULL) {
        free(loop_variable);
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(condition);
        ast_free(body);

        return NULL;
    }

    /*
     * item = items[index]
     */
    AstNode *item_declaration =
        ast_create_variable_declaration(
            loop_variable,
            item_expression
        );

    free(loop_variable);

    if (item_declaration == NULL) {
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(condition);
        ast_free(body);

        return NULL;
    }

    /*
     * index + 1
     */
    AstNode *increment_left =
        ast_create_variable(index_name);

    AstNode *increment_right =
        ast_create_integer(1);

    if (
        increment_left == NULL ||
        increment_right == NULL
    ) {
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(condition);
        ast_free(item_declaration);
        ast_free(increment_left);
        ast_free(increment_right);
        ast_free(body);

        return NULL;
    }

    AstNode *increment_expression =
        ast_create_binary(
            increment_left,
            increment_right,
            BINARY_ADD
        );

    if (increment_expression == NULL) {
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(condition);
        ast_free(item_declaration);
        ast_free(increment_left);
        ast_free(increment_right);
        ast_free(body);

        return NULL;
    }

    /*
     * index = index + 1
     *
     * The parser normally turns plain assignment
     * into AST_VARIABLE_DECLARATION, which is exactly
     * what we need here because environment_define()
     * replaces the existing variable.
     */
    AstNode *increment =
        ast_create_variable_declaration(
            index_name,
            increment_expression
        );

    if (increment == NULL) {
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(condition);
        ast_free(item_declaration);
        ast_free(body);

        return NULL;
    }

    /*
     * The index increment must happen before the user's
     * body. If it were after the body, continue would skip
     * it and the generated for-loop could run forever.
     *
     * Order:
     *
     *     index = index + 1
     *     item = items[index]
     *     user body
     */
    increment->next = item_declaration;
    item_declaration->next = body;
    body = increment;

    AstNode *while_node =
        ast_create_while(
            condition,
            body
        );

    if (while_node == NULL) {
        ast_free(items_declaration);
        ast_free(index_declaration);
        ast_free(body);

        printf(
            "Parser error: out of memory.\n"
        );

        return NULL;
    }

    /*
     * Return the generated statements as one chain.
     */
    ast_append(
        &items_declaration,
        index_declaration
    );

    ast_append(
        &items_declaration,
        while_node
    );

    return items_declaration;
}

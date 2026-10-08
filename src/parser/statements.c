#include <vnt/parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int for_counter = 0;

static AstNode *clone_expression(AstNode *node) {
    if (node == NULL) {
        return NULL;
    }

    switch (node->type) {
        case AST_VARIABLE:
            return ast_create_variable(node->variable.name);

        case AST_INTEGER_LITERAL:
            return ast_create_integer(
                node->integer_literal.value
            );

        case AST_BOOLEAN_LITERAL:
            return ast_create_boolean(
                node->boolean_literal.value
            );

        case AST_STRING_LITERAL:
            return ast_create_string(
                node->string_literal.value
            );

        case AST_INDEX_EXPRESSION: {
            AstNode *array =
                clone_expression(
                    node->index_expression.array
                );

            AstNode *index =
                clone_expression(
                    node->index_expression.index
                );

            if (array == NULL || index == NULL) {
                ast_free(array);
                ast_free(index);
                return NULL;
            }

            AstNode *copy =
                ast_create_index(
                    array,
                    index
                );

            if (copy == NULL) {
                ast_free(array);
                ast_free(index);
            }

            return copy;
        }

        case AST_BINARY_EXPRESSION: {
            AstNode *left =
                clone_expression(
                    node->binary_expression.left
                );

            AstNode *right =
                clone_expression(
                    node->binary_expression.right
                );

            if (left == NULL || right == NULL) {
                ast_free(left);
                ast_free(right);
                return NULL;
            }

            AstNode *copy =
                ast_create_binary(
                    left,
                    right,
                    node->binary_expression.operator
                );

            if (copy == NULL) {
                ast_free(left);
                ast_free(right);
            }

            return copy;
        }

        case AST_UNARY_EXPRESSION: {
            AstNode *operand =
                clone_expression(
                    node->unary_expression.operand
                );

            if (operand == NULL) {
                return NULL;
            }

            AstNode *copy =
                ast_create_unary(
                    operand,
                    node->unary_expression.operator
                );

            if (copy == NULL) {
                ast_free(operand);
            }

            return copy;
        }

        default:
            return NULL;
    }
}

static AstNode *clone_assignment_target(AstNode *node) {
    return clone_expression(node);
}

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

static AstNode *parse_for(Parser *parser) {
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
            "for"
        )) {
        return parse_for(parser);
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

        /*
         * Read the assignment operator.
         */
        TokenType assignment_operator =
            parser->current.type;

        if (
            assignment_operator != TOKEN_EQUALS &&
            assignment_operator != TOKEN_PLUS_EQUALS &&
            assignment_operator != TOKEN_MINUS_EQUALS &&
            assignment_operator != TOKEN_STAR_EQUALS &&
            assignment_operator != TOKEN_SLASH_EQUALS &&
            assignment_operator != TOKEN_PERCENT_EQUALS
        ) {
            ast_free(target);

            printf(
                "Parser error: expected assignment operator.\n"
            );

            return NULL;
        }

        parser_advance(parser);

        AstNode *value =
            parse_expression(parser);

        if (value == NULL) {
            ast_free(target);
            return NULL;
        }

        /*
         * Convert compound assignment:
         *
         * x += 5
         * numbers[0] += 5
         *
         * into the existing assignment AST:
         *
         * x = x + 5
         * numbers[0] = numbers[0] + 5
         *
         * Cloning the target is required because the original
         * target remains owned by the assignment node.
         */
        if (
            assignment_operator != TOKEN_EQUALS
        ) {
            BinaryOperator binary_operator;

            switch (assignment_operator) {
                case TOKEN_PLUS_EQUALS:
                    binary_operator = BINARY_ADD;
                    break;

                case TOKEN_MINUS_EQUALS:
                    binary_operator = BINARY_SUBTRACT;
                    break;

                case TOKEN_STAR_EQUALS:
                    binary_operator = BINARY_MULTIPLY;
                    break;

                case TOKEN_SLASH_EQUALS:
                    binary_operator = BINARY_DIVIDE;
                    break;

                case TOKEN_PERCENT_EQUALS:
                    binary_operator = BINARY_MODULO;
                    break;

                default:
                    ast_free(target);
                    ast_free(value);
                    return NULL;
            }

            AstNode *left =
                clone_assignment_target(target);

            if (left == NULL) {
                ast_free(target);
                ast_free(value);

                printf(
                    "Parser error: out of memory.\n"
                );

                return NULL;
            }

            AstNode *compound_value =
                ast_create_binary(
                    left,
                    value,
                    binary_operator
                );

            if (compound_value == NULL) {
                ast_free(left);
                ast_free(target);
                ast_free(value);

                printf(
                    "Parser error: out of memory.\n"
                );

                return NULL;
            }

            int target_is_variable =
                target->type == AST_VARIABLE;

            AstNode *node =
                target_is_variable
                    ? ast_create_variable_declaration(
                        target->variable.name,
                        compound_value
                    )
                    : ast_create_assignment(
                        target,
                        compound_value
                    );

            if (node == NULL) {
                ast_free(target);
                ast_free(compound_value);

                printf(
                    "Parser error: out of memory.\n"
                );

                return NULL;
            }

            if (target_is_variable) {
                ast_free(target);
            }

            return node;
        }

        /*
         * Normal assignment.
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
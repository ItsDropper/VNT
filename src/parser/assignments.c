#include <vnt/parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        case AST_FLOAT_LITERAL:
            return ast_create_float(node->float_literal.value);

        case AST_MEMBER_EXPRESSION: {
            AstNode *object = clone_expression(node->member_expression.object);
            if (object == NULL) return NULL;
            AstNode *copy = ast_create_member(object, node->member_expression.member);
            if (copy == NULL) ast_free(object);
            return copy;
        }

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


AstNode *parse_identifier_statement(Parser *parser) {
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

    for (;;) {
        if (parser_check(parser, TOKEN_LEFT_BRACKET)) {
            parser_advance(parser);

            AstNode *index = parse_expression(parser);
            if (index == NULL) {
                ast_free(target);
                printf("Parser error: expected array index.\n");
                return NULL;
            }

            if (!parser_consume(parser, TOKEN_RIGHT_BRACKET, "expected ']' after array index.")) {
                ast_free(target);
                ast_free(index);
                return NULL;
            }

            AstNode *indexed = ast_create_index(target, index);
            if (indexed == NULL) {
                ast_free(target);
                ast_free(index);
                printf("Parser error: out of memory.\n");
                return NULL;
            }
            target = indexed;
            continue;
        }

        if (parser_check(parser, TOKEN_DOT)) {
            parser_advance(parser);
            if (!parser_check(parser, TOKEN_IDENTIFIER)) {
                ast_free(target);
                printf("Parser error: expected member name after '.'.\n");
                return NULL;
            }
            char *member = parser_token_to_string(parser->current);
            parser_advance(parser);
            AstNode *member_node = ast_create_member(target, member);
            free(member);
            if (member_node == NULL) {
                ast_free(target);
                printf("Parser error: out of memory.\n");
                return NULL;
            }
            target = member_node;
            continue;
        }

        break;
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


AstNode *parse_dereference_statement(Parser *parser) {
    parser_advance(parser);

    AstNode *operand = parse_expression(parser);
    if (!operand) {
        printf("Parser error: expected reference after '*'.\n");
        return NULL;
    }

    AstNode *target = ast_create_unary(operand, UNARY_DEREFERENCE);
    if (!target) {
        ast_free(operand);
        printf("Parser error: out of memory.\n");
        return NULL;
    }

    TokenType op = parser->current.type;
    if (op != TOKEN_EQUALS &&
        op != TOKEN_PLUS_EQUALS &&
        op != TOKEN_MINUS_EQUALS &&
        op != TOKEN_STAR_EQUALS &&
        op != TOKEN_SLASH_EQUALS &&
        op != TOKEN_PERCENT_EQUALS) {
        ast_free(target);
        printf("Parser error: expected assignment operator after dereference.\n");
        return NULL;
    }

    parser_advance(parser);

    AstNode *value = parse_expression(parser);
    if (!value) {
        ast_free(target);
        return NULL;
    }

    if (op != TOKEN_EQUALS) {
        BinaryOperator bop;
        switch (op) {
            case TOKEN_PLUS_EQUALS: bop = BINARY_ADD; break;
            case TOKEN_MINUS_EQUALS: bop = BINARY_SUBTRACT; break;
            case TOKEN_STAR_EQUALS: bop = BINARY_MULTIPLY; break;
            case TOKEN_SLASH_EQUALS: bop = BINARY_DIVIDE; break;
            case TOKEN_PERCENT_EQUALS: bop = BINARY_MODULO; break;
            default:
                ast_free(target);
                ast_free(value);
                return NULL;
        }

        AstNode *left = clone_assignment_target(target);
        if (!left) {
            ast_free(target);
            ast_free(value);
            return NULL;
        }

        AstNode *compound = ast_create_binary(left, value, bop);
        if (!compound) {
            ast_free(left);
            ast_free(target);
            ast_free(value);
            return NULL;
        }

        AstNode *node = ast_create_assignment(target, compound);
        if (!node) {
            ast_free(target);
            ast_free(compound);
            return NULL;
        }
        return node;
    }

    AstNode *node = ast_create_assignment(target, value);
    if (!node) {
        ast_free(target);
        ast_free(value);
    }
    return node;
}

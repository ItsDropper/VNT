#include <vnt/ir.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>

static int int_value(AstNode *n, int *out) {
    if (!n || n->type != AST_INTEGER_LITERAL) return 0;
    *out = n->integer_literal.value;
    return 1;
}

static int bool_value(AstNode *n, int *out) {
    if (!n || n->type != AST_BOOLEAN_LITERAL) return 0;
    *out = n->boolean_literal.value ? 1 : 0;
    return 1;
}

static int replace_int_binary(AstNode *n, int value) {
    ast_free(n->binary_expression.left);
    ast_free(n->binary_expression.right);
    n->binary_expression.left = NULL;
    n->binary_expression.right = NULL;
    n->type = AST_INTEGER_LITERAL;
    n->integer_literal.value = value;
    return 1;
}

static int replace_bool_binary(AstNode *n, int value) {
    ast_free(n->binary_expression.left);
    ast_free(n->binary_expression.right);
    n->binary_expression.left = NULL;
    n->binary_expression.right = NULL;
    n->type = AST_BOOLEAN_LITERAL;
    n->boolean_literal.value = value ? 1 : 0;
    return 1;
}

static int fold_binary(AstNode *n) {
    int a, b;
    AstNode *left = n->binary_expression.left;
    AstNode *right = n->binary_expression.right;
    if (int_value(left, &a) && int_value(right, &b)) {
        int64_t result;
        switch (n->binary_expression.operator) {
            case BINARY_ADD:
                result = (int64_t)a + b;
                if (result >= INT_MIN && result <= INT_MAX) return replace_int_binary(n, (int)result);
                break;
            case BINARY_SUBTRACT:
                result = (int64_t)a - b;
                if (result >= INT_MIN && result <= INT_MAX) return replace_int_binary(n, (int)result);
                break;
            case BINARY_MULTIPLY:
                result = (int64_t)a * b;
                if (result >= INT_MIN && result <= INT_MAX) return replace_int_binary(n, (int)result);
                break;
            case BINARY_DIVIDE:
                if (b && !(a == INT_MIN && b == -1)) return replace_int_binary(n, a / b);
                break;
            case BINARY_MODULO:
                if (b && !(a == INT_MIN && b == -1)) return replace_int_binary(n, a % b);
                break;
            case BINARY_EQUAL: return replace_bool_binary(n, a == b);
            case BINARY_NOT_EQUAL: return replace_bool_binary(n, a != b);
            case BINARY_GREATER: return replace_bool_binary(n, a > b);
            case BINARY_LESS: return replace_bool_binary(n, a < b);
            case BINARY_GREATER_EQUAL: return replace_bool_binary(n, a >= b);
            case BINARY_LESS_EQUAL: return replace_bool_binary(n, a <= b);
            default: break;
        }
    }
    if (bool_value(left, &a) && bool_value(right, &b)) {
        if (n->binary_expression.operator == BINARY_AND) return replace_bool_binary(n, a && b);
        if (n->binary_expression.operator == BINARY_OR) return replace_bool_binary(n, a || b);
        if (n->binary_expression.operator == BINARY_EQUAL) return replace_bool_binary(n, a == b);
        if (n->binary_expression.operator == BINARY_NOT_EQUAL) return replace_bool_binary(n, a != b);
    }
    return 0;
}

static void fold(AstNode *n, int *changed) {
    for (; n; n = n->next) {
        switch (n->type) {
            case AST_BINARY_EXPRESSION:
                fold(n->binary_expression.left, changed);
                fold(n->binary_expression.right, changed);
                if (fold_binary(n)) *changed = 1;
                break;
            case AST_UNARY_EXPRESSION: fold(n->unary_expression.operand, changed); break;
            case AST_ARRAY_LITERAL: fold(n->array_literal.elements, changed); break;
            case AST_INDEX_EXPRESSION:
                fold(n->index_expression.array, changed);
                fold(n->index_expression.index, changed);
                break;
            case AST_MEMBER_EXPRESSION: fold(n->member_expression.object, changed); break;
            case AST_ASSIGNMENT:
                fold(n->assignment.target, changed);
                fold(n->assignment.value, changed);
                break;
            case AST_VARIABLE_DECLARATION: fold(n->variable_declaration.value, changed); break;
            case AST_FUNCTION_CALL: fold(n->function_call.arguments, changed); break;
            case AST_IF_STATEMENT:
                fold(n->if_statement.condition, changed);
                fold(n->if_statement.then_branch, changed);
                fold(n->if_statement.else_branch, changed);
                break;
            case AST_WHILE_STATEMENT:
                fold(n->while_statement.condition, changed);
                fold(n->while_statement.body, changed);
                break;
            case AST_FUNCTION_DECLARATION: fold(n->function_declaration.body, changed); break;
            case AST_RETURN_STATEMENT: fold(n->return_statement.expression, changed); break;
            case AST_PRINT_STATEMENT: fold(n->print_statement.expression, changed); break;
            default: break;
        }
    }
}

int vnt_ir_lower(VntIrProgram *ir, AstNode *program) {
    if (!ir || !program || program->type != AST_PROGRAM) return 0;
    ir->program = program;
    ir->optimized_nodes = 0;
    return 1;
}

int vnt_ir_validate(const VntIrProgram *ir) {
    return ir && ir->program && ir->program->type == AST_PROGRAM;
}

void vnt_ir_free(VntIrProgram *ir) {
    if (!ir) return;
    ir->program = NULL;
    ir->optimized_nodes = 0;
}

int vnt_ir_optimize(VntIrProgram *ir) {
    if (!ir || !ir->program) return 0;

    int changed = 0;
    for (int pass = 0; pass < 8; ++pass) {
        int pass_changed = 0;
        fold(ir->program->program.statements, &pass_changed);
        if (!pass_changed) break;
        changed += pass_changed;
    }

    ir->optimized_nodes = changed;
    return 1;
}

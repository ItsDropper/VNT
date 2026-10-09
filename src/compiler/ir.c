#include <vnt/ir.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

static VntIrOpcode opcode_for(AstNodeType type) {
    switch (type) {
        case AST_PROGRAM: return VNT_IR_PROGRAM;
        case AST_PRINT_STATEMENT: return VNT_IR_PRINT;
        case AST_IF_STATEMENT: return VNT_IR_IF;
        case AST_WHILE_STATEMENT: return VNT_IR_WHILE;
        case AST_FUNCTION_DECLARATION: return VNT_IR_FUNCTION;
        case AST_STRUCT_DECLARATION: return VNT_IR_STRUCT;
        case AST_FUNCTION_CALL: return VNT_IR_CALL;
        case AST_RETURN_STATEMENT: return VNT_IR_RETURN;
        case AST_BREAK_STATEMENT: return VNT_IR_BREAK;
        case AST_CONTINUE_STATEMENT: return VNT_IR_CONTINUE;
        case AST_STRING_LITERAL: return VNT_IR_STRING;
        case AST_INTEGER_LITERAL: return VNT_IR_INTEGER;
        case AST_FLOAT_LITERAL: return VNT_IR_FLOAT;
        case AST_BOOLEAN_LITERAL: return VNT_IR_BOOLEAN;
        case AST_ARRAY_LITERAL: return VNT_IR_ARRAY;
        case AST_VARIABLE_DECLARATION: return VNT_IR_VARIABLE_DECL;
        case AST_VARIABLE: return VNT_IR_VARIABLE;
        case AST_INDEX_EXPRESSION: return VNT_IR_INDEX;
        case AST_MEMBER_EXPRESSION: return VNT_IR_MEMBER;
        case AST_ASSIGNMENT: return VNT_IR_ASSIGN;
        case AST_BINARY_EXPRESSION: return VNT_IR_BINARY;
        case AST_UNARY_EXPRESSION: return VNT_IR_UNARY;
        default: return (VntIrOpcode)-1;
    }
}

static void clear_nodes(VntIrProgram *ir) {
    for (size_t i = 0; i < ir->node_count; ++i) {
        VntIrOpcode op = ir->nodes[i].opcode;
        if (op == VNT_IR_STRING || op == VNT_IR_VARIABLE_DECL ||
            op == VNT_IR_VARIABLE || op == VNT_IR_MEMBER ||
            op == VNT_IR_CALL || op == VNT_IR_FUNCTION || op == VNT_IR_STRUCT)
            free(ir->nodes[i].value.text);
    }
    free(ir->nodes);
    ir->nodes = NULL;
    ir->node_count = 0;
    ir->node_capacity = 0;
    ir->root = VNT_IR_NO_NODE;
}

static int reserve_node(VntIrProgram *ir, size_t *index) {
    if (ir->node_count == ir->node_capacity) {
        size_t cap = ir->node_capacity ? ir->node_capacity * 2 : 64;
        if (cap < ir->node_capacity || cap > SIZE_MAX / sizeof(*ir->nodes)) return 0;
        VntIrNode *nodes = realloc(ir->nodes, cap * sizeof(*nodes));
        if (!nodes) return 0;
        ir->nodes = nodes;
        ir->node_capacity = cap;
    }
    *index = ir->node_count++;
    VntIrNode *node = &ir->nodes[*index];
    memset(node, 0, sizeof(*node));
    node->first_child = node->last_child = node->next_sibling = VNT_IR_NO_NODE;
    return 1;
}

static int add_child(VntIrProgram *ir, size_t parent, size_t child) {
    if (parent >= ir->node_count || child >= ir->node_count) return 0;
    if (ir->nodes[parent].last_child == VNT_IR_NO_NODE)
        ir->nodes[parent].first_child = child;
    else
        ir->nodes[ir->nodes[parent].last_child].next_sibling = child;
    ir->nodes[parent].last_child = child;
    ir->nodes[parent].child_count++;
    return 1;
}

static int lower_node(VntIrProgram *ir, const AstNode *ast, size_t *result);

static int lower_list(VntIrProgram *ir, size_t parent, const AstNode *ast) {
    for (const AstNode *item = ast; item; item = item->next) {
        size_t child;
        if (!lower_node(ir, item, &child) || !add_child(ir, parent, child)) return 0;
    }
    return 1;
}

static int lower_one(VntIrProgram *ir, size_t parent, const AstNode *ast) {
    if (!ast) return 1;
    size_t child;
    return lower_node(ir, ast, &child) && add_child(ir, parent, child);
}

static int copy_text(char **destination, const char *source) {
    if (!source) source = "";
    size_t len = strlen(source);
    char *copy = malloc(len + 1);
    if (!copy) return 0;
    memcpy(copy, source, len + 1);
    *destination = copy;
    return 1;
}

static int lower_node(VntIrProgram *ir, const AstNode *ast, size_t *result) {
    if (!ast || !result) return 0;
    VntIrOpcode op = opcode_for(ast->type);
    if ((int)op < 0 || !reserve_node(ir, result)) return 0;
    VntIrNode *node = &ir->nodes[*result];
    node->opcode = op;
    node->source = ast;

    switch (ast->type) {
        case AST_INTEGER_LITERAL: node->value.integer = ast->integer_literal.value; break;
        case AST_BOOLEAN_LITERAL: node->value.boolean = !!ast->boolean_literal.value; break;
        case AST_FLOAT_LITERAL: node->value.floating = ast->float_literal.value; break;
        case AST_STRING_LITERAL:
            if (!copy_text(&node->value.text, ast->string_literal.value)) return 0;
            break;
        case AST_VARIABLE_DECLARATION:
            if (!copy_text(&node->value.text, ast->variable_declaration.name)) return 0;
            break;
        case AST_VARIABLE:
            if (!copy_text(&node->value.text, ast->variable.name)) return 0;
            break;
        case AST_MEMBER_EXPRESSION:
            if (!copy_text(&node->value.text, ast->member_expression.member)) return 0;
            break;
        case AST_FUNCTION_CALL:
            if (!copy_text(&node->value.text, ast->function_call.name)) return 0;
            break;
        case AST_FUNCTION_DECLARATION:
            if (!copy_text(&node->value.text, ast->function_declaration.name)) return 0;
            break;
        case AST_STRUCT_DECLARATION:
            if (!copy_text(&node->value.text, ast->struct_declaration.name)) return 0;
            break;
        default: break;
    }

    switch (ast->type) {
        case AST_PROGRAM: return lower_list(ir, *result, ast->program.statements);
        case AST_PRINT_STATEMENT: return lower_one(ir, *result, ast->print_statement.expression);
        case AST_IF_STATEMENT:
            return lower_one(ir, *result, ast->if_statement.condition) &&
                   lower_list(ir, *result, ast->if_statement.then_branch) &&
                   lower_list(ir, *result, ast->if_statement.else_branch);
        case AST_WHILE_STATEMENT:
            return lower_one(ir, *result, ast->while_statement.condition) &&
                   lower_list(ir, *result, ast->while_statement.body);
        case AST_FUNCTION_DECLARATION: return lower_list(ir, *result, ast->function_declaration.body);
        case AST_FUNCTION_CALL: return lower_list(ir, *result, ast->function_call.arguments);
        case AST_RETURN_STATEMENT: return lower_one(ir, *result, ast->return_statement.expression);
        case AST_ARRAY_LITERAL: return lower_list(ir, *result, ast->array_literal.elements);
        case AST_INDEX_EXPRESSION:
            return lower_one(ir, *result, ast->index_expression.array) &&
                   lower_one(ir, *result, ast->index_expression.index);
        case AST_MEMBER_EXPRESSION: return lower_one(ir, *result, ast->member_expression.object);
        case AST_ASSIGNMENT:
            return lower_one(ir, *result, ast->assignment.target) &&
                   lower_one(ir, *result, ast->assignment.value);
        case AST_BINARY_EXPRESSION:
            return lower_one(ir, *result, ast->binary_expression.left) &&
                   lower_one(ir, *result, ast->binary_expression.right);
        case AST_UNARY_EXPRESSION: return lower_one(ir, *result, ast->unary_expression.operand);
        default: return 1;
    }
}

static int build_hir(VntIrProgram *ir) {
    clear_nodes(ir);
    if (!ir->program || ir->program->type != AST_PROGRAM) return 0;
    if (!lower_node(ir, ir->program, &ir->root)) {
        clear_nodes(ir);
        return 0;
    }
    return vnt_ir_validate(ir);
}

int vnt_ir_lower(VntIrProgram *ir, AstNode *program) {
    if (!ir || !program || program->type != AST_PROGRAM) return 0;
    memset(ir, 0, sizeof(*ir));
    ir->root = VNT_IR_NO_NODE;
    ir->program = program;
    if (!build_hir(ir)) {
        vnt_ir_free(ir);
        return 0;
    }
    return 1;
}

int vnt_ir_validate(const VntIrProgram *ir) {
    if (!ir || !ir->program || ir->program->type != AST_PROGRAM ||
        !ir->nodes || !ir->node_count || ir->root >= ir->node_count ||
        ir->nodes[ir->root].opcode != VNT_IR_PROGRAM) return 0;

    for (size_t i = 0; i < ir->node_count; ++i) {
        const VntIrNode *node = &ir->nodes[i];
        if (!node->source || (int)node->opcode < 0 ||
            node->opcode > VNT_IR_UNARY ||
            opcode_for(node->source->type) != node->opcode) return 0;
        if ((node->first_child == VNT_IR_NO_NODE) != (node->child_count == 0)) return 0;
        size_t child = node->first_child;
        size_t seen = 0, last = VNT_IR_NO_NODE;
        while (child != VNT_IR_NO_NODE) {
            if (child >= ir->node_count || child == i || ++seen > ir->node_count) return 0;
            last = child;
            child = ir->nodes[child].next_sibling;
        }
        if (seen != node->child_count || last != node->last_child) return 0;
        if (node->last_child != VNT_IR_NO_NODE &&
            ir->nodes[node->last_child].next_sibling != VNT_IR_NO_NODE) return 0;
    }
    return 1;
}

void vnt_ir_free(VntIrProgram *ir) {
    if (!ir) return;
    clear_nodes(ir);
    ir->program = NULL;
    ir->optimized_nodes = 0;
}

int vnt_ir_optimize(VntIrProgram *ir) {
    if (!ir || !ir->program || !vnt_ir_validate(ir)) return 0;

    size_t changed = 0;
    for (int pass = 0; pass < 8; ++pass) {
        int pass_changed = 0;
        fold(ir->program->program.statements, &pass_changed);
        if (!pass_changed) break;
        changed += (size_t)pass_changed;
    }

    ir->optimized_nodes = changed;
    /* Folding changes AST node kinds; rebuild HIR so it cannot go stale. */
    return build_hir(ir);
}

#include <vnt/ir.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
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

static int float_value(AstNode *n, double *out) {
    if (!n || n->type != AST_FLOAT_LITERAL) return 0;
    *out = n->float_literal.value;
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

static int replace_float_binary(AstNode *n, double value) {
    ast_free(n->binary_expression.left);
    ast_free(n->binary_expression.right);
    n->binary_expression.left = NULL;
    n->binary_expression.right = NULL;
    n->type = AST_FLOAT_LITERAL;
    n->float_literal.value = value;
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

    double x, y;
    if (float_value(left, &x) && float_value(right, &y)) {
        switch (n->binary_expression.operator) {
            case BINARY_ADD:
                if (isfinite(x + y)) return replace_float_binary(n, x + y);
                break;
            case BINARY_SUBTRACT:
                if (isfinite(x - y)) return replace_float_binary(n, x - y);
                break;
            case BINARY_MULTIPLY:
                if (isfinite(x * y)) return replace_float_binary(n, x * y);
                break;
            case BINARY_DIVIDE:
                if (y != 0.0 && isfinite(x / y))
                    return replace_float_binary(n, x / y);
                break;
            case BINARY_EQUAL: return replace_bool_binary(n, x == y);
            case BINARY_NOT_EQUAL: return replace_bool_binary(n, x != y);
            case BINARY_GREATER: return replace_bool_binary(n, x > y);
            case BINARY_LESS: return replace_bool_binary(n, x < y);
            case BINARY_GREATER_EQUAL: return replace_bool_binary(n, x >= y);
            case BINARY_LESS_EQUAL: return replace_bool_binary(n, x <= y);
            default: break;
        }
    }
    return 0;
}

static void fold(AstNode *n, size_t *changed) {
    for (; n; n = n->next) {
        switch (n->type) {
            case AST_BINARY_EXPRESSION:
                fold(n->binary_expression.left, changed);
                fold(n->binary_expression.right, changed);
                if (fold_binary(n)) ++*changed;
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
            op == VNT_IR_REASSIGN || op == VNT_IR_VARIABLE ||
            op == VNT_IR_MEMBER || op == VNT_IR_CALL ||
            op == VNT_IR_FUNCTION || op == VNT_IR_STRUCT)
            free(ir->nodes[i].value.text);
        for (size_t j = 0; j < ir->nodes[i].name_count; ++j)
            free(ir->nodes[i].names[j]);
        free(ir->nodes[i].names);
        free(ir->nodes[i].type_name);
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

static int lower_list(VntIrProgram *ir, size_t parent, const AstNode *ast, VntIrEdgeRole role) {
    for (const AstNode *item = ast; item; item = item->next) {
        size_t child;
        if (!lower_node(ir, item, &child)) return 0;
        ir->nodes[child].role = role;
        if (!add_child(ir, parent, child)) return 0;
    }
    return 1;
}

static int lower_one(VntIrProgram *ir, size_t parent, const AstNode *ast, VntIrEdgeRole role) {
    if (!ast) return 1;
    size_t child;
    if (!lower_node(ir, ast, &child)) return 0;
    ir->nodes[child].role = role;
    return add_child(ir, parent, child);
}

static int copy_text(char **destination, const char *source);

static int copy_names(VntIrNode *node, char *const *names, int count) {
    if (count <= 0) return 1;
    node->names = calloc((size_t)count, sizeof(*node->names));
    if (!node->names) return 0;
    node->name_count = (size_t)count;
    for (int i = 0; i < count; ++i)
        if (!copy_text(&node->names[i], names[i])) return 0;
    return 1;
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
    if (ast->type == AST_VARIABLE_DECLARATION &&
        ast->variable_declaration.is_reassignment)
        op = VNT_IR_REASSIGN;
    if ((int)op < 0 || !reserve_node(ir, result)) return 0;
    VntIrNode *node = &ir->nodes[*result];
    node->opcode = op;
    node->role = VNT_IR_EDGE_ROOT;
    node->source = ast;
    if (ast->type == AST_VARIABLE_DECLARATION &&
        ast->variable_declaration.declared_type &&
        !copy_text(&node->type_name,
                   ast->variable_declaration.declared_type)) return 0;
    if (ast->type == AST_BINARY_EXPRESSION)
        node->operation = (int)ast->binary_expression.operator;
    else if (ast->type == AST_UNARY_EXPRESSION)
        node->operation = (int)ast->unary_expression.operator;

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
            if (!copy_text(&node->value.text, ast->function_declaration.name) ||
                !copy_names(node, ast->function_declaration.parameters,
                            ast->function_declaration.parameter_count)) return 0;
            break;
        case AST_STRUCT_DECLARATION:
            if (!copy_text(&node->value.text, ast->struct_declaration.name) ||
                !copy_names(node, ast->struct_declaration.fields,
                            ast->struct_declaration.field_count)) return 0;
            break;
        default: break;
    }

    switch (ast->type) {
        case AST_PROGRAM: return lower_list(ir, *result, ast->program.statements, VNT_IR_EDGE_STATEMENT);
        case AST_PRINT_STATEMENT: return lower_one(ir, *result, ast->print_statement.expression, VNT_IR_EDGE_VALUE);
        case AST_IF_STATEMENT:
            return lower_one(ir, *result, ast->if_statement.condition, VNT_IR_EDGE_CONDITION) &&
                   lower_list(ir, *result, ast->if_statement.then_branch, VNT_IR_EDGE_THEN) &&
                   lower_list(ir, *result, ast->if_statement.else_branch, VNT_IR_EDGE_ELSE);
        case AST_WHILE_STATEMENT:
            return lower_one(ir, *result, ast->while_statement.condition, VNT_IR_EDGE_CONDITION) &&
                   lower_list(ir, *result, ast->while_statement.body, VNT_IR_EDGE_BODY);
        case AST_FUNCTION_DECLARATION: return lower_list(ir, *result, ast->function_declaration.body, VNT_IR_EDGE_BODY);
        case AST_VARIABLE_DECLARATION: return lower_one(ir, *result, ast->variable_declaration.value, VNT_IR_EDGE_VALUE);
        case AST_FUNCTION_CALL: return lower_list(ir, *result, ast->function_call.arguments, VNT_IR_EDGE_ARGUMENT);
        case AST_RETURN_STATEMENT: return lower_one(ir, *result, ast->return_statement.expression, VNT_IR_EDGE_VALUE);
        case AST_ARRAY_LITERAL: return lower_list(ir, *result, ast->array_literal.elements, VNT_IR_EDGE_ELEMENT);
        case AST_INDEX_EXPRESSION:
            return lower_one(ir, *result, ast->index_expression.array, VNT_IR_EDGE_OBJECT) &&
                   lower_one(ir, *result, ast->index_expression.index, VNT_IR_EDGE_INDEX);
        case AST_MEMBER_EXPRESSION: return lower_one(ir, *result, ast->member_expression.object, VNT_IR_EDGE_OBJECT);
        case AST_ASSIGNMENT:
            return lower_one(ir, *result, ast->assignment.target, VNT_IR_EDGE_TARGET) &&
                   lower_one(ir, *result, ast->assignment.value, VNT_IR_EDGE_VALUE);
        case AST_BINARY_EXPRESSION:
            return lower_one(ir, *result, ast->binary_expression.left, VNT_IR_EDGE_LEFT) &&
                   lower_one(ir, *result, ast->binary_expression.right, VNT_IR_EDGE_RIGHT);
        case AST_UNARY_EXPRESSION: return lower_one(ir, *result, ast->unary_expression.operand, VNT_IR_EDGE_OPERAND);
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

static size_t child_for_role(const VntIrProgram *ir, const VntIrNode *node,
                            VntIrEdgeRole role) {
    for (size_t child = node->first_child; child != VNT_IR_NO_NODE;
         child = ir->nodes[child].next_sibling)
        if (ir->nodes[child].role == role) return child;
    return VNT_IR_NO_NODE;
}

static int role_count(const VntIrProgram *ir, const VntIrNode *node,
                      VntIrEdgeRole role) {
    int count = 0;
    for (size_t child = node->first_child; child != VNT_IR_NO_NODE;
         child = ir->nodes[child].next_sibling)
        if (ir->nodes[child].role == role) ++count;
    return count;
}

static int validate_node_shape(const VntIrProgram *ir, const VntIrNode *node) {
    int statements = role_count(ir, node, VNT_IR_EDGE_STATEMENT);
    int conditions = role_count(ir, node, VNT_IR_EDGE_CONDITION);
    int then_edges = role_count(ir, node, VNT_IR_EDGE_THEN);
    int else_edges = role_count(ir, node, VNT_IR_EDGE_ELSE);
    int bodies = role_count(ir, node, VNT_IR_EDGE_BODY);
    int values = role_count(ir, node, VNT_IR_EDGE_VALUE);
    int targets = role_count(ir, node, VNT_IR_EDGE_TARGET);
    int args = role_count(ir, node, VNT_IR_EDGE_ARGUMENT);
    int elements = role_count(ir, node, VNT_IR_EDGE_ELEMENT);
    int objects = role_count(ir, node, VNT_IR_EDGE_OBJECT);
    int indices = role_count(ir, node, VNT_IR_EDGE_INDEX);
    int lefts = role_count(ir, node, VNT_IR_EDGE_LEFT);
    int rights = role_count(ir, node, VNT_IR_EDGE_RIGHT);
    int operands = role_count(ir, node, VNT_IR_EDGE_OPERAND);

    switch (node->opcode) {
        case VNT_IR_PROGRAM:
            return statements == (int)node->child_count &&
                   !conditions && !then_edges && !else_edges && !bodies &&
                   !values && !targets && !args && !elements && !objects &&
                   !indices && !lefts && !rights && !operands;
        case VNT_IR_PRINT:
        case VNT_IR_RETURN:
            return values == (int)node->child_count && values <= 1;
        case VNT_IR_IF:
            return conditions == 1 && then_edges + else_edges + conditions ==
                   (int)node->child_count && !statements && !bodies && !values &&
                   !targets && !args && !elements && !objects && !indices &&
                   !lefts && !rights && !operands;
        case VNT_IR_WHILE:
            return conditions == 1 && bodies + conditions == (int)node->child_count &&
                   !statements && !then_edges && !else_edges && !values &&
                   !targets && !args && !elements && !objects && !indices &&
                   !lefts && !rights && !operands;
        case VNT_IR_FUNCTION:
            return bodies == (int)node->child_count && !conditions && !then_edges &&
                   !else_edges && !values && !targets && !args && !elements &&
                   !objects && !indices && !lefts && !rights && !operands;
        case VNT_IR_STRUCT:
        case VNT_IR_BREAK:
        case VNT_IR_CONTINUE:
        case VNT_IR_STRING:
        case VNT_IR_INTEGER:
        case VNT_IR_FLOAT:
        case VNT_IR_BOOLEAN:
        case VNT_IR_VARIABLE:
            return node->child_count == 0;
        case VNT_IR_CALL:
            return args == (int)node->child_count;
        case VNT_IR_ARRAY:
            return elements == (int)node->child_count;
        case VNT_IR_VARIABLE_DECL:
            return values == (int)node->child_count && values <= 1;
        case VNT_IR_REASSIGN:
            return values == 1 && node->child_count == 1;
        case VNT_IR_INDEX:
            return objects == 1 && indices == 1 && node->child_count == 2;
        case VNT_IR_MEMBER:
            return objects == 1 && node->child_count == 1;
        case VNT_IR_ASSIGN:
            return targets == 1 && values == 1 && node->child_count == 2;
        case VNT_IR_BINARY:
            return lefts == 1 && rights == 1 && node->child_count == 2;
        case VNT_IR_UNARY:
            return operands == 1 && node->child_count == 1;
        default:
            return 0;
    }
}

int vnt_ir_validate(const VntIrProgram *ir) {
    if (!ir || !ir->nodes || !ir->node_count || ir->root >= ir->node_count ||
        ir->nodes[ir->root].opcode != VNT_IR_PROGRAM) return 0;

    if (ir->nodes[ir->root].role != VNT_IR_EDGE_ROOT ||
        ir->nodes[ir->root].next_sibling != VNT_IR_NO_NODE) return 0;
    size_t *parents = calloc(ir->node_count, sizeof(*parents));
    if (!parents) return 0;

    for (size_t i = 0; i < ir->node_count; ++i) {
        const VntIrNode *node = &ir->nodes[i];
        if ((int)node->opcode < 0 || node->opcode > VNT_IR_REASSIGN ||
            (int)node->role < 0 || node->role > VNT_IR_EDGE_OPERAND ||
            (node->name_count && !node->names) ||
            (node->opcode == VNT_IR_BINARY &&
             (node->operation < BINARY_ADD || node->operation > BINARY_OR)) ||
            (node->opcode == VNT_IR_UNARY &&
             (node->operation < UNARY_NOT || node->operation > UNARY_DEREFERENCE)) ||
            ((node->first_child == VNT_IR_NO_NODE) != (node->child_count == 0))) {
            free(parents);
            return 0;
        }
        for (size_t j = 0; j < node->name_count; ++j) {
            if (!node->names[j]) {
                free(parents);
                return 0;
            }
        }
        size_t child = node->first_child;
        size_t seen = 0, last = VNT_IR_NO_NODE;
        while (child != VNT_IR_NO_NODE) {
            if (child >= ir->node_count || child == i || ++seen > ir->node_count) {
                free(parents);
                return 0;
            }
            if (++parents[child] > 1) {
                free(parents);
                return 0;
            }
            last = child;
            child = ir->nodes[child].next_sibling;
        }
        if (seen != node->child_count || last != node->last_child ||
            (node->last_child != VNT_IR_NO_NODE &&
             ir->nodes[node->last_child].next_sibling != VNT_IR_NO_NODE) ||
            !validate_node_shape(ir, node)) {
            free(parents);
            return 0;
        }

        /* Validation uses HIR-owned metadata only; source nodes are optional. */

    int valid = parents[ir->root] == 0;
    for (size_t i = 0; valid && i < ir->node_count; ++i)
        if (i != ir->root && parents[i] != 1) valid = 0;
    free(parents);
    return valid;
}


static size_t hir_child_role(const VntIrProgram *ir, size_t parent, VntIrEdgeRole role) {
    for (size_t c = ir->nodes[parent].first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        if (ir->nodes[c].role == role) return c;
    return VNT_IR_NO_NODE;
}

static AstNode *hir_materialize_node(const VntIrProgram *ir, size_t index);

static AstNode *hir_materialize_list(const VntIrProgram *ir, size_t parent,
                                     VntIrEdgeRole role, int *count) {
    AstNode *list = NULL;
    *count = 0;
    for (size_t c = ir->nodes[parent].first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling) {
        if (ir->nodes[c].role != role) continue;
        AstNode *item = hir_materialize_node(ir, c);
        if (!item) { ast_free(list); return NULL; }
        ast_append(&list, item);
        ++*count;
    }
    return list;
}

static AstNode *hir_materialize_child(const VntIrProgram *ir, size_t parent,
                                      VntIrEdgeRole role) {
    size_t child = hir_child_role(ir, parent, role);
    return child == VNT_IR_NO_NODE ? NULL : hir_materialize_node(ir, child);
}

static char **hir_copy_names(const VntIrNode *node) {
    if (!node->name_count) return NULL;
    char **names = calloc(node->name_count, sizeof(*names));
    if (!names) return NULL;
    for (size_t i = 0; i < node->name_count; ++i) {
        names[i] = strdup(node->names[i]);
        if (!names[i]) {
            for (size_t j = 0; j < i; ++j) free(names[j]);
            free(names);
            return NULL;
        }
    }
    return names;
}

static AstNode *hir_materialize_node(const VntIrProgram *ir, size_t index) {
    if (!ir || index >= ir->node_count) return NULL;
    const VntIrNode *n = &ir->nodes[index];
    AstNode *a = NULL;
    int count = 0, count2 = 0;
    AstNode *x = NULL, *y = NULL, *z = NULL;
    switch (n->opcode) {
        case VNT_IR_PROGRAM:
            x = hir_materialize_list(ir, index, VNT_IR_EDGE_STATEMENT, &count);
            if (n->child_count && !x) return NULL;
            a = ast_create_program(x); break;
        case VNT_IR_PRINT:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_VALUE);
            if (hir_child_role(ir,index,VNT_IR_EDGE_VALUE)!=VNT_IR_NO_NODE && !x) return NULL;
            a = ast_create_print(x); break;
        case VNT_IR_IF:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_CONDITION);
            y = hir_materialize_list(ir,index,VNT_IR_EDGE_THEN,&count);
            z = hir_materialize_list(ir,index,VNT_IR_EDGE_ELSE,&count2);
            if (!x || (role_count(ir,&ir->nodes[index],VNT_IR_EDGE_THEN) && !y) ||
                (role_count(ir,&ir->nodes[index],VNT_IR_EDGE_ELSE) && !z)) {
                ast_free(x); ast_free(y); ast_free(z); return NULL;
            }
            a = ast_create_if(x,y,z); break;
        case VNT_IR_WHILE:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_CONDITION);
            y = hir_materialize_list(ir,index,VNT_IR_EDGE_BODY,&count);
            if (!x || (role_count(ir,&ir->nodes[index],VNT_IR_EDGE_BODY) && !y)) {
                ast_free(x); ast_free(y); return NULL;
            }
            a = ast_create_while(x,y); break;
        case VNT_IR_FUNCTION: {
            x = hir_materialize_list(ir,index,VNT_IR_EDGE_BODY,&count);
            char **names = hir_copy_names(n);
            if (n->name_count && !names) { ast_free(x); return NULL; }
            a = ast_create_function_declaration(n->value.text,names,(int)n->name_count,x);
            if (!a) { for(size_t i=0;i<n->name_count;i++) free(names[i]); free(names); ast_free(x); }
            break;
        }
        case VNT_IR_STRUCT: {
            char **names = hir_copy_names(n);
            if (n->name_count && !names) return NULL;
            a = ast_create_struct_declaration(n->value.text,names,(int)n->name_count);
            if (!a) { for(size_t i=0;i<n->name_count;i++) free(names[i]); free(names); }
            break;
        }
        case VNT_IR_CALL:
            x = hir_materialize_list(ir,index,VNT_IR_EDGE_ARGUMENT,&count);
            if (n->child_count && !x) return NULL;
            a = ast_create_function_call(n->value.text,x,count); break;
        case VNT_IR_RETURN:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_VALUE);
            if (hir_child_role(ir,index,VNT_IR_EDGE_VALUE)!=VNT_IR_NO_NODE && !x) return NULL;
            a = ast_create_return(x); break;
        case VNT_IR_BREAK: a = ast_create_break(); break;
        case VNT_IR_CONTINUE: a = ast_create_continue(); break;
        case VNT_IR_STRING: a = ast_create_string(n->value.text); break;
        case VNT_IR_INTEGER: a = ast_create_integer(n->value.integer); break;
        case VNT_IR_FLOAT: a = ast_create_float(n->value.floating); break;
        case VNT_IR_BOOLEAN: a = ast_create_boolean(n->value.boolean); break;
        case VNT_IR_ARRAY:
            x = hir_materialize_list(ir,index,VNT_IR_EDGE_ELEMENT,&count);
            if (n->child_count && !x) return NULL;
            a = ast_create_array(x,count); break;
        case VNT_IR_VARIABLE_DECL:
        case VNT_IR_REASSIGN:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_VALUE);
            if (hir_child_role(ir,index,VNT_IR_EDGE_VALUE)!=VNT_IR_NO_NODE && !x) return NULL;
            a = n->type_name
                ? ast_create_typed_variable_declaration(n->value.text,n->type_name,x)
                : ast_create_variable_declaration(n->value.text,x);
            if (a && n->opcode == VNT_IR_REASSIGN)
                a->variable_declaration.is_reassignment = 1;
            break;
        case VNT_IR_VARIABLE: a = ast_create_variable(n->value.text); break;
        case VNT_IR_INDEX:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_OBJECT);
            y = hir_materialize_child(ir,index,VNT_IR_EDGE_INDEX);
            if (!x || !y) { ast_free(x); ast_free(y); return NULL; }
            a = ast_create_index(x,y); break;
        case VNT_IR_MEMBER:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_OBJECT);
            if (!x) return NULL;
            a = ast_create_member(x,n->value.text); break;
        case VNT_IR_ASSIGN:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_TARGET);
            y = hir_materialize_child(ir,index,VNT_IR_EDGE_VALUE);
            if (!x || !y) { ast_free(x); ast_free(y); return NULL; }
            a = ast_create_assignment(x,y); break;
        case VNT_IR_BINARY:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_LEFT);
            y = hir_materialize_child(ir,index,VNT_IR_EDGE_RIGHT);
            if (!x || !y) { ast_free(x); ast_free(y); return NULL; }
            a = ast_create_binary(x,y,(BinaryOperator)n->operation); break;
        case VNT_IR_UNARY:
            x = hir_materialize_child(ir,index,VNT_IR_EDGE_OPERAND);
            if (!x) return NULL;
            a = ast_create_unary(x,(UnaryOperator)n->operation); break;
        default: return NULL;
    }
    if (a && n->type_name && n->opcode != VNT_IR_VARIABLE_DECL &&
        n->opcode != VNT_IR_REASSIGN) {
        /* type_name is currently meaningful only for declarations. */
    }
    return a;
}

AstNode *vnt_ir_materialize_program(const VntIrProgram *ir) {
    if (!ir || !ir->nodes || !ir->node_count || ir->root >= ir->node_count ||
        ir->nodes[ir->root].opcode != VNT_IR_PROGRAM) return NULL;
    return hir_materialize_node(ir, ir->root);
}

static const char *opcode_name(VntIrOpcode opcode) {
    static const char *names[] = {
        "program", "print", "if", "while", "function", "struct",
        "call", "return", "break", "continue", "string", "integer",
        "float", "boolean", "array", "variable-decl", "variable",
        "index", "member", "assign", "binary", "unary", "reassign"
    };
    return opcode >= VNT_IR_PROGRAM && opcode <= VNT_IR_REASSIGN
        ? names[opcode] : "invalid";
}

static const char *edge_role_name(VntIrEdgeRole role) {
    static const char *names[] = {
        "root", "statement", "condition", "then", "else", "body",
        "value", "target", "argument", "element", "object", "index",
        "left", "right", "operand"
    };
    return role >= VNT_IR_EDGE_ROOT && role <= VNT_IR_EDGE_OPERAND
        ? names[role] : "invalid";
}

static const char *operation_name(const VntIrNode *node) {
    static const char *binary[] = {
        "add", "subtract", "multiply", "divide", "modulo",
        "equal", "not-equal", "greater", "less", "greater-equal",
        "less-equal", "and", "or"
    };
    static const char *unary[] = {"not", "negate", "reference", "dereference"};
    if (node->opcode == VNT_IR_BINARY &&
        node->operation >= BINARY_ADD && node->operation <= BINARY_OR)
        return binary[node->operation];
    if (node->opcode == VNT_IR_UNARY &&
        node->operation >= UNARY_NOT && node->operation <= UNARY_DEREFERENCE)
        return unary[node->operation];
    return "invalid";
}

void vnt_ir_dump(const VntIrProgram *ir, FILE *out) {
    if (!out || !vnt_ir_validate(ir)) {
        if (out) fputs("VNT HIR error: invalid IR graph.\n", out);
        return;
    }
    fprintf(out, "VNT HIR: %zu nodes; root=%zu; folded=%zu\n",
            ir->node_count, ir->root, ir->optimized_nodes);
    for (size_t i = 0; i < ir->node_count; ++i) {
        const VntIrNode *node = &ir->nodes[i];
        fprintf(out, "%04zu %-13s role=%s children=[", i, opcode_name(node->opcode), edge_role_name(node->role));
        size_t child = node->first_child;
        while (child != VNT_IR_NO_NODE) {
            fprintf(out, "%s%zu", child == node->first_child ? "" : ",", child);
            child = ir->nodes[child].next_sibling;
        }
        fputc(']', out);
        switch (node->opcode) {
            case VNT_IR_INTEGER: fprintf(out, " value=%d", node->value.integer); break;
            case VNT_IR_BOOLEAN: fprintf(out, " value=%s", node->value.boolean ? "true" : "false"); break;
            case VNT_IR_FLOAT: fprintf(out, " value=%.17g", node->value.floating); break;
            case VNT_IR_STRING:
            case VNT_IR_VARIABLE_DECL:
            case VNT_IR_REASSIGN:
            case VNT_IR_VARIABLE:
            case VNT_IR_MEMBER:
            case VNT_IR_CALL:
            case VNT_IR_FUNCTION:
            case VNT_IR_STRUCT: fprintf(out, " name/text=\"%s\"", node->value.text); break;
            case VNT_IR_BINARY:
            case VNT_IR_UNARY: fprintf(out, " op=%s", operation_name(node)); break;
            default: break;
        }
        if (node->type_name)
            fprintf(out, " type=%s", node->type_name);
        if (node->name_count) {
            fputs(" names=[", out);
            for (size_t j = 0; j < node->name_count; ++j)
                fprintf(out, "%s%s", j ? "," : "", node->names[j]);
            fputc(']', out);
        }
        fputc('\n', out);
    }
}

void vnt_ir_free(VntIrProgram *ir) {
    if (!ir) return;
    clear_nodes(ir);
    ir->program = NULL;
    ir->optimized_nodes = 0;
}


typedef struct {
    int kind; /* 1 = integer, 2 = boolean, 3 = float */
    union { int integer; int boolean; double floating; } value;
} HirConstant;

static int hir_constant(const VntIrProgram *ir, size_t index, HirConstant *out) {
    if (index >= ir->node_count) return 0;
    const VntIrNode *n = &ir->nodes[index];
    switch (n->opcode) {
        case VNT_IR_INTEGER: out->kind = 1; out->value.integer = n->value.integer; return 1;
        case VNT_IR_BOOLEAN: out->kind = 2; out->value.boolean = n->value.boolean; return 1;
        case VNT_IR_FLOAT: out->kind = 3; out->value.floating = n->value.floating; return 1;
        default: return 0;
    }
}

static int hir_fold_node(VntIrProgram *ir, size_t index, size_t *changed) {
    VntIrNode *n = &ir->nodes[index];
    size_t left = VNT_IR_NO_NODE, right = VNT_IR_NO_NODE;
    for (size_t c = n->first_child; c != VNT_IR_NO_NODE; c = ir->nodes[c].next_sibling) {
        VntIrEdgeRole role = ir->nodes[c].role;
        if (role == VNT_IR_EDGE_LEFT) left = c;
        else if (role == VNT_IR_EDGE_RIGHT) right = c;
        if (!hir_fold_node(ir, c, changed)) return 0;
    }
    if (n->opcode != VNT_IR_BINARY || left == VNT_IR_NO_NODE || right == VNT_IR_NO_NODE)
        return 1;
    HirConstant a, b;
    if (!hir_constant(ir, left, &a) || !hir_constant(ir, right, &b) || a.kind != b.kind)
        return 1;

    int result_int = 0, result_bool = 0, folded = 0;
    double result_float = 0.0;
    int kind = a.kind;
    int op = n->operation;
    if (kind == 1) {
        int64_t x = a.value.integer, y = b.value.integer, z = 0;
        switch (op) {
            case BINARY_ADD: z = x + y; folded = z >= INT_MIN && z <= INT_MAX; break;
            case BINARY_SUBTRACT: z = x - y; folded = z >= INT_MIN && z <= INT_MAX; break;
            case BINARY_MULTIPLY: z = x * y; folded = z >= INT_MIN && z <= INT_MAX; break;
            case BINARY_DIVIDE: if (y && !(x == INT_MIN && y == -1)) { z = x / y; folded = 1; } break;
            case BINARY_MODULO: if (y && !(x == INT_MIN && y == -1)) { z = x % y; folded = 1; } break;
            case BINARY_EQUAL: result_bool = x == y; kind = 2; folded = 1; break;
            case BINARY_NOT_EQUAL: result_bool = x != y; kind = 2; folded = 1; break;
            case BINARY_GREATER: result_bool = x > y; kind = 2; folded = 1; break;
            case BINARY_LESS: result_bool = x < y; kind = 2; folded = 1; break;
            case BINARY_GREATER_EQUAL: result_bool = x >= y; kind = 2; folded = 1; break;
            case BINARY_LESS_EQUAL: result_bool = x <= y; kind = 2; folded = 1; break;
            default: break;
        }
        result_int = (int)z;
    } else if (kind == 2) {
        int x = a.value.boolean, y = b.value.boolean;
        switch (op) {
            case BINARY_AND: result_bool = x && y; folded = 1; break;
            case BINARY_OR: result_bool = x || y; folded = 1; break;
            case BINARY_EQUAL: result_bool = x == y; folded = 1; break;
            case BINARY_NOT_EQUAL: result_bool = x != y; folded = 1; break;
            default: break;
        }
    } else if (kind == 3) {
        double x = a.value.floating, y = b.value.floating;
        switch (op) {
            case BINARY_ADD: result_float = x + y; folded = isfinite(result_float); break;
            case BINARY_SUBTRACT: result_float = x - y; folded = isfinite(result_float); break;
            case BINARY_MULTIPLY: result_float = x * y; folded = isfinite(result_float); break;
            case BINARY_DIVIDE: if (y != 0.0) { result_float = x / y; folded = isfinite(result_float); } break;
            case BINARY_EQUAL: result_bool = x == y; kind = 2; folded = 1; break;
            case BINARY_NOT_EQUAL: result_bool = x != y; kind = 2; folded = 1; break;
            case BINARY_GREATER: result_bool = x > y; kind = 2; folded = 1; break;
            case BINARY_LESS: result_bool = x < y; kind = 2; folded = 1; break;
            case BINARY_GREATER_EQUAL: result_bool = x >= y; kind = 2; folded = 1; break;
            case BINARY_LESS_EQUAL: result_bool = x <= y; kind = 2; folded = 1; break;
            default: break;
        }
    }
    if (!folded) return 1;
    n->opcode = kind == 1 ? VNT_IR_INTEGER : kind == 2 ? VNT_IR_BOOLEAN : VNT_IR_FLOAT;
    if (kind == 1) n->value.integer = result_int;
    else if (kind == 2) n->value.boolean = !!result_bool;
    else n->value.floating = result_float;
    /* Children are discarded by the compaction pass below. */
    n->first_child = n->last_child = VNT_IR_NO_NODE;
    n->child_count = 0;
    ++*changed;
    return 1;
}

static int hir_copy_node(const VntIrProgram *old, VntIrProgram *out,
                         size_t old_index, size_t *new_index) {
    if (old_index >= old->node_count || !reserve_node(out, new_index)) return 0;
    const VntIrNode *src = &old->nodes[old_index];
    VntIrNode *dst = &out->nodes[*new_index];
    dst->opcode = src->opcode;
    dst->role = src->role;
    dst->operation = src->operation;
    dst->source = src->source;
    dst->value = src->value;
    if (src->opcode == VNT_IR_STRING || src->opcode == VNT_IR_VARIABLE_DECL ||
        src->opcode == VNT_IR_REASSIGN || src->opcode == VNT_IR_VARIABLE ||
        src->opcode == VNT_IR_MEMBER || src->opcode == VNT_IR_CALL ||
        src->opcode == VNT_IR_FUNCTION || src->opcode == VNT_IR_STRUCT) {
        dst->value.text = NULL;
        if (!copy_text(&dst->value.text, src->value.text)) return 0;
    }
    if (src->type_name && !copy_text(&dst->type_name, src->type_name)) return 0;
    if (src->name_count) {
        dst->names = calloc(src->name_count, sizeof(*dst->names));
        if (!dst->names) return 0;
        dst->name_count = src->name_count;
        for (size_t i = 0; i < src->name_count; ++i)
            if (!copy_text(&dst->names[i], src->names[i])) return 0;
    }
    for (size_t child = src->first_child; child != VNT_IR_NO_NODE;
         child = old->nodes[child].next_sibling) {
        size_t copied;
        if (!hir_copy_node(old, out, child, &copied) ||
            !add_child(out, *new_index, copied)) return 0;
    }
    return 1;
}

int vnt_ir_optimize(VntIrProgram *ir) {
    if (!ir || !ir->nodes || !ir->node_count || !vnt_ir_validate(ir)) return 0;

    size_t changed = 0;
    if (!hir_fold_node(ir, ir->root, &changed)) return 0;

    VntIrProgram compact = {0};
    compact.root = VNT_IR_NO_NODE;
    compact.optimized_nodes = ir->optimized_nodes + changed;
    if (!hir_copy_node(ir, &compact, ir->root, &compact.root)) {
        vnt_ir_free(&compact);
        return 0;
    }
    /* Preserve the AST temporarily for the still-AST-based native backend. */
    AstNode *source_program = ir->program;
    /* The old flat array can now be released, including unreachable folded operands. */
    clear_nodes(ir);
    ir->program = source_program;
    ir->nodes = compact.nodes;
    ir->node_count = compact.node_count;
    ir->node_capacity = compact.node_capacity;
    ir->root = compact.root;
    ir->optimized_nodes = compact.optimized_nodes;
    return vnt_ir_validate(ir);
}

#include <vnt/ir.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int copy_text(char **destination, const char *source) {
    if (!source) source = "";
    size_t len = strlen(source);
    char *copy = malloc(len + 1);
    if (!copy) return 0;
    memcpy(copy, source, len + 1);
    *destination = copy;
    return 1;
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
            ((node->opcode == VNT_IR_STRING ||
              node->opcode == VNT_IR_VARIABLE_DECL ||
              node->opcode == VNT_IR_REASSIGN ||
              node->opcode == VNT_IR_VARIABLE ||
              node->opcode == VNT_IR_MEMBER ||
              node->opcode == VNT_IR_CALL ||
              node->opcode == VNT_IR_FUNCTION ||
              node->opcode == VNT_IR_STRUCT) && !node->value.text) ||
            ((node->name_count != 0) &&
             node->opcode != VNT_IR_FUNCTION && node->opcode != VNT_IR_STRUCT) ||
            (node->type_name &&
             node->opcode != VNT_IR_VARIABLE_DECL && node->opcode != VNT_IR_REASSIGN) ||
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
    }

    int valid = parents[ir->root] == 0;
    for (size_t i = 0; valid && i < ir->node_count; ++i)
        if (i != ir->root && parents[i] != 1) valid = 0;
    free(parents);
    return valid;
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
    /* The old flat array can now be released, including unreachable folded operands. */
    clear_nodes(ir);
    ir->nodes = compact.nodes;
    ir->node_count = compact.node_count;
    ir->node_capacity = compact.node_capacity;
    ir->root = compact.root;
    ir->optimized_nodes = compact.optimized_nodes;
    return vnt_ir_validate(ir);
}

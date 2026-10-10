#include <vnt/ir.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    /*
     * Parent counts alone do not reject a disconnected cycle. Walk from the
     * sole root and require every node to be reached exactly once.
     */
    unsigned char *visited = calloc(ir->node_count, sizeof(*visited));
    size_t *stack = malloc(ir->node_count * sizeof(*stack));
    if (!visited || !stack) {
        free(visited);
        free(stack);
        free(parents);
        return 0;
    }
    size_t top = 0, reached = 0;
    stack[top++] = ir->root;
    while (valid && top) {
        size_t current = stack[--top];
        if (current >= ir->node_count || visited[current]) {
            valid = 0;
            break;
        }
        visited[current] = 1;
        ++reached;
        for (size_t c = ir->nodes[current].first_child;
             c != VNT_IR_NO_NODE; c = ir->nodes[c].next_sibling) {
            if (c >= ir->node_count || visited[c] || top >= ir->node_count) {
                valid = 0;
                break;
            }
            stack[top++] = c;
        }
    }
    if (reached != ir->node_count) valid = 0;
    free(visited);
    free(stack);
    free(parents);
    return valid;
}



static const char *opcode_name(VntIrOpcode opcode) {
    static const char *const names[] = {
        "program", "print", "if", "while", "function", "struct", "call",
        "return", "break", "continue", "string", "integer", "float",
        "boolean", "array", "variable_decl", "variable", "index", "member",
        "assign", "binary", "unary", "reassign"
    };
    return (unsigned)opcode < sizeof(names) / sizeof(names[0]) ? names[opcode] : "invalid";
}

static const char *edge_role_name(VntIrEdgeRole role) {
    static const char *const names[] = {
        "root", "statement", "condition", "then", "else", "body", "value",
        "target", "argument", "element", "object", "index", "left", "right",
        "operand"
    };
    return (unsigned)role < sizeof(names) / sizeof(names[0]) ? names[role] : "invalid";
}

static const char *operation_name(const VntIrNode *node) {
    static const char *const binary[] = {
        "+", "-", "*", "/", "%", "==", "!=", ">", "<", ">=", "<=", "&&", "||"
    };
    static const char *const unary[] = { "!", "-", "&", "*" };
    if (node->opcode == VNT_IR_BINARY)
        return node->operation >= BINARY_ADD && node->operation <= BINARY_OR
            ? binary[node->operation] : "invalid";
    if (node->opcode == VNT_IR_UNARY)
        return node->operation >= UNARY_NOT && node->operation <= UNARY_DEREFERENCE
            ? unary[node->operation] : "invalid";
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


static size_t hir_child_role(const VntIrProgram *ir, size_t parent,
                             VntIrEdgeRole role) {
    for (size_t c = ir->nodes[parent].first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        if (ir->nodes[c].role == role) return c;
    return VNT_IR_NO_NODE;
}

static int statement_role(VntIrEdgeRole role) {
    return role == VNT_IR_EDGE_STATEMENT || role == VNT_IR_EDGE_THEN ||
           role == VNT_IR_EDGE_ELSE || role == VNT_IR_EDGE_BODY;
}

/* Drop statements after unconditional terminators within the same sequence.
   Child links are rebuilt so the compaction pass only copies reachable nodes. */
static void hir_prune_sequence(VntIrProgram *ir, size_t parent, size_t *changed) {
    unsigned char terminated[VNT_IR_EDGE_OPERAND + 1] = {0};
    VntIrNode *p = &ir->nodes[parent];
    size_t old = p->first_child, first = VNT_IR_NO_NODE, last = VNT_IR_NO_NODE;
    size_t kept = 0;
    while (old != VNT_IR_NO_NODE) {
        size_t current = old;
        old = ir->nodes[current].next_sibling;
        VntIrNode *child = &ir->nodes[current];
        int drop = statement_role(child->role) && terminated[child->role];
        if (drop) {
            ++*changed;
            continue;
        }
        if (first == VNT_IR_NO_NODE) first = current;
        else ir->nodes[last].next_sibling = current;
        last = current;
        ++kept;
        if (statement_role(child->role) &&
            (child->opcode == VNT_IR_RETURN || child->opcode == VNT_IR_BREAK ||
             child->opcode == VNT_IR_CONTINUE))
            terminated[child->role] = 1;
    }
    if (last != VNT_IR_NO_NODE) ir->nodes[last].next_sibling = VNT_IR_NO_NODE;
    p->first_child = first;
    p->last_child = last;
    p->child_count = kept;
}

/* Control simplification only removes code that is statically unreachable:
   constant if/while conditions and statements following unconditional exits. */
static void hir_simplify_control_flow(VntIrProgram *ir, size_t index,
                                      size_t *changed) {
    VntIrNode *n = &ir->nodes[index];
    for (size_t c = n->first_child; c != VNT_IR_NO_NODE;) {
        size_t next = ir->nodes[c].next_sibling;
        hir_simplify_control_flow(ir, c, changed);
        c = next;
    }

    if (n->opcode == VNT_IR_IF) {
        size_t cond = hir_child_role(ir, index, VNT_IR_EDGE_CONDITION);
        if (cond != VNT_IR_NO_NODE && ir->nodes[cond].opcode == VNT_IR_BOOLEAN) {
            VntIrEdgeRole chosen = ir->nodes[cond].value.boolean
                ? VNT_IR_EDGE_THEN : VNT_IR_EDGE_ELSE;
            size_t first = VNT_IR_NO_NODE, last = VNT_IR_NO_NODE, count = 0;
            for (size_t c = n->first_child; c != VNT_IR_NO_NODE;
                 c = ir->nodes[c].next_sibling) {
                if (ir->nodes[c].role != chosen) continue;
                if (first == VNT_IR_NO_NODE) first = c;
                last = c;
                ++count;
            }
            if (last != VNT_IR_NO_NODE) {
                ir->nodes[last].next_sibling = VNT_IR_NO_NODE;
                for (size_t c = first; c != VNT_IR_NO_NODE;
                     c = ir->nodes[c].next_sibling)
                    ir->nodes[c].role = VNT_IR_EDGE_STATEMENT;
            }
            n->opcode = VNT_IR_PROGRAM;
            n->first_child = first;
            n->last_child = last;
            n->child_count = count;
            ++*changed;
        }
    } else if (n->opcode == VNT_IR_WHILE) {
        size_t cond = hir_child_role(ir, index, VNT_IR_EDGE_CONDITION);
        if (cond != VNT_IR_NO_NODE && ir->nodes[cond].opcode == VNT_IR_BOOLEAN &&
            !ir->nodes[cond].value.boolean) {
            n->opcode = VNT_IR_PROGRAM;
            n->first_child = n->last_child = VNT_IR_NO_NODE;
            n->child_count = 0;
            ++*changed;
        }
    }

    hir_prune_sequence(ir, index, changed);
}



typedef struct {
    char *name;
    HirConstant constant;
} HirKnownConstant;

typedef struct {
    HirKnownConstant *items;
    size_t count;
    size_t capacity;
} HirConstantEnvironment;

static void hir_constants_clear(HirConstantEnvironment *env) {
    for (size_t i = 0; i < env->count; ++i) free(env->items[i].name);
    env->count = 0;
}

static void hir_constants_free(HirConstantEnvironment *env) {
    hir_constants_clear(env);
    free(env->items);
    memset(env, 0, sizeof(*env));
}

static size_t hir_constant_find(const HirConstantEnvironment *env, const char *name) {
    for (size_t i = 0; i < env->count; ++i)
        if (!strcmp(env->items[i].name, name)) return i;
    return VNT_IR_NO_NODE;
}

static void hir_constant_invalidate(HirConstantEnvironment *env, const char *name) {
    size_t index = hir_constant_find(env, name);
    if (index == VNT_IR_NO_NODE) return;
    free(env->items[index].name);
    env->items[index] = env->items[--env->count];
}

static int hir_constant_remember(HirConstantEnvironment *env, const char *name,
                                 HirConstant constant) {
    if (!name) return 1;
    size_t index = hir_constant_find(env, name);
    if (index == VNT_IR_NO_NODE) {
        if (env->count == env->capacity) {
            size_t cap = env->capacity ? env->capacity * 2 : 16;
            if (cap < env->capacity || cap > SIZE_MAX / sizeof(*env->items)) return 0;
            HirKnownConstant *items = realloc(env->items, cap * sizeof(*items));
            if (!items) return 0;
            env->items = items;
            env->capacity = cap;
        }
        index = env->count++;
        env->items[index].name = NULL;
        size_t length = strlen(name);
        env->items[index].name = malloc(length + 1);
        if (!env->items[index].name) {
            --env->count;
            return 0;
        }
        memcpy(env->items[index].name, name, length + 1);
    }
    env->items[index].constant = constant;
    return 1;
}

static void hir_replace_variable_with_constant(VntIrNode *node, HirConstant constant,
                                                size_t *changed) {
    free(node->value.text);
    node->opcode = constant.kind == 1 ? VNT_IR_INTEGER
        : constant.kind == 2 ? VNT_IR_BOOLEAN : VNT_IR_FLOAT;
    if (constant.kind == 1) node->value.integer = constant.value.integer;
    else if (constant.kind == 2) node->value.boolean = !!constant.value.boolean;
    else node->value.floating = constant.value.floating;
    ++*changed;
}

static int hir_contains_call(const VntIrProgram *ir, size_t index) {
    if (index == VNT_IR_NO_NODE || index >= ir->node_count) return 0;
    const VntIrNode *node = &ir->nodes[index];
    if (node->opcode == VNT_IR_CALL) return 1;
    for (size_t child = node->first_child; child != VNT_IR_NO_NODE;
         child = ir->nodes[child].next_sibling)
        if (hir_contains_call(ir, child)) return 1;
    return 0;
}

static void hir_substitute_expression(VntIrProgram *ir, size_t index,
                                      HirConstantEnvironment *env, size_t *changed) {
    if (index == VNT_IR_NO_NODE || index >= ir->node_count) return;
    VntIrNode *node = &ir->nodes[index];
    if (node->opcode == VNT_IR_VARIABLE) {
        size_t known = hir_constant_find(env, node->value.text);
        if (known != VNT_IR_NO_NODE)
            hir_replace_variable_with_constant(node, env->items[known].constant, changed);
        return;
    }
    /* Assignment expressions have ordering and aliasing effects; leave their
       complete subtree untouched rather than substituting across a write. */
    if (node->opcode == VNT_IR_ASSIGN) {
        size_t target = hir_child_role(ir, index, VNT_IR_EDGE_TARGET);
        if (target != VNT_IR_NO_NODE && ir->nodes[target].opcode == VNT_IR_VARIABLE)
            hir_constant_invalidate(env, ir->nodes[target].value.text);
        else
            /* An indirect write may alias any value currently known as constant. */
            hir_constants_clear(env);
        return;
    }
    /*
     * The operand of & is an lvalue, not a value read. Replacing &x with
     * &10 destroys the addressable-variable invariant required by codegen.
     */
    if (node->opcode == VNT_IR_UNARY && node->operation == UNARY_REFERENCE)
        return;
    for (size_t child = node->first_child; child != VNT_IR_NO_NODE;
         child = ir->nodes[child].next_sibling)
        hir_substitute_expression(ir, child, env, changed);
}

static int hir_propagate_sequence(VntIrProgram *ir, size_t parent,
                                  VntIrEdgeRole role,
                                  HirConstantEnvironment *env, size_t *changed);

static int hir_propagate_statement(VntIrProgram *ir, size_t index,
                                   HirConstantEnvironment *env, size_t *changed) {
    VntIrNode *node = &ir->nodes[index];
    if (node->opcode == VNT_IR_IF) {
        size_t condition = hir_child_role(ir, index, VNT_IR_EDGE_CONDITION);
        hir_substitute_expression(ir, condition, env, changed);
        for (size_t child = node->first_child; child != VNT_IR_NO_NODE;) {
            size_t next = ir->nodes[child].next_sibling;
            if (ir->nodes[child].role == VNT_IR_EDGE_THEN ||
                ir->nodes[child].role == VNT_IR_EDGE_ELSE) {
                HirConstantEnvironment branch = {0};
                if (ir->nodes[child].opcode == VNT_IR_PROGRAM &&
                    !hir_propagate_sequence(ir, child, VNT_IR_EDGE_STATEMENT,
                                            &branch, changed)) {
                    hir_constants_free(&branch);
                    return 0;
                }
                hir_constants_free(&branch);
            }
            child = next;
        }
        hir_constants_clear(env);
    } else if (node->opcode == VNT_IR_WHILE) {
        /*
         * Do not propagate constants into a loop condition or through its
         * body. Both execute repeatedly and may mutate values that were
         * constant before the loop. Rewriting even one read in this region
         * can turn a terminating loop into an infinite loop.
         *
         * The independent constant-folding pass still optimizes expressions
         * whose operands are literal constants, without relying on dataflow
         * facts that are invalid across iterations.
         */
        hir_constants_clear(env);
        return 1;
    } else if (node->opcode == VNT_IR_FUNCTION) {
        size_t body = hir_child_role(ir, index, VNT_IR_EDGE_BODY);
        if (body != VNT_IR_NO_NODE && ir->nodes[body].opcode == VNT_IR_PROGRAM) {
            HirConstantEnvironment function = {0};
            int ok = hir_propagate_sequence(ir, body, VNT_IR_EDGE_STATEMENT,
                                            &function, changed);
            hir_constants_free(&function);
            if (!ok) return 0;
        }
    } else if (node->opcode == VNT_IR_PROGRAM) {
        if (!hir_propagate_sequence(ir, index, VNT_IR_EDGE_STATEMENT, env, changed))
            return 0;
    } else if (node->opcode == VNT_IR_VARIABLE_DECL ||
               node->opcode == VNT_IR_REASSIGN) {
        size_t value = hir_child_role(ir, index, VNT_IR_EDGE_VALUE);
        if (value != VNT_IR_NO_NODE)
            hir_substitute_expression(ir, value, env, changed);
        HirConstant constant;
        if (value != VNT_IR_NO_NODE && hir_constant(ir, value, &constant)) {
            if (!hir_constant_remember(env, node->value.text, constant)) return 0;
        } else {
            hir_constant_invalidate(env, node->value.text);
        }
    } else if (node->opcode == VNT_IR_ASSIGN) {
        size_t target = hir_child_role(ir, index, VNT_IR_EDGE_TARGET);
        if (target != VNT_IR_NO_NODE && ir->nodes[target].opcode == VNT_IR_VARIABLE)
            hir_constant_invalidate(env, ir->nodes[target].value.text);
        else
            /* Indirect writes can mutate variables through references or aliases. */
            hir_constants_clear(env);
    } else {
        for (size_t child = node->first_child; child != VNT_IR_NO_NODE;
             child = ir->nodes[child].next_sibling)
            hir_substitute_expression(ir, child, env, changed);
    }

    /* Calls may mutate globals or values reachable through references. */
    if (hir_contains_call(ir, index)) hir_constants_clear(env);
    return 1;
}

static int hir_propagate_sequence(VntIrProgram *ir, size_t parent,
                                  VntIrEdgeRole role,
                                  HirConstantEnvironment *env, size_t *changed) {
    if (parent >= ir->node_count) return 0;
    for (size_t child = ir->nodes[parent].first_child; child != VNT_IR_NO_NODE;) {
        size_t next = ir->nodes[child].next_sibling;
        if (ir->nodes[child].role == role &&
            !hir_propagate_statement(ir, child, env, changed)) return 0;
        child = next;
    }
    return 1;
}

int vnt_ir_optimize(VntIrProgram *ir) {
    if (!ir || !ir->nodes || !ir->node_count || !vnt_ir_validate(ir)) return 0;

    size_t changed = 0;
    /*
     * Iterate to a small fixed point: propagation can expose foldable
     * expressions, and folding can expose constant branches. Every pass is
     * conservative across loops, branches, calls, and assignment expressions.
     */
    for (unsigned iteration = 0; iteration < 8; ++iteration) {
        size_t round_changed = 0;
        if (!hir_fold_node(ir, ir->root, &round_changed)) return 0;
        HirConstantEnvironment env = {0};
        int propagated = hir_propagate_sequence(ir, ir->root,
                                                VNT_IR_EDGE_STATEMENT,
                                                &env, &round_changed);
        hir_constants_free(&env);
        if (!propagated || !hir_fold_node(ir, ir->root, &round_changed)) return 0;
        hir_simplify_control_flow(ir, ir->root, &round_changed);
        changed += round_changed;
        if (!round_changed) break;
        /*
         * During rewriting, folded operands and pruned branches remain in the
         * old flat node array until compaction. The intermediate graph is
         * therefore intentionally not a fully reachable tree yet.
         */
    }

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

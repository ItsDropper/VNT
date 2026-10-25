#include <vnt/ir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const VntIrProgram *ir;
    FILE *out;
    char **variables;
    char **variable_types;
    size_t variable_count;
    size_t variable_capacity;
    int next_label;
    int loop_depth;
    int loop_start[64];
    int loop_end[64];
    int failed;
} FreestandingGen;

static const VntIrNode *child_role(const VntIrProgram *ir, size_t index,
                                   VntIrEdgeRole role) {
    if (index >= ir->node_count) return NULL;
    for (size_t c = ir->nodes[index].first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        if (ir->nodes[c].role == role) return &ir->nodes[c];
    return NULL;
}

static int variable_index(const FreestandingGen *g, const char *name) {
    for (size_t i = 0; i < g->variable_count; ++i)
        if (!strcmp(g->variables[i], name)) return (int)i;
    return -1;
}

static int add_variable(FreestandingGen *g, const char *name) {
    if (!name) return 0;
    if (variable_index(g, name) >= 0) return 1;
    if (g->variable_count == g->variable_capacity) {
        size_t capacity = g->variable_capacity ? g->variable_capacity * 2 : 16;
        char **variables = realloc(g->variables, capacity * sizeof(*variables));
        if (!variables) return 0;
        g->variables = variables;
        char **types = realloc(g->variable_types, capacity * sizeof(*types));
        if (!types) return 0;
        g->variable_types = types;
        g->variable_capacity = capacity;
    }
    g->variables[g->variable_count] = strdup(name);
    g->variable_types[g->variable_count] = strdup("int");
    if (!g->variables[g->variable_count] || !g->variable_types[g->variable_count]) return 0;
    ++g->variable_count;
    return 1;
}

static void collect_variables(FreestandingGen *g, size_t index) {
    if (index >= g->ir->node_count || g->failed) return;
    const VntIrNode *n = &g->ir->nodes[index];
    if ((n->opcode == VNT_IR_VARIABLE_DECL ||
         n->opcode == VNT_IR_REASSIGN ||
         n->opcode == VNT_IR_VARIABLE) && !add_variable(g, n->value.text)) {
        fprintf(stderr, "Freestanding backend: out of memory collecting locals.\n");
        g->failed = 1;
        return;
    }
    if (n->opcode == VNT_IR_VARIABLE_DECL && n->type_name) {
        int vi = variable_index(g, n->value.text);
        if (vi >= 0) {
            char *copy = strdup(n->type_name);
            if (!copy) { g->failed = 1; return; }
            free(g->variable_types[vi]);
            g->variable_types[vi] = copy;
        }
    }
    for (size_t c = n->first_child; c != VNT_IR_NO_NODE;
         c = g->ir->nodes[c].next_sibling)
        collect_variables(g, c);
}

static int variable_is_64(const FreestandingGen *g, int vi) {
    return vi >= 0 && (size_t)vi < g->variable_count &&
           g->variable_types[vi] && !strcmp(g->variable_types[vi], "i64");
}
static int expression_is_64(const FreestandingGen *g, size_t i) {
    if (i >= g->ir->node_count) return 0;
    const VntIrNode *n = &g->ir->nodes[i];
    if (n->opcode == VNT_IR_VARIABLE)
        return variable_is_64(g, variable_index(g, n->value.text));
    if (n->opcode == VNT_IR_UNARY) {
        const VntIrNode *c = child_role(g->ir, i, VNT_IR_EDGE_OPERAND);
        return c && expression_is_64(g, (size_t)(c - g->ir->nodes));
    }
    if (n->opcode == VNT_IR_BINARY) {
        const VntIrNode *l = child_role(g->ir, i, VNT_IR_EDGE_LEFT);
        const VntIrNode *r = child_role(g->ir, i, VNT_IR_EDGE_RIGHT);
        return (l && expression_is_64(g, (size_t)(l - g->ir->nodes))) ||
               (r && expression_is_64(g, (size_t)(r - g->ir->nodes)));
    }
    return 0;
}
static int label_new(FreestandingGen *g) { return g->next_label++; }
static void label_emit(FreestandingGen *g, int label) {
    fprintf(g->out, ".Lvnt_free_%d:\n", label);
}
static void fail(FreestandingGen *g, const char *message) {
    if (!g->failed) fprintf(stderr, "Freestanding backend: %s\n", message);
    g->failed = 1;
}

static int emit_expression(FreestandingGen *g, size_t index);
static int emit_statement(FreestandingGen *g, size_t index);

static int emit_expression(FreestandingGen *g, size_t index) {
    if (index >= g->ir->node_count) {
        fail(g, "missing expression.");
        return 0;
    }
    const VntIrNode *n = &g->ir->nodes[index];
    switch (n->opcode) {
    case VNT_IR_INTEGER:
        fprintf(g->out, "    movl $%d, %%eax\n", n->value.integer);
        return 1;
    case VNT_IR_BOOLEAN:
        fprintf(g->out, "    movl $%d, %%eax\n", !!n->value.boolean);
        return 1;
    case VNT_IR_VARIABLE: {
        int vi = variable_index(g, n->value.text);
        if (vi < 0) { fail(g, "unknown local variable."); return 0; }
        fprintf(g->out, "    movl -%d(%%rbp), %%eax\n", (vi + 1) * 8);
        return 1;
    }
    case VNT_IR_UNARY: {
        const VntIrNode *operand = child_role(g->ir, index, VNT_IR_EDGE_OPERAND);
        if (!operand || !emit_expression(g, (size_t)(operand - g->ir->nodes)))
            return 0;
        if (n->operation == UNARY_NEGATE) {
            fputs("    negl %eax\n    jo .Lvnt_arithmetic_fault\n", g->out);
            return 1;
        }
        if (n->operation == UNARY_NOT) {
            fputs("    testl %eax, %eax\n    sete %al\n    movzbl %al, %eax\n", g->out);
            return 1;
        }
        fail(g, "pointers and references are not yet supported by this freestanding subset.");
        return 0;
    }
    case VNT_IR_BINARY: {
        const VntIrNode *left = child_role(g->ir, index, VNT_IR_EDGE_LEFT);
        const VntIrNode *right = child_role(g->ir, index, VNT_IR_EDGE_RIGHT);
        if (!left || !right) { fail(g, "malformed binary expression."); return 0; }
        size_t li = (size_t)(left - g->ir->nodes);
        size_t ri = (size_t)(right - g->ir->nodes);
        if (n->operation == BINARY_AND || n->operation == BINARY_OR) {
            int short_label = label_new(g), done_label = label_new(g);
            if (!emit_expression(g, li)) return 0;
            fputs("    testl %eax, %eax\n", g->out);
            fprintf(g->out, n->operation == BINARY_AND
                    ? "    jz .Lvnt_free_%d\n" : "    jnz .Lvnt_free_%d\n",
                    short_label);
            if (!emit_expression(g, ri)) return 0;
            fputs("    testl %eax, %eax\n    setne %al\n    movzbl %al, %eax\n", g->out);
            fprintf(g->out, "    jmp .Lvnt_free_%d\n", done_label);
            label_emit(g, short_label);
            fprintf(g->out, "    movl $%d, %%eax\n",
                    n->operation == BINARY_AND ? 0 : 1);
            label_emit(g, done_label);
            return 1;
        }
        if (!emit_expression(g, li)) return 0;
        fputs("    pushq %rax\n", g->out);
        if (!emit_expression(g, ri)) return 0;
        fputs("    movl %eax, %ecx\n    popq %rax\n", g->out);
        switch (n->operation) {
        case BINARY_ADD:
            fputs("    addl %ecx, %eax\n    jo .Lvnt_arithmetic_fault\n", g->out); return 1;
        case BINARY_SUBTRACT:
            fputs("    subl %ecx, %eax\n    jo .Lvnt_arithmetic_fault\n", g->out); return 1;
        case BINARY_MULTIPLY:
            fputs("    imull %ecx, %eax\n    jo .Lvnt_arithmetic_fault\n", g->out); return 1;
        case BINARY_DIVIDE:
            fputs("    cltd\n    idivl %ecx\n", g->out); return 1;
        case BINARY_MODULO:
            fputs("    cltd\n    idivl %ecx\n    movl %edx, %eax\n", g->out); return 1;
        case BINARY_EQUAL: fputs("    cmpl %ecx, %eax\n    sete %al\n", g->out); break;
        case BINARY_NOT_EQUAL: fputs("    cmpl %ecx, %eax\n    setne %al\n", g->out); break;
        case BINARY_GREATER: fputs("    cmpl %ecx, %eax\n    setg %al\n", g->out); break;
        case BINARY_LESS: fputs("    cmpl %ecx, %eax\n    setl %al\n", g->out); break;
        case BINARY_GREATER_EQUAL: fputs("    cmpl %ecx, %eax\n    setge %al\n", g->out); break;
        case BINARY_LESS_EQUAL: fputs("    cmpl %ecx, %eax\n    setle %al\n", g->out); break;
        default: fail(g, "unsupported binary operator."); return 0;
        }
        fputs("    movzbl %al, %eax\n", g->out);
        return 1;
    }
    default:
        fail(g, "only integer/boolean expressions and local variables are supported; hosted values are rejected.");
        return 0;
    }
}

static int emit_statement(FreestandingGen *g, size_t index) {
    if (index >= g->ir->node_count) { fail(g, "missing statement."); return 0; }
    const VntIrNode *n = &g->ir->nodes[index];
    if (n->opcode == VNT_IR_PROGRAM) {
        for (size_t c = n->first_child; c != VNT_IR_NO_NODE;
             c = g->ir->nodes[c].next_sibling) {
            if (g->ir->nodes[c].role == VNT_IR_EDGE_STATEMENT &&
                !emit_statement(g, c)) return 0;
        }
        return 1;
    }
    if (n->opcode == VNT_IR_ASM) {
        fprintf(g->out, "    # VNT inline assembly follows\n    %s\n",
                n->value.text ? n->value.text : "");
        return 1;
    }
    if (n->opcode == VNT_IR_VARIABLE_DECL || n->opcode == VNT_IR_REASSIGN) {
        if (n->type_name && strcmp(n->type_name, "int") &&
            strcmp(n->type_name, "bool")) {
            fail(g, "freestanding locals must be int or bool in this initial target.");
            return 0;
        }
        const VntIrNode *value = child_role(g->ir, index, VNT_IR_EDGE_VALUE);
        int vi = variable_index(g, n->value.text);
        if (!value || vi < 0 || !emit_expression(g, (size_t)(value - g->ir->nodes)))
            return 0;
        fprintf(g->out, "    movl %%eax, -%d(%%rbp)\n", (vi + 1) * 8);
        return 1;
    }
    if (n->opcode == VNT_IR_ASSIGN) {
        const VntIrNode *target = child_role(g->ir, index, VNT_IR_EDGE_TARGET);
        const VntIrNode *value = child_role(g->ir, index, VNT_IR_EDGE_VALUE);
        if (!target || target->opcode != VNT_IR_VARIABLE || !value) {
            fail(g, "only assignment to local variables is supported.");
            return 0;
        }
        int vi = variable_index(g, target->value.text);
        if (vi < 0 || !emit_expression(g, (size_t)(value - g->ir->nodes)))
            return 0;
        fprintf(g->out, "    movl %%eax, -%d(%%rbp)\n", (vi + 1) * 8);
        return 1;
    }
    if (n->opcode == VNT_IR_RETURN) {
        const VntIrNode *value = child_role(g->ir, index, VNT_IR_EDGE_VALUE);
        if (value && !emit_expression(g, (size_t)(value - g->ir->nodes))) return 0;
        fputs("    leave\n    ret\n", g->out);
        return 1;
    }
    if (n->opcode == VNT_IR_IF) {
        const VntIrNode *condition = child_role(g->ir, index, VNT_IR_EDGE_CONDITION);
        if (!condition || !emit_expression(g, (size_t)(condition - g->ir->nodes))) return 0;
        int else_label = label_new(g), done_label = label_new(g);
        fputs("    testl %eax, %eax\n", g->out);
        fprintf(g->out, "    jz .Lvnt_free_%d\n", else_label);
        for (size_t c = n->first_child; c != VNT_IR_NO_NODE; c = g->ir->nodes[c].next_sibling)
            if (g->ir->nodes[c].role == VNT_IR_EDGE_THEN && !emit_statement(g, c)) return 0;
        fprintf(g->out, "    jmp .Lvnt_free_%d\n", done_label);
        label_emit(g, else_label);
        for (size_t c = n->first_child; c != VNT_IR_NO_NODE; c = g->ir->nodes[c].next_sibling)
            if (g->ir->nodes[c].role == VNT_IR_EDGE_ELSE && !emit_statement(g, c)) return 0;
        label_emit(g, done_label);
        return 1;
    }
    if (n->opcode == VNT_IR_WHILE) {
        if (g->loop_depth >= 64) { fail(g, "loop nesting exceeds 64."); return 0; }
        int start = label_new(g), done = label_new(g);
        g->loop_start[g->loop_depth] = start;
        g->loop_end[g->loop_depth] = done;
        ++g->loop_depth;
        label_emit(g, start);
        const VntIrNode *condition = child_role(g->ir, index, VNT_IR_EDGE_CONDITION);
        if (!condition || !emit_expression(g, (size_t)(condition - g->ir->nodes))) return 0;
        fputs("    testl %eax, %eax\n", g->out);
        fprintf(g->out, "    jz .Lvnt_free_%d\n", done);
        for (size_t c = n->first_child; c != VNT_IR_NO_NODE; c = g->ir->nodes[c].next_sibling)
            if (g->ir->nodes[c].role == VNT_IR_EDGE_BODY && !emit_statement(g, c)) return 0;
        fprintf(g->out, "    jmp .Lvnt_free_%d\n", start);
        label_emit(g, done);
        --g->loop_depth;
        return 1;
    }
    if (n->opcode == VNT_IR_BREAK || n->opcode == VNT_IR_CONTINUE) {
        if (!g->loop_depth) { fail(g, "break/continue used outside a loop."); return 0; }
        fprintf(g->out, "    jmp .Lvnt_free_%d\n",
                n->opcode == VNT_IR_BREAK ? g->loop_end[g->loop_depth - 1]
                                          : g->loop_start[g->loop_depth - 1]);
        return 1;
    }
    fail(g, "unsupported statement in freestanding target; no hosted fallback is allowed.");
    return 0;
}

static void emit_symbol(FILE *out, const char *name) {
    for (const unsigned char *p = (const unsigned char *)name; *p; ++p)
        fputc(isalnum(*p) || *p == '_' ? *p : '_', out);
}

/*
 * Freestanding x86-64 ELF subset. Values are signed 32-bit int/bool for now;
 * arithmetic overflow traps with UD2. No hosted runtime or libc references
 * are emitted. Unsupported source constructs fail compilation.
 */
int vnt_emit_freestanding_x86_64(const VntIrProgram *ir,
                                 const char *assembly_path) {
    if (!ir || !assembly_path || !vnt_ir_validate(ir)) {
        fprintf(stderr, "Freestanding backend: invalid HIR program.\n");
        return 0;
    }
    const VntIrNode *root = &ir->nodes[ir->root];
    const VntIrNode *kernel_main = NULL;
    for (size_t c = root->first_child; c != VNT_IR_NO_NODE; c = ir->nodes[c].next_sibling) {
        const VntIrNode *n = &ir->nodes[c];
        if (n->opcode == VNT_IR_FUNCTION && n->value.text &&
            !strcmp(n->value.text, "kernel_main")) {
            if (n->name_count != 0) {
                fprintf(stderr, "Freestanding backend: kernel_main must take zero arguments.\n");
                return 0;
            }
            if (kernel_main) {
                fprintf(stderr, "Freestanding backend: duplicate kernel_main.\n");
                return 0;
            }
            kernel_main = n;
        } else {
            fprintf(stderr, "Freestanding backend: only kernel_main() is allowed at top level in this initial target.\n");
            return 0;
        }
    }
    if (!kernel_main) {
        fprintf(stderr, "Freestanding backend: define fun kernel_main() as the entry routine.\n");
        return 0;
    }

    FreestandingGen g = {0};
    g.ir = ir;
    collect_variables(&g, (size_t)(kernel_main - ir->nodes));
    if (g.failed) goto cleanup;

    FILE *out = fopen(assembly_path, "wb");
    if (!out) {
        fprintf(stderr, "Freestanding backend: cannot create '%s'.\n", assembly_path);
        g.failed = 1;
        goto cleanup;
    }
    g.out = out;
    fputs(".section .text\n"
          ".code64\n"
          ".globl _start\n"
          ".type _start, @function\n"
          "_start:\n"
          "    cli\n"
          "    lea freestanding_stack_top(%rip), %rsp\n"
          "    andq $-16, %rsp\n"
          "    call vnt_fn_kernel_main\n"
          ".Lvnt_halt_forever:\n"
          "    cli\n"
          "    hlt\n"
          "    jmp .Lvnt_halt_forever\n"
          ".size _start, .-_start\n\n"
          ".globl vnt_fn_kernel_main\n"
          ".type vnt_fn_kernel_main, @function\n"
          "vnt_fn_kernel_main:\n"
          "    pushq %rbp\n"
          "    movq %rsp, %rbp\n", out);
    size_t frame = g.variable_count * 8;
    frame = (frame + 15) & ~(size_t)15;
    if (frame) fprintf(out, "    subq $%zu, %%rsp\n", frame);
    for (size_t s = kernel_main->first_child; s != VNT_IR_NO_NODE; s = ir->nodes[s].next_sibling) {
        if (ir->nodes[s].role == VNT_IR_EDGE_BODY &&
            !emit_statement(&g, s)) break;
    }
    if (!g.failed) fputs("    leave\n    ret\n", out);
    fputs(".Lvnt_arithmetic_fault:\n    ud2\n"
          ".size vnt_fn_kernel_main, .-vnt_fn_kernel_main\n"
          ".section .bss\n"
          ".balign 16\n"
          ".globl freestanding_stack_bottom\n"
          "freestanding_stack_bottom:\n"
          "    .skip 16384\n"
          ".globl freestanding_stack_top\n"
          "freestanding_stack_top:\n"
          ".section .note.GNU-stack,\"\",@progbits\n", out);
    int write_failed = ferror(out);
    if (fclose(out) != 0) write_failed = 1;
    if (write_failed) g.failed = 1;
    if (g.failed) remove(assembly_path);

cleanup:
    for (size_t i = 0; i < g.variable_count; ++i) free(g.variables[i]);
    free(g.variables);
    return !g.failed;
}

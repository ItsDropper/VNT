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
           g->variable_types[vi] &&
           (!strcmp(g->variable_types[vi], "int") ||
            !strcmp(g->variable_types[vi], "i64"));
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

static void emit_symbol(FILE *out, const char *name);
static int emit_expression(FreestandingGen *g, size_t index);
static int emit_statement(FreestandingGen *g, size_t index);

static const VntIrNode *find_function(const VntIrProgram *ir, const char *name) {
    if (!ir || !name || ir->root >= ir->node_count) return NULL;
    const VntIrNode *root = &ir->nodes[ir->root];
    for (size_t c = root->first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling) {
        const VntIrNode *n = &ir->nodes[c];
        if (n->opcode == VNT_IR_FUNCTION && n->value.text &&
            !strcmp(n->value.text, name)) return n;
    }
    return NULL;
}

static void emit_function_symbol(FILE *out, const char *name) {
    fputs("vnt_fn_", out);
    emit_symbol(out, name);
}

static int emit_expression(FreestandingGen *g, size_t index) {
    if (index >= g->ir->node_count) {
        fail(g, "missing expression.");
        return 0;
    }
    const VntIrNode *n = &g->ir->nodes[index];
    switch (n->opcode) {
    case VNT_IR_CALL: {
        if (!n->value.text) { fail(g, "call has no function name."); return 0; }
        const VntIrNode *target = find_function(g->ir, n->value.text);
        if (!target) { fail(g, "call to unknown freestanding function."); return 0; }
        if (n->child_count != target->name_count || target->name_count > 6) {
            fail(g, "freestanding calls require matching arity and at most six register arguments.");
            return 0;
        }
        static const char *arg_regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        size_t argc = target->name_count;
        size_t area = ((argc + 1) & ~(size_t)1) * 8;
        if (area) fprintf(g->out, "    subq $%zu, %%rsp\n", area);
        size_t ai = 0;
        for (size_t c = n->first_child; c != VNT_IR_NO_NODE; c = g->ir->nodes[c].next_sibling) {
            if (g->ir->nodes[c].role != VNT_IR_EDGE_ARGUMENT) continue;
            if (!emit_expression(g, c)) return 0;
            fprintf(g->out, "    movq %%rax, %zu(%%rsp)\n", ai++ * 8);
        }
        for (size_t i = 0; i < argc; ++i)
            fprintf(g->out, "    movq %zu(%%rsp), %s\n", i * 8, arg_regs[i]);
        fputs("    call ", g->out);
        emit_function_symbol(g->out, n->value.text);
        fputc('\n', g->out);
        if (area) fprintf(g->out, "    addq $%zu, %%rsp\n", area);
        return 1;
    }
    case VNT_IR_INTEGER:
        if (n->value.integer < INT32_MIN || n->value.integer > INT32_MAX)
            fprintf(g->out, "    movabsq $%lld, %%rax\n", (long long)n->value.integer);
        else
            fprintf(g->out, "    movl $%d, %%eax\n", (int)n->value.integer);
        return 1;
    case VNT_IR_BOOLEAN:
        fprintf(g->out, "    movl $%d, %%eax\n", !!n->value.boolean);
        return 1;
    case VNT_IR_VARIABLE: {
        int vi = variable_index(g, n->value.text);
        if (vi < 0) { fail(g, "unknown local variable."); return 0; }
        if (variable_is_64(g, vi)) fprintf(g->out, "    movq -%d(%%rbp), %%rax\n", (vi + 1) * 8);
        else fprintf(g->out, "    movl -%d(%%rbp), %%eax\n", (vi + 1) * 8);
        return 1;
    }
    case VNT_IR_UNARY: {
        const VntIrNode *operand = child_role(g->ir, index, VNT_IR_EDGE_OPERAND);
        if (!operand || !emit_expression(g, (size_t)(operand - g->ir->nodes)))
            return 0;
        if (n->operation == UNARY_NEGATE) {
            fputs("    negq %rax\n    jo .Lvnt_arithmetic_fault\n", g->out);
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
        int wide = 1; /* All freestanding VNT integer arithmetic is 64-bit. */
        if (!emit_expression(g, li)) return 0;
        fputs("    pushq %rax\n", g->out);
        if (!emit_expression(g, ri)) return 0;
        fputs(wide ? "    movq %rax, %rcx\n    popq %rax\n" : "    movl %eax, %ecx\n    popq %rax\n", g->out);
        switch (n->operation) {
        case BINARY_ADD:
            fputs(wide ? "    addq %rcx, %rax\n    jo .Lvnt_arithmetic_fault\n" : "    addl %ecx, %eax\n    jo .Lvnt_arithmetic_fault\n", g->out); return 1;
        case BINARY_SUBTRACT:
            fputs(wide ? "    subq %rcx, %rax\n    jo .Lvnt_arithmetic_fault\n" : "    subl %ecx, %eax\n    jo .Lvnt_arithmetic_fault\n", g->out); return 1;
        case BINARY_MULTIPLY:
            fputs(wide ? "    imulq %rcx, %rax\n    jo .Lvnt_arithmetic_fault\n" : "    imull %ecx, %eax\n    jo .Lvnt_arithmetic_fault\n", g->out); return 1;
        case BINARY_DIVIDE:
            fputs(wide ? "    cqto\n    idivq %rcx\n" : "    cltd\n    idivl %ecx\n", g->out); return 1;
        case BINARY_MODULO:
            fputs(wide ? "    cqto\n    idivq %rcx\n    movq %rdx, %rax\n" : "    cltd\n    idivl %ecx\n    movl %edx, %eax\n", g->out); return 1;
        case BINARY_EQUAL: fputs(wide ? "    cmpq %rcx, %rax\n    sete %al\n" : "    cmpl %ecx, %eax\n    sete %al\n", g->out); break;
        case BINARY_NOT_EQUAL: fputs(wide ? "    cmpq %rcx, %rax\n    setne %al\n" : "    cmpl %ecx, %eax\n    setne %al\n", g->out); break;
        case BINARY_GREATER: fputs(wide ? "    cmpq %rcx, %rax\n    setg %al\n" : "    cmpl %ecx, %eax\n    setg %al\n", g->out); break;
        case BINARY_LESS: fputs(wide ? "    cmpq %rcx, %rax\n    setl %al\n" : "    cmpl %ecx, %eax\n    setl %al\n", g->out); break;
        case BINARY_GREATER_EQUAL: fputs(wide ? "    cmpq %rcx, %rax\n    setge %al\n" : "    cmpl %ecx, %eax\n    setge %al\n", g->out); break;
        case BINARY_LESS_EQUAL: fputs(wide ? "    cmpq %rcx, %rax\n    setle %al\n" : "    cmpl %ecx, %eax\n    setle %al\n", g->out); break;
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
    if (n->opcode == VNT_IR_CALL) return emit_expression(g, index);
    if (n->opcode == VNT_IR_ASM) {
        fprintf(g->out, "    # VNT inline assembly follows\n    %s\n",
                n->value.text ? n->value.text : "");
        return 1;
    }
    if (n->opcode == VNT_IR_VARIABLE_DECL || n->opcode == VNT_IR_REASSIGN) {
        if (n->type_name && strcmp(n->type_name, "int") &&
            strcmp(n->type_name, "bool") && strcmp(n->type_name, "i64")) {
            fail(g, "freestanding locals support int, bool, and i64.");
            return 0;
        }
        const VntIrNode *value = child_role(g->ir, index, VNT_IR_EDGE_VALUE);
        int vi = variable_index(g, n->value.text);
        if (!value || vi < 0 || !emit_expression(g, (size_t)(value - g->ir->nodes)))
            return 0;
        if (variable_is_64(g, vi)) fprintf(g->out, "    movq %%rax, -%d(%%rbp)\n", (vi + 1) * 8);
        else fprintf(g->out, "    movl %%eax, -%d(%%rbp)\n", (vi + 1) * 8);
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
        if (variable_is_64(g, vi)) fprintf(g->out, "    movq %%rax, -%d(%%rbp)\n", (vi + 1) * 8);
        else fprintf(g->out, "    movl %%eax, -%d(%%rbp)\n", (vi + 1) * 8);
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
 * Freestanding x86-64 ELF subset. Integer values use 64-bit registers and
 * stack slots; booleans use 32-bit values. Signed arithmetic overflow traps
 * with UD2. No hosted runtime or libc references are emitted.
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
        if (n->opcode != VNT_IR_FUNCTION || !n->value.text) {
            fprintf(stderr, "Freestanding backend: only function declarations are allowed at top level.\n");
            return 0;
        }
        if (!strcmp(n->value.text, "kernel_main")) {
            if (n->name_count != 0) {
                fprintf(stderr, "Freestanding backend: kernel_main must take zero arguments.\n");
                return 0;
            }
            if (kernel_main) {
                fprintf(stderr, "Freestanding backend: duplicate kernel_main.\n");
                return 0;
            }
            kernel_main = n;
        }
        for (size_t d = ir->nodes[c].next_sibling; d != VNT_IR_NO_NODE; d = ir->nodes[d].next_sibling) {
            const VntIrNode *other = &ir->nodes[d];
            if (other->opcode == VNT_IR_FUNCTION && other->value.text &&
                !strcmp(other->value.text, n->value.text)) {
                fprintf(stderr, "Freestanding backend: duplicate function name.\n");
                return 0;
            }
        }
    }
    if (!kernel_main) {
        fprintf(stderr, "Freestanding backend: define fun kernel_main() as the entry routine.\n");
        return 0;
    }

    FreestandingGen g = {0};
    g.ir = ir;
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
          "    call ", out);
    emit_function_symbol(out, "kernel_main");
    fputs("\n.Lvnt_halt_forever:\n"
          "    cli\n"
          "    hlt\n"
          "    jmp .Lvnt_halt_forever\n"
          ".size _start, .-_start\n\n", out);

    for (size_t fn = root->first_child; fn != VNT_IR_NO_NODE;
         fn = ir->nodes[fn].next_sibling) {
        const VntIrNode *function = &ir->nodes[fn];
        if (function->opcode != VNT_IR_FUNCTION) continue;
        if (function->name_count > 6) {
            fail(&g, "freestanding functions support at most six parameters.");
            break;
        }
        for (size_t i = 0; i < g.variable_count; ++i) {
            free(g.variables[i]);
            free(g.variable_types[i]);
        }
        g.variable_count = 0;
        for (size_t p = 0; p < function->name_count; ++p) {
            if (!add_variable(&g, function->names[p])) {
                fail(&g, "out of memory collecting function parameters.");
                break;
            }
        }
        if (g.failed) break;
        collect_variables(&g, fn);
        if (g.failed) break;
        fputs(".globl ", out);
        emit_function_symbol(out, function->value.text);
        fputs("\n.type ", out);
        emit_function_symbol(out, function->value.text);
        fputs(", @function\n", out);
        emit_function_symbol(out, function->value.text);
        fputs(":\n    pushq %rbp\n    movq %rsp, %rbp\n", out);
        size_t frame = g.variable_count * 8;
        frame = (frame + 15) & ~(size_t)15;
        if (frame) fprintf(out, "    subq $%zu, %%rsp\n", frame);
        static const char *arg_regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        for (size_t p = 0; p < function->name_count; ++p)
            fprintf(out, "    movq %s, -%zu(%%rbp)\n", arg_regs[p], (p + 1) * 8);
        for (size_t st = function->first_child; st != VNT_IR_NO_NODE;
             st = ir->nodes[st].next_sibling) {
            if (ir->nodes[st].role == VNT_IR_EDGE_BODY &&
                !emit_statement(&g, st)) break;
        }
        if (!g.failed) fputs("    leave\n    ret\n", out);
        fputs(".size ", out);
        emit_function_symbol(out, function->value.text);
        fputs(", .-", out);
        emit_function_symbol(out, function->value.text);
        fputc('\n', out);
        if (g.failed) break;
    }
    fputs(".Lvnt_arithmetic_fault:\n    ud2\n"
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
    for (size_t i = 0; i < g.variable_count; ++i) { free(g.variables[i]); free(g.variable_types[i]); }
    free(g.variables);
    free(g.variable_types);
    return !g.failed;
}

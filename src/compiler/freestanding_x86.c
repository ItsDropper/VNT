#include <vnt/ir.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static const VntIrNode *child_role(const VntIrProgram *ir, size_t index,
                                   VntIrEdgeRole role) {
    if (index >= ir->node_count) return NULL;
    for (size_t c = ir->nodes[index].first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        if (ir->nodes[c].role == role) return &ir->nodes[c];
    return NULL;
}

static void emit_symbol(FILE *out, const char *name) {
    for (const unsigned char *p = (const unsigned char *)name; *p; ++p)
        fputc(isalnum(*p) || *p == '_' ? *p : '_', out);
}

static int emit_statement(const VntIrProgram *ir, FILE *out, size_t index,
                          const char *function_name) {
    const VntIrNode *stmt = &ir->nodes[index];
    if (stmt->opcode == VNT_IR_PROGRAM) {
        for (size_t c = stmt->first_child; c != VNT_IR_NO_NODE;
             c = ir->nodes[c].next_sibling) {
            if (ir->nodes[c].role != VNT_IR_EDGE_STATEMENT) continue;
            if (!emit_statement(ir, out, c, function_name)) return 0;
        }
        return 1;
    }
    if (stmt->opcode == VNT_IR_ASM) {
        fprintf(out, "    # VNT inline assembly follows\n    %s\n",
                stmt->value.text ? stmt->value.text : "");
        return 1;
    }
    if (stmt->opcode == VNT_IR_RETURN &&
        !child_role(ir, index, VNT_IR_EDGE_VALUE)) {
        fputs("    ret\n", out);
        return 1;
    }
    fprintf(stderr,
        "Freestanding backend: function '%s' contains unsupported HIR opcode %d; no hosted fallback is allowed.\n",
        function_name, (int)stmt->opcode);
    return 0;
}

/*
 * Strict initial freestanding subset: System V x86-64 assembly, a zero-argument
 * kernel_main entry routine, and bodies containing only inline assembly, nested
 * unsafe blocks, and bare return. No VNT runtime or libc references are emitted.
 * Unsupported constructs are hard errors rather than hosted-runtime fallbacks.
 */
int vnt_emit_freestanding_x86_64(const VntIrProgram *ir,
                                 const char *assembly_path) {
    if (!ir || !assembly_path || !vnt_ir_validate(ir)) {
        fprintf(stderr, "Freestanding backend: invalid HIR program.\n");
        return 0;
    }
    const VntIrNode *root = &ir->nodes[ir->root];
    const VntIrNode *kernel_main = NULL;
    for (size_t c = root->first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling) {
        const VntIrNode *n = &ir->nodes[c];
        if (n->opcode == VNT_IR_FUNCTION &&
            n->value.text && !strcmp(n->value.text, "kernel_main")) {
            if (n->name_count != 0) {
                fprintf(stderr, "Freestanding backend: kernel_main must take zero arguments in this initial target.\n");
                return 0;
            }
            if (kernel_main) {
                fprintf(stderr, "Freestanding backend: duplicate kernel_main.\n");
                return 0;
            }
            kernel_main = n;
        } else if (n->opcode != VNT_IR_FUNCTION && n->opcode != VNT_IR_STRUCT) {
            fprintf(stderr,
                "Freestanding backend: top-level statements and globals are unsupported; no hosted fallback is allowed.\n");
            return 0;
        }
    }
    if (!kernel_main) {
        fprintf(stderr, "Freestanding backend: define fun kernel_main() as the entry routine.\n");
        return 0;
    }

    FILE *out = fopen(assembly_path, "wb");
    if (!out) {
        fprintf(stderr, "Freestanding backend: cannot create '%s'.\n",
                assembly_path);
        return 0;
    }

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
          ".size _start, .-_start\n\n", out);

    for (size_t c = root->first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling) {
        const VntIrNode *n = &ir->nodes[c];
        if (n->opcode != VNT_IR_FUNCTION) continue;
        fputs(".globl vnt_fn_", out);
        emit_symbol(out, n->value.text);
        fputs("\n.type vnt_fn_", out);
        emit_symbol(out, n->value.text);
        fputs(", @function\nvnt_fn_", out);
        emit_symbol(out, n->value.text);
        fputs(":\n", out);

        for (size_t s = n->first_child; s != VNT_IR_NO_NODE;
             s = ir->nodes[s].next_sibling) {
            if (ir->nodes[s].role != VNT_IR_EDGE_BODY) continue;
            if (!emit_statement(ir, out, s, n->value.text)) {
                fclose(out);
                remove(assembly_path);
                return 0;
            }
        }
        fputs("    ret\n.size vnt_fn_", out);
        emit_symbol(out, n->value.text);
        fputs(", .-vnt_fn_", out);
        emit_symbol(out, n->value.text);
        fputs("\n\n", out);
    }

    fputs(".section .bss\n"
          ".balign 16\n"
          ".globl freestanding_stack_bottom\n"
          "freestanding_stack_bottom:\n"
          "    .skip 16384\n"
          ".globl freestanding_stack_top\n"
          "freestanding_stack_top:\n"
          ".section .note.GNU-stack,\"\",@progbits\n", out);

    int ok = !ferror(out);
    if (fclose(out) != 0) ok = 0;
    if (!ok) {
        remove(assembly_path);
        fprintf(stderr, "Freestanding backend: failed writing assembly output.\n");
    }
    return ok;
}

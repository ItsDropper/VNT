#include <vnt/ir.h>
#include <vnt/ast.h>
#include "x86_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int file_contains(const char *path, const char *needle) {
    FILE *file = fopen(path, "rb");
    if (!file) return 0;
    char buffer[65536];
    size_t size = fread(buffer, 1, sizeof(buffer) - 1, file);
    fclose(file);
    buffer[size] = '\0';
    return strstr(buffer, needle) != NULL;
}

int main(void) {
    const char *assembly = "tests/out/ast_free_native.s";
    AstNode *program = ast_create_program(
        ast_create_print(ast_create_binary(ast_create_integer(20),
                                           ast_create_integer(22),
                                           BINARY_ADD)));
    VntIrProgram ir = {0};
    if (!program || !vnt_ir_lower(&ir, program)) {
        fputs("FAIL: HIR lowering failed\n", stderr);
        ast_free(program);
        vnt_ir_free(&ir);
        return 1;
    }

    ast_free(program);
    program = NULL;

    if (!vnt_ir_validate(&ir) || !vnt_ir_optimize(&ir) ||
        !vnt_ir_validate(&ir)) {
        fputs("FAIL: HIR validation/optimization failed after AST destruction\n", stderr);
        vnt_ir_free(&ir);
        return 1;
    }

    FILE *dump = tmpfile();
    if (!dump) {
        fputs("FAIL: could not allocate HIR dump stream\n", stderr);
        vnt_ir_free(&ir);
        return 1;
    }
    vnt_ir_dump(&ir, dump);
    fclose(dump);

    if (!vnt_emit_x86_64(&ir, assembly) ||
        !file_contains(assembly, "main:") ||
        !file_contains(assembly, "vnt_print")) {
        fputs("FAIL: native assembly emission failed after AST destruction\n", stderr);
        vnt_ir_free(&ir);
        remove(assembly);
        return 1;
    }

    vnt_ir_free(&ir);
    puts("AST-free HIR pipeline: PASS");
    return 0;
}

#include <vnt/compiler.h>
#include <vnt/typecheck.h>
#include <vnt/ir.h>
#include <stdio.h>

#include "x86_backend.h"

int compiler_compile(AstNode *program, const char *assembly_path) {
    if (!vnt_typecheck(program)) {
        fprintf(stderr, "Native compiler: type checking failed.\n");
        return 0;
    }

    VntIrProgram ir;
    if (!vnt_ir_lower(&ir, program) || !vnt_ir_optimize(&ir)) {
        fprintf(stderr, "Native compiler: IR lowering/optimization failed.\n");
        return 0;
    }

    if (!vnt_emit_x86_64(ir.program, assembly_path)) {
        fprintf(stderr, "Native compiler: x86-64 code generation failed.\n");
        return 0;
    }

    return 1;
}

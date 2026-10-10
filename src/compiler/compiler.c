#include <vnt/compiler.h>
#include <vnt/typecheck.h>
#include <vnt/ir.h>
#include <vnt/ir_lower.h>
#include <vnt/cfg.h>
#include <stdio.h>

#include "x86_backend.h"

int compiler_compile(AstNode *program, const char *assembly_path) {
    if (!vnt_typecheck(program)) {
        fprintf(stderr, "Native compiler: type checking failed.\n");
        return 0;
    }

    VntIrProgram ir = {0};
    VntCfg cfg = {0};
    if (!vnt_ir_lower(&ir, program)) {
        fprintf(stderr, "Native compiler: IR lowering failed.\n");
        vnt_ir_free(&ir);
        return 0;
    }
    if (!vnt_ir_optimize(&ir) || !vnt_ir_validate(&ir)) {
        fprintf(stderr, "Native compiler: IR optimization/validation failed.\n");
        vnt_ir_free(&ir);
        return 0;
    }
    if (!vnt_cfg_build(&ir, &cfg) || !vnt_cfg_validate(&cfg)) {
        fprintf(stderr, "Native compiler: control-flow graph construction failed.\n");
        vnt_cfg_free(&cfg);
        vnt_ir_free(&ir);
        return 0;
    }
    vnt_cfg_free(&cfg);

    if (!vnt_emit_x86_64(&ir, assembly_path)) {
        fprintf(stderr, "Native compiler: x86-64 code generation failed.\n");
        vnt_ir_free(&ir);
        return 0;
    }

    vnt_ir_free(&ir);
    return 1;
}

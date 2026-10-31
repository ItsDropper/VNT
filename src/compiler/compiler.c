#include <vnt/compiler.h>
#include <vnt/typecheck.h>
#include <vnt/ir.h>
#include <vnt/ir_lower.h>
#include <vnt/cfg.h>
#include <stdio.h>

#include "x86_backend.h"

int vnt_emit_freestanding_x86_64(const VntIrProgram *ir,
                                 const char *assembly_path);

static int compile_mode(AstNode *program, const char *assembly_path,
                        int freestanding) {
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
    char dataflow_diagnostic[256];
    if (!vnt_cfg_check_definite_assignment(&ir, &cfg, dataflow_diagnostic,
                                           sizeof(dataflow_diagnostic))) {
        fprintf(stderr, "Native compiler: %s.\n", dataflow_diagnostic);
        vnt_cfg_free(&cfg);
        vnt_ir_free(&ir);
        return 0;
    }
    vnt_cfg_free(&cfg);

    int emitted = freestanding
        ? vnt_emit_freestanding_x86_64(&ir, assembly_path)
        : vnt_emit_x86_64(&ir, assembly_path);
    if (!emitted) {
        /* Never leave a partial or stale assembly artifact after a rejected
           program. Callers and build scripts rely on failure being atomic. */
        remove(assembly_path);
        fprintf(stderr, "%s compiler: x86-64 code generation failed.\n",
                freestanding ? "Freestanding" : "Native");
        vnt_ir_free(&ir);
        return 0;
    }

    vnt_ir_free(&ir);
    return 1;
}

int compiler_compile(AstNode *program, const char *assembly_path) {
    return compile_mode(program, assembly_path, 0);
}

int compiler_compile_freestanding(AstNode *program, const char *assembly_path) {
    return compile_mode(program, assembly_path, 1);
}

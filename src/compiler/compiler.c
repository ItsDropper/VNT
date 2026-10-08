#include <vnt/compiler.h>
#include <stdio.h>

#include "x86_backend.h"

int compiler_compile(AstNode *program, const char *assembly_path) {
    if (!vnt_emit_x86_64(program, assembly_path)) {
        fprintf(stderr, "Native compiler: x86-64 code generation failed.\n");
        return 0;
    }

    return 1;
}

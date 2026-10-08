#ifndef VNT_NATIVE_BACKEND_H
#define VNT_NATIVE_BACKEND_H

#include <vnt/ast.h>
#include <stdio.h>

typedef struct {
    FILE *out;
    char **vars;
    int var_count;
    int var_capacity;
    int label_count;
    int error;
} CGen;

int vnt_emit_c_program(AstNode *program, const char *c_path);

#endif

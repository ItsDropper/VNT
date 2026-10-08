#ifndef VNT_IR_H
#define VNT_IR_H

#include <vnt/ast.h>

typedef struct {
    AstNode *program;
    int optimized_nodes;
} VntIrProgram;

int vnt_ir_lower(VntIrProgram *ir, AstNode *program);
int vnt_ir_optimize(VntIrProgram *ir);

#endif

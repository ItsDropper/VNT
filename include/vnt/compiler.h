#ifndef VNT_COMPILER_H
#define VNT_COMPILER_H

#include <vnt/ast.h>

int compiler_compile(AstNode *program, const char *assembly_path);

#endif

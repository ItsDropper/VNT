#ifndef VNT_X86_BACKEND_H
#define VNT_X86_BACKEND_H

#include <vnt/ast.h>

int vnt_emit_x86_64(AstNode *program, const char *assembly_path);

#endif

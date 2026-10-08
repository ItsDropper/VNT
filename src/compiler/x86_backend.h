#ifndef VNT_X86_BACKEND_H
#define VNT_X86_BACKEND_H

#include <vnt/ir.h>

int vnt_emit_x86_64(const VntIrProgram *ir, const char *assembly_path);

#endif

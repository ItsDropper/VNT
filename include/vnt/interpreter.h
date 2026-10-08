#ifndef VNT_INTERPRETER_H
#define VNT_INTERPRETER_H

#include <vnt/ast.h>
#include <vnt/environment.h>

int interpreter_execute(
    AstNode *program,
    Environment *environment
);

#endif


#ifndef VNT_INTERPRETER_H
#define VNT_INTERPRETER_H

#include "ast.h"
#include "environment.h"

void interpreter_execute(
    AstNode *program,
    Environment *environment
);

#endif


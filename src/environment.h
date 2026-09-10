#ifndef VNT_ENVIRONMENT_H
#define VNT_ENVIRONMENT_H

#include "ast.h"
#include "value.h"

typedef struct {
    char *name;
    Value value;
} Variable;

typedef struct {
    char *name;
    AstNode *declaration;
} Function;

typedef struct {
    Variable *variables;
    int count;
    int capacity;

    Function *functions;
    int function_count;
    int function_capacity;
} Environment;

void environment_init(Environment *environment);

void environment_free(Environment *environment);

int environment_define(
    Environment *environment,
    const char *name,
    Value value
);

Value *environment_get(
    Environment *environment,
    const char *name
);

int environment_assign(
    Environment *environment,
    const char *name,
    Value value
);

int environment_define_function(
    Environment *environment,
    AstNode *declaration
);

AstNode *environment_get_function(
    Environment *environment,
    const char *name
);

#endif
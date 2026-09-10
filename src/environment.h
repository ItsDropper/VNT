
#ifndef VNT_ENVIRONMENT_H
#define VNT_ENVIRONMENT_H

#include "value.h"

typedef struct {
    char *name;
    Value value;
} Variable;

typedef struct {
    Variable *variables;
    int count;
    int capacity;
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

#endif


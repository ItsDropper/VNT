
#include "environment.h"

#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *string) {
    char *copy = malloc(strlen(string) + 1);

    if (copy == NULL) {
        return NULL;
    }

    strcpy(copy, string);

    return copy;
}

void environment_init(Environment *environment) {
    environment->variables = NULL;
    environment->count = 0;
    environment->capacity = 0;
}

void environment_free(Environment *environment) {
    for (int i = 0; i < environment->count; i++) {
        free(environment->variables[i].name);
        value_free(&environment->variables[i].value);
    }

    free(environment->variables);

    environment->variables = NULL;
    environment->count = 0;
    environment->capacity = 0;
}

int environment_define(
    Environment *environment,
    const char *name,
    Value value
) {
    for (int i = 0; i < environment->count; i++) {
        if (strcmp(environment->variables[i].name, name) == 0) {
            value_free(&environment->variables[i].value);
            environment->variables[i].value = value;

            return 1;
        }
    }

    if (environment->count >= environment->capacity) {
        int new_capacity = environment->capacity == 0
            ? 8
            : environment->capacity * 2;

        Variable *new_variables = realloc(
            environment->variables,
            sizeof(Variable) * new_capacity
        );

        if (new_variables == NULL) {
            return 0;
        }

        environment->variables = new_variables;
        environment->capacity = new_capacity;
    }

    char *variable_name = copy_string(name);

    if (variable_name == NULL) {
        return 0;
    }

    environment->variables[environment->count].name = variable_name;
    environment->variables[environment->count].value = value;

    environment->count++;

    return 1;
}

Value *environment_get(
    Environment *environment,
    const char *name
) {
    for (int i = 0; i < environment->count; i++) {
        if (strcmp(environment->variables[i].name, name) == 0) {
            return &environment->variables[i].value;
        }
    }

    return NULL;
}


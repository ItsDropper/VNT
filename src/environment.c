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

    environment->functions = NULL;
    environment->function_count = 0;
    environment->function_capacity = 0;
}

void environment_free(Environment *environment) {
    for (int i = 0; i < environment->count; i++) {
        free(environment->variables[i].name);
        value_free(&environment->variables[i].value);
    }

    free(environment->variables);

    for (int i = 0; i < environment->function_count; i++) {
        free(environment->functions[i].name);
    }

    free(environment->functions);

    environment->variables = NULL;
    environment->count = 0;
    environment->capacity = 0;

    environment->functions = NULL;
    environment->function_count = 0;
    environment->function_capacity = 0;
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

int environment_define_function(
    Environment *environment,
    AstNode *declaration
) {
    const char *name =
        declaration->function_declaration.name;

    for (int i = 0; i < environment->function_count; i++) {
        if (strcmp(environment->functions[i].name, name) == 0) {
            environment->functions[i].declaration = declaration;
            return 1;
        }
    }

    if (
        environment->function_count >=
        environment->function_capacity
    ) {
        int new_capacity =
            environment->function_capacity == 0
                ? 8
                : environment->function_capacity * 2;

        Function *new_functions = realloc(
            environment->functions,
            sizeof(Function) * new_capacity
        );

        if (new_functions == NULL) {
            return 0;
        }

        environment->functions = new_functions;
        environment->function_capacity = new_capacity;
    }

    char *function_name = copy_string(name);

    if (function_name == NULL) {
        return 0;
    }

    environment->functions[
        environment->function_count
    ].name = function_name;

    environment->functions[
        environment->function_count
    ].declaration = declaration;

    environment->function_count++;

    return 1;
}

AstNode *environment_get_function(
    Environment *environment,
    const char *name
) {
    for (int i = 0; i < environment->function_count; i++) {
        if (
            strcmp(
                environment->functions[i].name,
                name
            ) == 0
        ) {
            return environment->functions[i].declaration;
        }
    }

    return NULL;
}
#include <vnt/compiler.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int vnt_emit_c_program(AstNode *program, const char *c_path);

int compiler_compile(AstNode *program, const char *assembly_path) {
    size_t n = strlen(assembly_path);
    char *c_path = malloc(n + 3);
    if (!c_path) {
        fprintf(stderr, "Native compiler error: out of memory.\n");
        return 0;
    }

    sprintf(c_path, "%s.c", assembly_path);

    if (!vnt_emit_c_program(program, c_path)) {
        remove(c_path);
        free(c_path);
        return 0;
    }

    size_t command_size = strlen(c_path) + strlen(assembly_path) + 128;
    char *command = malloc(command_size);
    if (!command) {
        fprintf(stderr, "Native compiler error: out of memory.\n");
        remove(c_path);
        free(c_path);
        return 0;
    }

    snprintf(
        command,
        command_size,
        "gcc -std=c11 -O2 -fwrapv -Isrc/compiler -S \"%s\" -o \"%s\"",
        c_path,
        assembly_path
    );

    int result = system(command);
    free(command);
    remove(c_path);
    free(c_path);

    if (result != 0) {
        fprintf(stderr, "Native compiler: GCC failed to generate assembly.\n");
        remove(assembly_path);
        return 0;
    }

    return 1;
}

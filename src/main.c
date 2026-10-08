#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vnt/lexer.h>
#include <vnt/parser.h>
#include <vnt/ast.h>
#include <vnt/compiler.h>
#include <vnt/modules.h>

static int compile_native(AstNode *program, const char *output_path) {
    size_t length = strlen(output_path);
    char *assembly_path = malloc(length + 3);
    if (!assembly_path) {
        fprintf(stderr, "Could not allocate memory.\n");
        return 1;
    }

    sprintf(assembly_path, "%s.s", output_path);

    if (!compiler_compile(program, assembly_path)) {
        free(assembly_path);
        return 1;
    }

    size_t command_size = strlen(assembly_path) + strlen(output_path) + 128;
    char *command = malloc(command_size);
    if (!command) {
        fprintf(stderr, "Could not allocate memory.\n");
        remove(assembly_path);
        free(assembly_path);
        return 1;
    }

    sprintf(command,
        "gcc \"%s\" src/compiler/native_runtime.c -o \"%s\"",
        assembly_path, output_path);

    int result = system(command);
    free(command);

    if (result != 0) {
        fprintf(stderr, "Native linking failed.\n");
        free(assembly_path);
        return 1;
    }

    printf("Native executable created: %s\n", output_path);
    printf("Assembly kept at: %s\n", assembly_path);

    free(assembly_path);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "--compile") != 0 ||
        strcmp(argv[3], "-o") != 0) {
        printf("Usage: vnt --compile <file.vnt> -o <output.exe>\n");
        return 1;
    }

    char *source = vnt_load_project_source(argv[2]);
    if (!source)
        return 1;

    Lexer lexer;
    lexer_init(&lexer, source);

    Parser parser;
    parser_init(&parser, &lexer);

    AstNode *program = parser_parse(&parser);
    if (!program) {
        free(source);
        return 1;
    }

    int result = compile_native(program, argv[4]);

    ast_free(program);
    free(source);
    return result;
}

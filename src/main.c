#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vnt/lexer.h>
#include <vnt/parser.h>
#include <vnt/ast.h>
#include <vnt/interpreter.h>
#include <vnt/environment.h>
#include <vnt/compiler.h>

static char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        printf("Could not open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);

    if (buffer == NULL) {
        fclose(file);
        printf("Could not allocate memory.\n");
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, size, file);
    buffer[bytes_read] = '\0';

    fclose(file);

    return buffer;
}

static int compile_native(
    AstNode *program,
    const char *output_path
) {
    size_t length = strlen(output_path);
    char *assembly_path = malloc(length + 3);

    if (assembly_path == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        return 1;
    }

    sprintf(assembly_path, "%s.s", output_path);

    if (!compiler_compile(program, assembly_path)) {
        free(assembly_path);
        return 1;
    }

    size_t command_size =
        strlen(assembly_path) +
        strlen(output_path) +
        32;

    char *command = malloc(command_size);

    if (command == NULL) {
        fprintf(stderr, "Could not allocate memory.\n");
        remove(assembly_path);
        free(assembly_path);
        return 1;
    }

    sprintf(
        command,
        "gcc \"%s\" -o \"%s\"",
        assembly_path,
        output_path
    );

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
    int native = 0;
    const char *source_path = NULL;
    const char *output_path = NULL;

    if (argc >= 2 && strcmp(argv[1], "--compile") == 0) {
        native = 1;

        if (
            argc != 5 ||
            strcmp(argv[3], "-o") != 0
        ) {
            printf(
                "Usage: vnt --compile <file.vnt> -o <output.exe>\n"
            );
            return 1;
        }

        source_path = argv[2];
        output_path = argv[4];
    } else {
        if (argc < 2) {
            printf("Usage: vnt <file.vnt>\n");
            return 1;
        }

        source_path = argv[1];
    }

    char *source = read_file(source_path);

    if (source == NULL) {
        return 1;
    }

    Lexer lexer;
    lexer_init(&lexer, source);

    Parser parser;
    parser_init(&parser, &lexer);

    AstNode *program = parser_parse(&parser);

    if (program == NULL) {
        free(source);
        return 1;
    }

    if (native) {
        int result = compile_native(program, output_path);

        ast_free(program);
        free(source);

        return result;
    }

    Environment environment;
    environment_init(&environment);

    interpreter_execute(program, &environment);

    int result = environment.had_error ? 1 : 0;

    environment_free(&environment);
    ast_free(program);
    free(source);

    return result;
}

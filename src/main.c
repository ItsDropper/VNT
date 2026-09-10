
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "interpreter.h"
#include "environment.h"

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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: vnt <file.vnt>\n");
        return 1;
    }

    char *source = read_file(argv[1]);

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

    Environment environment;
    environment_init(&environment);

    interpreter_execute(program, &environment);

    environment_free(&environment);
    ast_free(program);
    free(source);

    return 0;
}


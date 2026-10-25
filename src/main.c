#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vnt/lexer.h>
#include <vnt/parser.h>
#include <vnt/ast.h>
#include <vnt/compiler.h>
#include <vnt/modules.h>
#include <vnt/ir.h>
#include <vnt/ir_lower.h>

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

    size_t command_size = strlen(assembly_path) + strlen(output_path) + 512;
    char *command = malloc(command_size);
    if (!command) {
        fprintf(stderr, "Could not allocate memory.\n");
        remove(assembly_path);
        free(assembly_path);
        return 1;
    }

#ifdef _WIN32
    snprintf(command, command_size,
        "gcc \"%s\" src/compiler/native_runtime.c src/compiler/native_net.c src/compiler/native_async_net.c -O3 -lgdi32 -luser32 -lwinhttp -lws2_32 -o \"%s\"",
        assembly_path, output_path);
#else
    snprintf(command, command_size,
        "gcc \"%s\" src/compiler/native_runtime.c src/compiler/native_net.c src/compiler/native_async_net.c -O3 -pthread -lm -ldl -lssl -lcrypto -o \"%s\"",
        assembly_path, output_path);
#endif

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

static int build_freestanding(AstNode *program, const char *output_path) {
#ifdef _WIN32
    (void)program;
    (void)output_path;
    fprintf(stderr, "Freestanding ELF linking currently requires an ELF-targeting GNU ld environment; use Linux or WSL.\n");
    return 1;
#else
    size_t length = strlen(output_path);
    char *assembly_path = malloc(length + 3);
    char *object_path = malloc(length + 3);
    if (!assembly_path || !object_path) {
        free(assembly_path);
        free(object_path);
        fprintf(stderr, "Could not allocate memory.\n");
        return 1;
    }
    snprintf(assembly_path, length + 3, "%s.s", output_path);
    snprintf(object_path, length + 3, "%s.o", output_path);

    if (!compiler_compile_freestanding(program, assembly_path)) {
        free(assembly_path);
        free(object_path);
        return 1;
    }

    size_t command_size = strlen(assembly_path) + strlen(object_path) +
                          strlen(output_path) + 768;
    char *command = malloc(command_size);
    if (!command) {
        remove(assembly_path);
        free(assembly_path);
        free(object_path);
        fprintf(stderr, "Could not allocate memory.\n");
        return 1;
    }
    snprintf(command, command_size,
        "gcc -c -ffreestanding -fno-pic -fno-stack-protector -mno-red-zone \"%s\" -o \"%s\" && "
        "ld -nostdlib -static -T freestanding/x86_64.ld -e _start -o \"%s\" \"%s\"",
        assembly_path, object_path, output_path, object_path);
    int result = system(command);
    free(command);
    if (result != 0) {
        remove(output_path);
        free(assembly_path);
        free(object_path);
        fprintf(stderr, "Freestanding assembly/link failed; no hosted runtime was linked.\n");
        return 1;
    }
    printf("Freestanding ELF created: %s\n", output_path);
    free(assembly_path);
    free(object_path);
    return 0;
#endif
}

int main(int argc, char *argv[]) {
    int dump_ir = argc == 3 && strcmp(argv[1], "--dump-ir") == 0;
    int emit_asm = argc == 5 && strcmp(argv[1], "--emit-asm") == 0 &&
                   strcmp(argv[3], "-o") == 0;
    int emit_freestanding = argc == 5 &&
        strcmp(argv[1], "--emit-freestanding-asm") == 0 &&
        strcmp(argv[3], "-o") == 0;
    int build_free = argc == 5 && strcmp(argv[1], "--build-freestanding") == 0 &&
                     strcmp(argv[3], "-o") == 0;
    int compile = argc == 5 && strcmp(argv[1], "--compile") == 0 &&
                  strcmp(argv[3], "-o") == 0;
    if (!dump_ir && !emit_asm && !emit_freestanding && !build_free && !compile) {
        printf("Usage: vnt --compile <file.vnt> -o <output.exe>\n");
        printf("       vnt --emit-asm <file.vnt> -o <output.s>\n");
        printf("       vnt --emit-freestanding-asm <file.vnt> -o <kernel.s>\n");
        printf("       vnt --build-freestanding <file.vnt> -o <kernel.elf>\n");
        printf("       vnt --dump-ir <file.vnt>\n");
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

    int result;
    if (dump_ir) {
        VntIrProgram ir = {0};
        if (!vnt_ir_lower(&ir, program) ||
            !vnt_ir_optimize(&ir) ||
            !vnt_ir_validate(&ir)) {
            fprintf(stderr, "VNT HIR lowering/validation failed.\n");
            result = 1;
        } else {
            vnt_ir_dump(&ir, stdout);
            result = 0;
        }
        vnt_ir_free(&ir);
    } else if (emit_asm) {
        result = compiler_compile(program, argv[4]) ? 0 : 1;
        if (result == 0)
            printf("Assembly emitted without linking: %s\n", argv[4]);
    } else if (emit_freestanding) {
        result = compiler_compile_freestanding(program, argv[4]) ? 0 : 1;
        if (result == 0)
            printf("Freestanding x86-64 assembly emitted: %s\n", argv[4]);
    } else if (build_free) {
        result = build_freestanding(program, argv[4]);
    } else {
        result = compile_native(program, argv[4]);
    }
    ast_free(program);
    free(source);
    return result;
}

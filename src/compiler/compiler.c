#include <vnt/compiler.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int offset;
} NativeVariable;

typedef struct {
    FILE *out;
    NativeVariable *variables;
    int variable_count;
    int variable_capacity;
    int label_count;
    int string_count;
    int error;
} NativeCompiler;

static void fail(NativeCompiler *compiler, const char *message) {
    if (!compiler->error) {
        fprintf(stderr, "Native compiler error: %s\n", message);
    }

    compiler->error = 1;
}

static int variable_find(NativeCompiler *compiler, const char *name) {
    for (int i = 0; i < compiler->variable_count; i++) {
        if (strcmp(compiler->variables[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

static int variable_add(NativeCompiler *compiler, const char *name) {
    int existing = variable_find(compiler, name);

    if (existing >= 0) {
        return existing;
    }

    if (compiler->variable_count >= compiler->variable_capacity) {
        int capacity = compiler->variable_capacity == 0
            ? 16
            : compiler->variable_capacity * 2;

        NativeVariable *variables = realloc(
            compiler->variables,
            sizeof(NativeVariable) * capacity
        );

        if (variables == NULL) {
            fail(compiler, "out of memory.");
            return -1;
        }

        compiler->variables = variables;
        compiler->variable_capacity = capacity;
    }

    NativeVariable *variable =
        &compiler->variables[compiler->variable_count];

    variable->name = strdup(name);

    if (variable->name == NULL) {
        fail(compiler, "out of memory.");
        return -1;
    }

    variable->offset =
        8 + compiler->variable_count * 8;

    return compiler->variable_count++;
}

static void collect_variables(
    NativeCompiler *compiler,
    AstNode *node
) {
    for (; node != NULL; node = node->next) {
        switch (node->type) {
            case AST_VARIABLE_DECLARATION:
                variable_add(
                    compiler,
                    node->variable_declaration.name
                );
                collect_variables(
                    compiler,
                    node->variable_declaration.value
                );
                break;

            case AST_VARIABLE:
                variable_add(
                    compiler,
                    node->variable.name
                );
                break;

            case AST_ASSIGNMENT:
                collect_variables(
                    compiler,
                    node->assignment.target
                );
                collect_variables(
                    compiler,
                    node->assignment.value
                );
                break;

            case AST_PRINT_STATEMENT:
                collect_variables(
                    compiler,
                    node->print_statement.expression
                );
                break;

            case AST_IF_STATEMENT:
                collect_variables(
                    compiler,
                    node->if_statement.condition
                );
                collect_variables(
                    compiler,
                    node->if_statement.then_branch
                );
                collect_variables(
                    compiler,
                    node->if_statement.else_branch
                );
                break;

            case AST_WHILE_STATEMENT:
                collect_variables(
                    compiler,
                    node->while_statement.condition
                );
                collect_variables(
                    compiler,
                    node->while_statement.body
                );
                break;

            case AST_BINARY_EXPRESSION:
                collect_variables(
                    compiler,
                    node->binary_expression.left
                );
                collect_variables(
                    compiler,
                    node->binary_expression.right
                );
                break;

            case AST_UNARY_EXPRESSION:
                collect_variables(
                    compiler,
                    node->unary_expression.operand
                );
                break;

            case AST_INDEX_EXPRESSION:
                collect_variables(
                    compiler,
                    node->index_expression.array
                );
                collect_variables(
                    compiler,
                    node->index_expression.index
                );
                break;

            case AST_FUNCTION_DECLARATION:
                collect_variables(
                    compiler,
                    node->function_declaration.body
                );
                break;

            case AST_FUNCTION_CALL:
                collect_variables(
                    compiler,
                    node->function_call.arguments
                );
                break;

            case AST_RETURN_STATEMENT:
                collect_variables(
                    compiler,
                    node->return_statement.expression
                );
                break;

            case AST_ARRAY_LITERAL:
                collect_variables(
                    compiler,
                    node->array_literal.elements
                );
                break;

            default:
                break;
        }
    }
}

static int new_label(NativeCompiler *compiler) {
    return compiler->label_count++;
}

static void emit(
    NativeCompiler *compiler,
    const char *text
) {
    if (!compiler->error) {
        fputs(text, compiler->out);
    }
}

static void emit_int(
    NativeCompiler *compiler,
    const char *instruction,
    int value
) {
    if (!compiler->error) {
        fprintf(
            compiler->out,
            "    %s $%d, %%eax\n",
            instruction,
            value
        );
    }
}

static void compile_expression(
    NativeCompiler *compiler,
    AstNode *node
);

static void compile_statement(
    NativeCompiler *compiler,
    AstNode *node
);

static void compile_binary(
    NativeCompiler *compiler,
    AstNode *node
) {
    BinaryOperator op =
        node->binary_expression.operator;

    if (op == BINARY_AND || op == BINARY_OR) {
        fail(
            compiler,
            "logical operators are not native-compiled yet."
        );
        return;
    }

    compile_expression(
        compiler,
        node->binary_expression.left
    );

    emit(compiler, "    pushq %rax\n");

    compile_expression(
        compiler,
        node->binary_expression.right
    );

    emit(compiler, "    movl %eax, %ecx\n");
    emit(compiler, "    popq %rax\n");

    switch (op) {
        case BINARY_ADD:
            emit(compiler, "    addl %ecx, %eax\n");
            break;

        case BINARY_SUBTRACT:
            emit(compiler, "    subl %ecx, %eax\n");
            break;

        case BINARY_MULTIPLY:
            emit(compiler, "    imull %ecx, %eax\n");
            break;

        case BINARY_DIVIDE:
            emit(compiler, "    cdq\n");
            emit(compiler, "    idivl %ecx\n");
            break;

        case BINARY_MODULO:
            emit(compiler, "    cdq\n");
            emit(compiler, "    idivl %ecx\n");
            emit(compiler, "    movl %edx, %eax\n");
            break;

        case BINARY_EQUAL:
        case BINARY_NOT_EQUAL:
        case BINARY_GREATER:
        case BINARY_LESS:
        case BINARY_GREATER_EQUAL:
        case BINARY_LESS_EQUAL:
            emit(compiler, "    cmpl %ecx, %eax\n");

            switch (op) {
                case BINARY_EQUAL:
                    emit(compiler, "    sete %al\n");
                    break;
                case BINARY_NOT_EQUAL:
                    emit(compiler, "    setne %al\n");
                    break;
                case BINARY_GREATER:
                    emit(compiler, "    setg %al\n");
                    break;
                case BINARY_LESS:
                    emit(compiler, "    setl %al\n");
                    break;
                case BINARY_GREATER_EQUAL:
                    emit(compiler, "    setge %al\n");
                    break;
                case BINARY_LESS_EQUAL:
                    emit(compiler, "    setle %al\n");
                    break;
                default:
                    break;
            }

            emit(compiler, "    movzbl %al, %eax\n");
            break;

        default:
            fail(compiler, "unsupported binary operator.");
            break;
    }
}

static void compile_expression(
    NativeCompiler *compiler,
    AstNode *node
) {
    if (compiler->error || node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_INTEGER_LITERAL:
            emit_int(
                compiler,
                "movl",
                node->integer_literal.value
            );
            break;

        case AST_BOOLEAN_LITERAL:
            emit_int(
                compiler,
                "movl",
                node->boolean_literal.value
            );
            break;

        case AST_STRING_LITERAL:
            fail(
                compiler,
                "string expressions are not native-compiled yet."
            );
            break;

        case AST_VARIABLE: {
            int index = variable_find(
                compiler,
                node->variable.name
            );

            if (index < 0) {
                fail(compiler, "unknown variable.");
                break;
            }

            fprintf(
                compiler->out,
                "    movl -%d(%%rbp), %%eax\n",
                compiler->variables[index].offset
            );
            break;
        }

        case AST_BINARY_EXPRESSION:
            compile_binary(compiler, node);
            break;

        case AST_UNARY_EXPRESSION:
            compile_expression(
                compiler,
                node->unary_expression.operand
            );

            if (
                node->unary_expression.operator ==
                UNARY_NEGATE
            ) {
                emit(compiler, "    negl %eax\n");
            } else if (
                node->unary_expression.operator ==
                UNARY_NOT
            ) {
                emit(compiler, "    testl %eax, %eax\n");
                emit(compiler, "    sete %al\n");
                emit(compiler, "    movzbl %al, %eax\n");
            } else {
                fail(
                    compiler,
                    "unsupported unary operator."
                );
            }
            break;

        default:
            fail(
                compiler,
                "expression is not native-compiled yet."
            );
            break;
    }
}

static void emit_print_integer(
    NativeCompiler *compiler
) {
    emit(
        compiler,
        "    leaq vnt_fmt_int(%rip), %rcx\n"
        "    movl %eax, %edx\n"
        "    xorl %eax, %eax\n"
        "    call printf\n"
    );
}

static void emit_print_boolean(
    NativeCompiler *compiler
) {
    int false_label = new_label(compiler);
    int done_label = new_label(compiler);

    emit(compiler, "    testl %eax, %eax\n");

    fprintf(
        compiler->out,
        "    je .Lvnt_false_%d\n",
        false_label
    );

    emit(
        compiler,
        "    leaq vnt_true(%rip), %rcx\n"
        "    xorl %eax, %eax\n"
        "    call printf\n"
    );

    fprintf(
        compiler->out,
        "    jmp .Lvnt_done_%d\n",
        done_label
    );

    fprintf(
        compiler->out,
        ".Lvnt_false_%d:\n",
        false_label
    );

    emit(
        compiler,
        "    leaq vnt_false(%rip), %rcx\n"
        "    xorl %eax, %eax\n"
        "    call printf\n"
    );

    fprintf(
        compiler->out,
        ".Lvnt_done_%d:\n",
        done_label
    );
}

static void compile_statement(
    NativeCompiler *compiler,
    AstNode *node
) {
    if (compiler->error || node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_PRINT_STATEMENT:
            if (
                node->print_statement.expression->type ==
                AST_BOOLEAN_LITERAL
            ) {
                compile_expression(
                    compiler,
                    node->print_statement.expression
                );
                emit_print_boolean(compiler);
            } else {
                compile_expression(
                    compiler,
                    node->print_statement.expression
                );
                emit_print_integer(compiler);
            }
            break;

        case AST_VARIABLE_DECLARATION: {
            int index = variable_find(
                compiler,
                node->variable_declaration.name
            );

            compile_expression(
                compiler,
                node->variable_declaration.value
            );

            fprintf(
                compiler->out,
                "    movl %%eax, -%d(%%rbp)\n",
                compiler->variables[index].offset
            );
            break;
        }

        case AST_ASSIGNMENT: {
            if (
                node->assignment.target->type !=
                AST_VARIABLE
            ) {
                fail(
                    compiler,
                    "only variable assignment is native-compiled yet."
                );
                break;
            }

            int index = variable_find(
                compiler,
                node->assignment.target->variable.name
            );

            compile_expression(
                compiler,
                node->assignment.value
            );

            fprintf(
                compiler->out,
                "    movl %%eax, -%d(%%rbp)\n",
                compiler->variables[index].offset
            );
            break;
        }

        case AST_IF_STATEMENT: {
            int else_label = new_label(compiler);
            int done_label = new_label(compiler);

            compile_expression(
                compiler,
                node->if_statement.condition
            );

            fprintf(
                compiler->out,
                "    testl %%eax, %%eax\n"
                "    je .Lvnt_else_%d\n",
                else_label
            );

            compile_statement(
                compiler,
                node->if_statement.then_branch
            );

            fprintf(
                compiler->out,
                "    jmp .Lvnt_done_%d\n"
                ".Lvnt_else_%d:\n",
                done_label,
                else_label
            );

            compile_statement(
                compiler,
                node->if_statement.else_branch
            );

            fprintf(
                compiler->out,
                ".Lvnt_done_%d:\n",
                done_label
            );
            break;
        }

        case AST_WHILE_STATEMENT: {
            int start_label = new_label(compiler);
            int done_label = new_label(compiler);

            fprintf(
                compiler->out,
                ".Lvnt_while_%d:\n",
                start_label
            );

            compile_expression(
                compiler,
                node->while_statement.condition
            );

            fprintf(
                compiler->out,
                "    testl %%eax, %%eax\n"
                "    je .Lvnt_done_%d\n",
                done_label
            );

            compile_statement(
                compiler,
                node->while_statement.body
            );

            fprintf(
                compiler->out,
                "    jmp .Lvnt_while_%d\n"
                ".Lvnt_done_%d:\n",
                start_label,
                done_label
            );
            break;
        }

        case AST_PROGRAM:
            compile_statement(
                compiler,
                node->program.statements
            );
            break;

        case AST_BREAK_STATEMENT:
        case AST_CONTINUE_STATEMENT:
        case AST_FUNCTION_DECLARATION:
        case AST_FUNCTION_CALL:
        case AST_RETURN_STATEMENT:
        case AST_ARRAY_LITERAL:
        case AST_INDEX_EXPRESSION:
            fail(
                compiler,
                "this statement is not native-compiled yet."
            );
            break;

        default:
            fail(
                compiler,
                "unsupported statement."
            );
            break;
    }

    if (!compiler->error && node->next != NULL) {
        compile_statement(compiler, node->next);
    }
}

static void free_variables(NativeCompiler *compiler) {
    for (int i = 0; i < compiler->variable_count; i++) {
        free(compiler->variables[i].name);
    }

    free(compiler->variables);
}

int compiler_compile(
    AstNode *program,
    const char *assembly_path
) {
    NativeCompiler compiler = {0};

    collect_variables(&compiler, program);

    if (compiler.error) {
        free_variables(&compiler);
        return 0;
    }

    compiler.out = fopen(assembly_path, "wb");

    if (compiler.out == NULL) {
        free_variables(&compiler);
        fprintf(
            stderr,
            "Could not create native assembly file: %s\n",
            assembly_path
        );
        return 0;
    }

    int frame_size =
        32 + compiler.variable_count * 8;

    frame_size =
        (frame_size + 15) & ~15;

    fprintf(
        compiler.out,
        ".section .rdata\n"
        "vnt_fmt_int:\n"
        "    .asciz \"%%d\\n\"\n"
        "vnt_true:\n"
        "    .asciz \"true\\n\"\n"
        "vnt_false:\n"
        "    .asciz \"false\\n\"\n"
        ".text\n"
        ".globl main\n"
        ".extern printf\n"
        "main:\n"
        "    pushq %%rbp\n"
        "    movq %%rsp, %%rbp\n"
        "    subq $%d, %%rsp\n",
        frame_size
    );

    compile_statement(&compiler, program);

    if (!compiler.error) {
        emit(
            &compiler,
            "    xorl %eax, %eax\n"
            "    leave\n"
            "    ret\n"
        );
    }

    fclose(compiler.out);
    free_variables(&compiler);

    if (compiler.error) {
        remove(assembly_path);
        return 0;
    }

    return 1;
}

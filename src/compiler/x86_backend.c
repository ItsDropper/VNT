#include "x86_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *name;
    int offset;
} Var;

typedef struct {
    char *value;
    int label;
} StringLit;

typedef struct {
    FILE *out;
    Var *vars;
    int var_count;
    int var_capacity;
    StringLit *strings;
    int string_count;
    int string_capacity;
    int label_count;
    int loop_depth;
    int loop_start[64];
    int loop_end[64];
    int error;
    int temp_depth;
} X86Gen;

static void fail(X86Gen *g, const char *message) {
    if (!g->error)
        fprintf(stderr, "Native compiler error: %s\n", message);
    g->error = 1;
}

static void emit_label(X86Gen *g, int label) {
    fprintf(g->out, ".L%d:\n", label);
}

static int new_label(X86Gen *g) {
    return g->label_count++;
}

static void cname(FILE *out, const char *prefix, const char *name) {
    fputs(prefix, out);
    for (const unsigned char *p = (const unsigned char *)name; *p; ++p)
        fputc(isalnum(*p) || *p == '_' ? *p : '_', out);
}

static int var_find(X86Gen *g, const char *name) {
    for (int i = 0; i < g->var_count; ++i)
        if (!strcmp(g->vars[i].name, name))
            return i;
    return -1;
}

static void var_add(X86Gen *g, const char *name) {
    if (g->error || var_find(g, name) >= 0)
        return;

    if (g->var_count == g->var_capacity) {
        int capacity = g->var_capacity ? g->var_capacity * 2 : 16;
        Var *vars = realloc(g->vars, sizeof(Var) * capacity);
        if (!vars) {
            fail(g, "out of memory.");
            return;
        }
        g->vars = vars;
        g->var_capacity = capacity;
    }

    g->vars[g->var_count].name = malloc(strlen(name) + 1);
    if (!g->vars[g->var_count].name) {
        fail(g, "out of memory.");
        return;
    }

    strcpy(g->vars[g->var_count].name, name);
    g->vars[g->var_count].offset = 0;
    g->var_count++;
}

static void free_vars(X86Gen *g) {
    for (int i = 0; i < g->var_count; ++i)
        free(g->vars[i].name);
    free(g->vars);
    g->vars = NULL;
    g->var_count = 0;
    g->var_capacity = 0;
}

static int var_offset(X86Gen *g, const char *name) {
    int index = var_find(g, name);
    if (index < 0) {
        fail(g, "unknown variable.");
        return 0;
    }
    return g->vars[index].offset;
}

static void collect_vars(X86Gen *g, AstNode *node) {
    for (; node; node = node->next) {
        switch (node->type) {
            case AST_VARIABLE_DECLARATION:
                var_add(g, node->variable_declaration.name);
                break;

            case AST_VARIABLE:
                var_add(g, node->variable.name);
                break;

            case AST_ASSIGNMENT:
                if (node->assignment.target &&
                    node->assignment.target->type == AST_VARIABLE) {
                    var_add(g, node->assignment.target->variable.name);
                }
                break;

            case AST_IF_STATEMENT:
                collect_vars(g, node->if_statement.then_branch);
                collect_vars(g, node->if_statement.else_branch);
                break;

            case AST_WHILE_STATEMENT:
                collect_vars(g, node->while_statement.body);
                break;

            case AST_FUNCTION_DECLARATION:
                break;

            default:
                break;
        }
    }
}

static void assign_offsets(X86Gen *g) {
    for (int i = 0; i < g->var_count; ++i)
        g->vars[i].offset = 8 + i * 8;
}

static void emit_mem(X86Gen *g, int offset) {
    fprintf(g->out, "-%d(%%rbp)", offset);
}

static void emit_string_escaped(FILE *out, const char *s) {
    fputc('"', out);
    for (const unsigned char *p = (const unsigned char *)s; *p; ++p) {
        switch (*p) {
            case '\\': fputs("\\\\", out); break;
            case '"': fputs("\\\"", out); break;
            case '\n': fputs("\\n", out); break;
            case '\r': fputs("\\r", out); break;
            case '\t': fputs("\\t", out); break;
            case '\b': fputs("\\b", out); break;
            case '\f': fputs("\\f", out); break;
            default:
                if (*p < 32)
                    fprintf(out, "\\x%02x", *p);
                else
                    fputc(*p, out);
        }
    }
    fputc('"', out);
}

static int string_label(X86Gen *g, const char *value) {
    for (int i = 0; i < g->string_count; ++i)
        if (!strcmp(g->strings[i].value, value))
            return g->strings[i].label;

    if (g->string_count == g->string_capacity) {
        int capacity = g->string_capacity ? g->string_capacity * 2 : 8;
        StringLit *strings =
            realloc(g->strings, sizeof(StringLit) * capacity);
        if (!strings) {
            fail(g, "out of memory.");
            return 0;
        }
        g->strings = strings;
        g->string_capacity = capacity;
    }

    int label = new_label(g);
    char *copy = malloc(strlen(value) + 1);
    if (!copy) {
        fail(g, "out of memory.");
        return 0;
    }

    strcpy(copy, value);
    g->strings[g->string_count].value = copy;
    g->strings[g->string_count].label = label;
    g->string_count++;
    return label;
}

static void free_strings(X86Gen *g) {
    for (int i = 0; i < g->string_count; ++i)
        free(g->strings[i].value);
    free(g->strings);
    g->strings = NULL;
    g->string_count = 0;
    g->string_capacity = 0;
}

static void emit_expr(X86Gen *g, AstNode *node);

static void emit_call(X86Gen *g, AstNode *node) {
    int count = node->function_call.argument_count;

    if (count > 4) {
        fail(g, "native functions currently support at most 4 arguments.");
        return;
    }

    if (!strcmp(node->function_call.name, "mod")) {
        if (count != 2) {
            fail(g, "mod() expects 2 arguments.");
            return;
        }

        AstNode *a = node->function_call.arguments;
        AstNode *b = a ? a->next : NULL;

        emit_expr(g, a);
        fputs("    pushq %rax\n", g->out);
        emit_expr(g, b);
        fputs("    movl %eax, %ecx\n", g->out);
        fputs("    popq %rax\n", g->out);
        fputs("    cltd\n", g->out);
        fputs("    idivl %ecx\n", g->out);
        return;
    }

    int i = 0;
    for (AstNode *arg = node->function_call.arguments;
         arg;
         arg = arg->next, ++i) {
        emit_expr(g, arg);
        fputs("    pushq %rax\n", g->out);
    }

    static const char *regs[] = {"%rcx", "%rdx", "%r8", "%r9"};

    for (i = count - 1; i >= 0; --i) {
        fprintf(g->out, "    movl %d(%%rsp), %s\n",
                (count - 1 - i) * 8, regs[i]);
    }

    if (count & 1)
        fputs("    subq $8, %rsp\n", g->out);
    fputs("    subq $32, %rsp\n", g->out);

    fputs("    call vnt_fn_", g->out);
    for (const unsigned char *p =
             (const unsigned char *)node->function_call.name;
         *p; ++p) {
        fputc(isalnum(*p) || *p == '_' ? *p : '_', g->out);
    }
    fputc('\n', g->out);

    fputs("    addq $32, %rsp\n", g->out);
    if (count & 1)
        fputs("    addq $8, %rsp\n", g->out);

    for (i = 0; i < count; ++i)
        fputs("    popq %r10\n", g->out);
}

static void emit_expr(X86Gen *g, AstNode *node) {
    if (g->error)
        return;

    if (!node) {
        fail(g, "missing expression.");
        return;
    }

    switch (node->type) {
        case AST_INTEGER_LITERAL:
            fprintf(g->out, "    movl $%d, %%eax\n",
                    node->integer_literal.value);
            break;

        case AST_BOOLEAN_LITERAL:
            fprintf(g->out, "    movl $%d, %%eax\n",
                    node->boolean_literal.value ? 1 : 0);
            break;

        case AST_VARIABLE:
            fprintf(g->out, "    movl ");
            emit_mem(g, var_offset(g, node->variable.name));
            fputs(", %eax\n", g->out);
            break;

        case AST_UNARY_EXPRESSION:
            if (node->unary_expression.operator == UNARY_NEGATE) {
                emit_expr(g, node->unary_expression.operand);
                fputs("    negl %eax\n", g->out);
            } else {
                int done = new_label(g);
                emit_expr(g, node->unary_expression.operand);
                fputs("    cmpl $0, %eax\n", g->out);
                fputs("    sete %al\n", g->out);
                fputs("    movzbl %al, %eax\n", g->out);
                emit_label(g, done);
            }
            break;

        case AST_BINARY_EXPRESSION: {
            BinaryOperator op = node->binary_expression.operator;

            if (op == BINARY_AND || op == BINARY_OR) {
                int short_label = new_label(g);
                int done = new_label(g);

                emit_expr(g, node->binary_expression.left);
                fputs("    cmpl $0, %eax\n", g->out);

                if (op == BINARY_AND)
                    fprintf(g->out, "    je .L%d\n", short_label);
                else
                    fprintf(g->out, "    jne .L%d\n", short_label);

                emit_expr(g, node->binary_expression.right);
                fputs("    cmpl $0, %eax\n", g->out);
                fputs("    setne %al\n", g->out);
                fputs("    movzbl %al, %eax\n", g->out);
                fprintf(g->out, "    jmp .L%d\n", done);

                emit_label(g, short_label);
                if (op == BINARY_AND)
                    fputs("    xorl %eax, %eax\n", g->out);
                else
                    fputs("    movl $1, %eax\n", g->out);
                emit_label(g, done);
                break;
            }

            emit_expr(g, node->binary_expression.left);
            fputs("    pushq %rax\n", g->out);
            emit_expr(g, node->binary_expression.right);
            fputs("    movl %eax, %ecx\n", g->out);
            fputs("    popq %rax\n", g->out);

            switch (op) {
                case BINARY_ADD:
                    fputs("    addl %ecx, %eax\n", g->out);
                    break;
                case BINARY_SUBTRACT:
                    fputs("    subl %ecx, %eax\n", g->out);
                    break;
                case BINARY_MULTIPLY:
                    fputs("    imull %ecx, %eax\n", g->out);
                    break;
                case BINARY_DIVIDE:
                    fputs("    cltd\n", g->out);
                    fputs("    idivl %ecx\n", g->out);
                    break;
                case BINARY_MODULO:
                    fputs("    cltd\n", g->out);
                    fputs("    idivl %ecx\n", g->out);
                    fputs("    movl %edx, %eax\n", g->out);
                    break;
                case BINARY_EQUAL:
                    fputs("    cmpl %ecx, %eax\n", g->out);
                    fputs("    sete %al\n", g->out);
                    fputs("    movzbl %al, %eax\n", g->out);
                    break;
                case BINARY_NOT_EQUAL:
                    fputs("    cmpl %ecx, %eax\n", g->out);
                    fputs("    setne %al\n", g->out);
                    fputs("    movzbl %al, %eax\n", g->out);
                    break;
                case BINARY_GREATER:
                    fputs("    cmpl %ecx, %eax\n", g->out);
                    fputs("    setg %al\n", g->out);
                    fputs("    movzbl %al, %eax\n", g->out);
                    break;
                case BINARY_LESS:
                    fputs("    cmpl %ecx, %eax\n", g->out);
                    fputs("    setl %al\n", g->out);
                    fputs("    movzbl %al, %eax\n", g->out);
                    break;
                case BINARY_GREATER_EQUAL:
                    fputs("    cmpl %ecx, %eax\n", g->out);
                    fputs("    setge %al\n", g->out);
                    fputs("    movzbl %al, %eax\n", g->out);
                    break;
                case BINARY_LESS_EQUAL:
                    fputs("    cmpl %ecx, %eax\n", g->out);
                    fputs("    setle %al\n", g->out);
                    fputs("    movzbl %al, %eax\n", g->out);
                    break;
                default:
                    fail(g, "unsupported binary operator.");
                    break;
            }
            break;
        }

        case AST_FUNCTION_CALL:
            emit_call(g, node);
            break;

        case AST_STRING_LITERAL:
        case AST_ARRAY_LITERAL:
        case AST_INDEX_EXPRESSION:
            fail(g, "this native backend does not support dynamic values yet.");
            break;

        default:
            fail(g, "unsupported native expression.");
            break;
    }
}

static void emit_print(X86Gen *g, AstNode *expression) {
    if (!expression) {
        fail(g, "print() requires an expression.");
        return;
    }

    if (expression->type == AST_STRING_LITERAL) {
        int label = string_label(g, expression->string_literal.value);
        fputs("    lea .Lstr", g->out);
        fprintf(g->out, "%d(%%rip), %%rcx\n", label);
        fputs("    subq $32, %rsp\n", g->out);
        fputs("    call puts\n", g->out);
        fputs("    addq $32, %rsp\n", g->out);
        return;
    }

    emit_expr(g, expression);
    fputs("    movl %eax, %edx\n", g->out);
    fputs("    lea .Lvnt_int_fmt(%rip), %rcx\n", g->out);
    fputs("    subq $32, %rsp\n", g->out);
    fputs("    call printf\n", g->out);
    fputs("    addq $32, %rsp\n", g->out);
}

static void emit_assignment(X86Gen *g, AstNode *node) {
    AstNode *target = node->assignment.target;

    if (!target || target->type != AST_VARIABLE) {
        fail(g, "native assignment targets must be variables.");
        return;
    }

    emit_expr(g, node->assignment.value);
    fprintf(g->out, "    movl %%eax, ");
    emit_mem(g, var_offset(g, target->variable.name));
    fputc('\n', g->out);
}

static void emit_stmt_list(X86Gen *g, AstNode *node);

static void emit_stmt(X86Gen *g, AstNode *node) {
    if (g->error || !node)
        return;

    switch (node->type) {
        case AST_PRINT_STATEMENT:
            emit_print(g, node->print_statement.expression);
            break;

        case AST_VARIABLE_DECLARATION:
            emit_expr(g, node->variable_declaration.value);
            fprintf(g->out, "    movl %%eax, ");
            emit_mem(g, var_offset(g, node->variable_declaration.name));
            fputc('\n', g->out);
            break;

        case AST_ASSIGNMENT:
            emit_assignment(g, node);
            break;

        case AST_IF_STATEMENT: {
            int else_label = new_label(g);
            int done = new_label(g);

            emit_expr(g, node->if_statement.condition);
            fputs("    cmpl $0, %eax\n", g->out);
            fprintf(g->out, "    je .L%d\n", else_label);

            emit_stmt_list(g, node->if_statement.then_branch);
            fprintf(g->out, "    jmp .L%d\n", done);

            emit_label(g, else_label);
            emit_stmt_list(g, node->if_statement.else_branch);
            emit_label(g, done);
            break;
        }

        case AST_WHILE_STATEMENT: {
            int start = new_label(g);
            int end = new_label(g);

            if (g->loop_depth >= 64) {
                fail(g, "loop nesting is too deep.");
                return;
            }

            g->loop_start[g->loop_depth] = start;
            g->loop_end[g->loop_depth] = end;
            g->loop_depth++;

            emit_label(g, start);
            emit_expr(g, node->while_statement.condition);
            fputs("    cmpl $0, %eax\n", g->out);
            fprintf(g->out, "    je .L%d\n", end);
            emit_stmt_list(g, node->while_statement.body);
            fprintf(g->out, "    jmp .L%d\n", start);
            emit_label(g, end);

            g->loop_depth--;
            break;
        }

        case AST_BREAK_STATEMENT:
            if (g->loop_depth == 0) {
                fail(g, "break outside a loop.");
                return;
            }
            fprintf(g->out, "    jmp .L%d\n",
                    g->loop_end[g->loop_depth - 1]);
            break;

        case AST_CONTINUE_STATEMENT:
            if (g->loop_depth == 0) {
                fail(g, "continue outside a loop.");
                return;
            }
            fprintf(g->out, "    jmp .L%d\n",
                    g->loop_start[g->loop_depth - 1]);
            break;

        case AST_RETURN_STATEMENT:
            if (node->return_statement.expression)
                emit_expr(g, node->return_statement.expression);
            else
                fputs("    xorl %eax, %eax\n", g->out);
            fputs("    leave\n", g->out);
            fputs("    ret\n", g->out);
            break;

        case AST_FUNCTION_CALL:
            emit_expr(g, node);
            break;

        case AST_FUNCTION_DECLARATION:
            break;

        default:
            fail(g, "unsupported native statement.");
            break;
    }
}

static void emit_stmt_list(X86Gen *g, AstNode *node) {
    for (; node && !g->error; node = node->next)
        emit_stmt(g, node);
}

static void emit_function(X86Gen *g, AstNode *function) {
    int count = function->function_declaration.parameter_count;

    if (count > 4) {
        fail(g, "native functions currently support at most 4 parameters.");
        return;
    }

    free_vars(g);
    collect_vars(g, function->function_declaration.body);

    for (int i = 0; i < count; ++i)
        var_add(g, function->function_declaration.parameters[i]);

    assign_offsets(g);

    fprintf(g->out, ".globl vnt_fn_");
    cname(g->out, "", function->function_declaration.name);
    fputc('\n', g->out);

    fputs("vnt_fn_", g->out);
    cname(g->out, "", function->function_declaration.name);
    fputs(":\n", g->out);

    fputs("    pushq %rbp\n", g->out);
    fputs("    movq %rsp, %rbp\n", g->out);

    int frame = ((g->var_count * 8 + 15) / 16) * 16;
    if (frame)
        fprintf(g->out, "    subq $%d, %%rsp\n", frame);

    static const char *regs[] = {"%ecx", "%edx", "%r8d", "%r9d"};

    for (int i = 0; i < count; ++i) {
        fprintf(g->out, "    movl %s, ", regs[i]);
        emit_mem(g, var_offset(g, function->function_declaration.parameters[i]));
        fputc('\n', g->out);
    }

    emit_stmt_list(g, function->function_declaration.body);

    if (!g->error) {
        fputs("    xorl %eax, %eax\n", g->out);
        fputs("    leave\n", g->out);
        fputs("    ret\n", g->out);
    }
}

int vnt_emit_x86_64(AstNode *program, const char *assembly_path) {
    if (!program || program->type != AST_PROGRAM) {
        fprintf(stderr, "Native compiler error: invalid program AST.\n");
        return 0;
    }

    X86Gen g = {0};
    g.out = fopen(assembly_path, "wb");

    if (!g.out) {
        fprintf(stderr, "Could not create assembly file: %s\n", assembly_path);
        return 0;
    }

    fputs(".section .rdata,\"dr\"\n", g.out);
    fputs(".Lvnt_int_fmt:\n", g.out);
    fputs("    .asciz \"%d\\n\"\n\n", g.out);

    fputs(".text\n", g.out);

    for (AstNode *node = program->program.statements;
         node;
         node = node->next) {
        if (node->type == AST_FUNCTION_DECLARATION) {
            emit_function(g.out ? &g : &g, node);
            if (g.error)
                break;
            fputc('\n', g.out);
        }
    }

    if (!g.error) {
        free_vars(&g);
        collect_vars(&g, program->program.statements);
        assign_offsets(&g);

        fputs(".globl main\nmain:\n", g.out);
        fputs("    pushq %rbp\n", g.out);
        fputs("    movq %rsp, %rbp\n", g.out);

        int frame = ((g.var_count * 8 + 15) / 16) * 16;
        if (frame)
            fprintf(g.out, "    subq $%d, %%rsp\n", frame);

        emit_stmt_list(&g, program->program.statements);

        if (!g.error) {
            fputs("    xorl %eax, %eax\n", g.out);
            fputs("    leave\n", g.out);
            fputs("    ret\n", g.out);
        }
    }

    if (!g.error && g.string_count) {
        fputs("\n.section .rdata,\"dr\"\n", g.out);
        for (int i = 0; i < g.string_count; ++i) {
            fprintf(g.out, ".Lstr%d:\n    .asciz ",
                    g.strings[i].label);
            emit_string_escaped(g.out, g.strings[i].value);
            fputc('\n', g.out);
        }
    }

    fclose(g.out);
    free_vars(&g);
    free_strings(&g);

    if (g.error) {
        remove(assembly_path);
        return 0;
    }

    return 1;
}

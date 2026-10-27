#include "native_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void fail(CGen *g, const char *message) {
    if (!g->error)
        fprintf(stderr, "Native compiler error: %s\n", message);
    g->error = 1;
}

static void cname(FILE *out, const char *prefix, const char *name) {
    fputs(prefix, out);
    for (const unsigned char *p = (const unsigned char *)name; *p; ++p)
        fputc(isalnum(*p) || *p == '_' ? *p : '_', out);
}

static void emit_string(FILE *out, const char *s) {
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
                if (*p < 32) fprintf(out, "\\x%02x", *p);
                else fputc(*p, out);
        }
    }
    fputc('"', out);
}

static int var_find(CGen *g, const char *name) {
    for (int i = 0; i < g->var_count; ++i)
        if (!strcmp(g->vars[i], name))
            return i;
    return -1;
}

static void var_add(CGen *g, const char *name) {
    if (g->error || var_find(g, name) >= 0) return;

    if (g->var_count == g->var_capacity) {
        int capacity = g->var_capacity ? g->var_capacity * 2 : 16;
        char **vars = realloc(
            g->vars,
            sizeof(char *) * capacity
        );

        if (!vars) {
            fail(g, "out of memory.");
            return;
        }

        g->vars = vars;
        g->var_capacity = capacity;
    }

    g->vars[g->var_count] = malloc(strlen(name) + 1);

    if (!g->vars[g->var_count]) {
        fail(g, "out of memory.");
        return;
    }

    strcpy(g->vars[g->var_count], name);
    g->var_count++;
}

static void free_vars(CGen *g) {
    for (int i = 0; i < g->var_count; ++i)
        free(g->vars[i]);

    free(g->vars);
    g->vars = NULL;
    g->var_count = 0;
    g->var_capacity = 0;
}

static void collect_vars(CGen *g, AstNode *node) {
    for (; node; node = node->next) {
        switch (node->type) {
            case AST_VARIABLE_DECLARATION:
                var_add(g, node->variable_declaration.name);
                collect_vars(g, node->variable_declaration.value);
                break;

            case AST_VARIABLE:
                var_add(g, node->variable.name);
                break;

            case AST_ASSIGNMENT:
                collect_vars(g, node->assignment.target);
                collect_vars(g, node->assignment.value);
                break;

            case AST_PRINT_STATEMENT:
                collect_vars(g, node->print_statement.expression);
                break;

            case AST_IF_STATEMENT:
                collect_vars(g, node->if_statement.condition);
                collect_vars(g, node->if_statement.then_branch);
                collect_vars(g, node->if_statement.else_branch);
                break;

            case AST_WHILE_STATEMENT:
                collect_vars(g, node->while_statement.condition);
                collect_vars(g, node->while_statement.body);
                break;

            case AST_FUNCTION_CALL:
                collect_vars(g, node->function_call.arguments);
                break;

            case AST_RETURN_STATEMENT:
                collect_vars(g, node->return_statement.expression);
                break;

            case AST_BINARY_EXPRESSION:
                collect_vars(g, node->binary_expression.left);
                collect_vars(g, node->binary_expression.right);
                break;

            case AST_UNARY_EXPRESSION:
                collect_vars(g, node->unary_expression.operand);
                break;

            case AST_INDEX_EXPRESSION:
                collect_vars(g, node->index_expression.array);
                collect_vars(g, node->index_expression.index);
                break;

            case AST_ARRAY_LITERAL:
                collect_vars(g, node->array_literal.elements);
                break;

            default:
                break;
        }
    }
}

static void emit_expr(CGen *g, AstNode *node);
static void emit_stmt(CGen *g, AstNode *node);

static void emit_function_name(FILE *out, const char *name) {
    cname(out, "vnt_fn_", name);
}

static void emit_args(CGen *g, AstNode *args) {
    int first = 1;

    for (AstNode *arg = args; arg; arg = arg->next) {
        if (!first)
            fputs(", ", g->out);

        emit_expr(g, arg);
        first = 0;
    }
}

static void emit_expr(CGen *g, AstNode *node) {
    if (g->error) return;

    if (node == NULL) {
        fputs("vnt_invalid()", g->out);
        return;
    }

    switch (node->type) {
        case AST_INTEGER_LITERAL:
            fprintf(
                g->out,
                "vnt_int(%d)",
                node->integer_literal.value
            );
            break;

        case AST_BOOLEAN_LITERAL:
            fprintf(
                g->out,
                "vnt_bool(%d)",
                node->boolean_literal.value
            );
            break;

        case AST_STRING_LITERAL:
            fputs("vnt_string(", g->out);
            emit_string(
                g->out,
                node->string_literal.value
            );
            fputc(')', g->out);
            break;

        case AST_VARIABLE:
            cname(g->out, "v_", node->variable.name);
            break;

        case AST_ARRAY_LITERAL: {
            fprintf(
                g->out,
                "vnt_array_from((VntValue[]){"
            );

            int first = 1;

            for (
                AstNode *element =
                    node->array_literal.elements;
                element;
                element = element->next
            ) {
                if (!first)
                    fputs(", ", g->out);

                emit_expr(g, element);
                first = 0;
            }

            fprintf(
                g->out,
                "}, %d)",
                node->array_literal.element_count
            );
            break;
        }

        case AST_INDEX_EXPRESSION:
            fputs("vnt_get(", g->out);
            emit_expr(
                g,
                node->index_expression.array
            );
            fputs(", ", g->out);
            emit_expr(
                g,
                node->index_expression.index
            );
            fputc(')', g->out);
            break;

        case AST_UNARY_EXPRESSION:
            if (
                node->unary_expression.operator ==
                UNARY_NOT
            ) {
                fputs("vnt_bool(!vnt_truth(", g->out);
                emit_expr(
                    g,
                    node->unary_expression.operand
                );
                fputs("))", g->out);
            } else {
                fputs("vnt_neg(", g->out);
                emit_expr(
                    g,
                    node->unary_expression.operand
                );
                fputc(')', g->out);
            }
            break;

        case AST_BINARY_EXPRESSION: {
            BinaryOperator op =
                node->binary_expression.operator;

            if (
                op == BINARY_AND ||
                op == BINARY_OR
            ) {
                fputs("vnt_bool(", g->out);
                fputs("vnt_truth(", g->out);

                emit_expr(
                    g,
                    node->binary_expression.left
                );

                fputs(")", g->out);

                fputs(
                    op == BINARY_AND
                        ? " && "
                        : " || ",
                    g->out
                );

                fputs("vnt_truth(", g->out);

                emit_expr(
                    g,
                    node->binary_expression.right
                );

                fputs("))", g->out);
                break;
            }

            const char *function = NULL;

            switch (op) {
                case BINARY_ADD: function = "vnt_add"; break;
                case BINARY_SUBTRACT: function = "vnt_sub"; break;
                case BINARY_MULTIPLY: function = "vnt_mul"; break;
                case BINARY_DIVIDE: function = "vnt_div"; break;
                case BINARY_MODULO: function = "vnt_mod_value"; break;
                case BINARY_EQUAL: function = "vnt_eq"; break;
                case BINARY_NOT_EQUAL: function = "vnt_ne"; break;
                case BINARY_GREATER: function = "vnt_gt"; break;
                case BINARY_LESS: function = "vnt_lt"; break;
                case BINARY_GREATER_EQUAL: function = "vnt_ge"; break;
                case BINARY_LESS_EQUAL: function = "vnt_le"; break;
                default:
                    fail(g, "unsupported binary operator.");
                    return;
            }

            fprintf(g->out, "%s(", function);

            emit_expr(
                g,
                node->binary_expression.left
            );

            fputs(", ", g->out);

            emit_expr(
                g,
                node->binary_expression.right
            );

            fputc(')', g->out);
            break;
        }

        case AST_FUNCTION_CALL: {
            const char *name =
                node->function_call.name;

            int count =
                node->function_call.argument_count;

            if (!strcmp(name, "len")) {
                if (count != 1) {
                    fail(g, "len() expects 1 argument.");
                    return;
                }

                fputs("vnt_len(", g->out);
                emit_args(g, node->function_call.arguments);
                fputc(')', g->out);
                break;
            }

            if (!strcmp(name, "mod")) {
                if (count != 2) {
                    fail(g, "mod() expects 2 arguments.");
                    return;
                }

                fputs("vnt_mod(", g->out);
                emit_args(g, node->function_call.arguments);
                fputc(')', g->out);
                break;
            }

            if (!strcmp(name, "input")) {
                if (count != 1) {
                    fail(g, "input() expects 1 argument.");
                    return;
                }

                fputs("vnt_input(", g->out);
                emit_args(g, node->function_call.arguments);
                fputc(')', g->out);
                break;
            }

            if (!strcmp(name, "range")) {
                if (count < 1 || count > 3) {
                    fail(
                        g,
                        "range() expects 1, 2, or 3 arguments."
                    );
                    return;
                }

                fprintf(
                    g->out,
                    "vnt_range%d(",
                    count
                );

                emit_args(g, node->function_call.arguments);
                fputc(')', g->out);
                break;
            }

            emit_function_name(
                g->out,
                name
            );

            fputc('(', g->out);
            emit_args(
                g,
                node->function_call.arguments
            );
            fputc(')', g->out);
            break;
        }

        default:
            fail(g, "unsupported expression.");
            break;
    }
}

static void emit_assignment(CGen *g, AstNode *node) {
    AstNode *target =
        node->assignment.target;

    if (target->type == AST_VARIABLE) {
        cname(
            g->out,
            "v_",
            target->variable.name
        );

        fputs(" = ", g->out);

        emit_expr(
            g,
            node->assignment.value
        );

        fputc(';', g->out);
        return;
    }

    if (target->type == AST_INDEX_EXPRESSION) {
        fputs("vnt_set(&", g->out);

        emit_expr(
            g,
            target->index_expression.array
        );

        fputs(", ", g->out);

        emit_expr(
            g,
            target->index_expression.index
        );

        fputs(", ", g->out);

        emit_expr(
            g,
            node->assignment.value
        );

        fputs(");", g->out);
        return;
    }

    fail(
        g,
        "invalid assignment target."
    );
}

static void emit_stmt(CGen *g, AstNode *node) {
    if (g->error || !node)
        return;

    switch (node->type) {
        case AST_PRINT_STATEMENT:
            fputs("vnt_print(", g->out);
            emit_expr(
                g,
                node->print_statement.expression
            );
            fputs(");", g->out);
            break;

        case AST_VARIABLE_DECLARATION:
            cname(
                g->out,
                "v_",
                node->variable_declaration.name
            );

            fputs(" = ", g->out);

            emit_expr(
                g,
                node->variable_declaration.value
            );

            fputc(';', g->out);
            break;

        case AST_ASSIGNMENT:
            emit_assignment(g, node);
            break;

        case AST_IF_STATEMENT: {
            int else_label = g->label_count++;
            int done_label = g->label_count++;

            fputs("if (vnt_truth(", g->out);

            emit_expr(
                g,
                node->if_statement.condition
            );

            fprintf(
                g->out,
                ")) {\n"
            );

            emit_stmt(
                g,
                node->if_statement.then_branch
            );

            fprintf(
                g->out,
                "\n} else {\n"
            );

            emit_stmt(
                g,
                node->if_statement.else_branch
            );

            fprintf(
                g->out,
                "\n}\n"
            );

            (void)else_label;
            (void)done_label;
            break;
        }

        case AST_WHILE_STATEMENT:
            fputs("while (vnt_truth(", g->out);

            emit_expr(
                g,
                node->while_statement.condition
            );

            fputs(")) {\n", g->out);

            emit_stmt(
                g,
                node->while_statement.body
            );

            fputs("\n}", g->out);
            break;

        case AST_BREAK_STATEMENT:
            fputs("break;", g->out);
            break;

        case AST_CONTINUE_STATEMENT:
            fputs("continue;", g->out);
            break;

        case AST_RETURN_STATEMENT:
            fputs("return ", g->out);

            emit_expr(
                g,
                node->return_statement.expression
            );

            fputc(';', g->out);
            break;

        case AST_FUNCTION_CALL:
            emit_expr(g, node);
            fputc(';', g->out);
            break;

        case AST_FUNCTION_DECLARATION:
            break;

        default:
            break;
    }

    if (!g->error && node->next) {
        fputc('\n', g->out);
        emit_stmt(g, node->next);
    }
}

static int has_parameter(
    AstNode *function,
    const char *name
) {
    for (
        int i = 0;
        i < function->function_declaration.parameter_count;
        ++i
    ) {
        if (
            !strcmp(
                function->function_declaration.parameters[i],
                name
            )
        ) {
            return 1;
        }
    }

    return 0;
}

static void emit_function_prototype(
    FILE *out,
    AstNode *function
) {
    fputs("static VntValue ", out);

    emit_function_name(
        out,
        function->function_declaration.name
    );

    fputc('(', out);

    int count =
        function->function_declaration.parameter_count;

    if (count == 0) {
        fputs("void", out);
    }

    for (int i = 0; i < count; ++i) {
        if (i) fputs(", ", out);

        fputs("VntValue ", out);

        cname(
            out,
            "v_",
            function->function_declaration.parameters[i]
        );
    }

    fputs(");\n", out);
}

static void emit_function(
    CGen *g,
    AstNode *function
) {
    fputs("static VntValue ", g->out);

    emit_function_name(
        g->out,
        function->function_declaration.name
    );

    fputc('(', g->out);

    int count =
        function->function_declaration.parameter_count;

    if (count == 0) {
        fputs("void", g->out);
    }

    for (int i = 0; i < count; ++i) {
        if (i) fputs(", ", g->out);

        fputs("VntValue ", g->out);

        cname(
            g->out,
            "v_",
            function->function_declaration.parameters[i]
        );
    }

    fputs(") {\n", g->out);

    collect_vars(
        g,
        function->function_declaration.body
    );

    for (int i = 0; i < g->var_count; ++i) {
        if (
            has_parameter(
                function,
                g->vars[i]
            )
        ) {
            continue;
        }

        fputs("    VntValue ", g->out);

        cname(
            g->out,
            "v_",
            g->vars[i]
        );

        fputs(" = vnt_invalid();\n", g->out);
    }

    free_vars(g);

    fputs("    ", g->out);

    emit_stmt(
        g,
        function->function_declaration.body
    );

    fputs("\n    return vnt_invalid();\n", g->out);
    fputs("}\n\n", g->out);
}

int vnt_emit_c_program(
    AstNode *program,
    const char *c_path
) {
    if (
        program == NULL ||
        program->type != AST_PROGRAM
    ) {
        fprintf(
            stderr,
            "Native compiler error: invalid program AST.\n"
        );
        return 0;
    }

    CGen g = {0};

    g.out = fopen(c_path, "wb");

    if (!g.out) {
        fprintf(
            stderr,
            "Could not create native C file: %s\n",
            c_path
        );
        return 0;
    }

    fputs(
        "#include \"native_runtime.h\"\n\n",
        g.out
    );

    for (
        AstNode *node = program->program.statements;
        node;
        node = node->next
    ) {
        if (
            node->type ==
            AST_FUNCTION_DECLARATION
        ) {
            emit_function_prototype(
                g.out,
                node
            );
        }
    }

    fputc('\n', g.out);

    for (
        AstNode *node = program->program.statements;
        node;
        node = node->next
    ) {
        if (
            node->type ==
            AST_FUNCTION_DECLARATION
        ) {
            emit_function(&g, node);
        }
    }

    fputs(
        "int main(void) {\n",
        g.out
    );

    collect_vars(
        &g,
        program->program.statements
    );

    for (int i = 0; i < g.var_count; ++i) {
        fputs(
            "    VntValue ",
            g.out
        );

        cname(
            g.out,
            "v_",
            g.vars[i]
        );

        fputs(
            " = vnt_invalid();\n",
            g.out
        );
    }

    free_vars(&g);

    fputs("    ", g.out);

    emit_stmt(
        &g,
        program->program.statements
    );

    fputc('\n', g.out);

    fputs(
        "    return 0;\n"
        "}\n",
        g.out
    );

    fclose(g.out);

    int success = !g.error;

    free_vars(&g);

    return success;
}

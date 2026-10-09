#include <vnt/typecheck.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TY_UNDECLARED, TY_UNKNOWN, TY_INT, TY_FLOAT, TY_BOOL, TY_STRING,
    TY_ARRAY, TY_OBJECT, TY_REFERENCE
} TypeKind;

typedef struct { char *name; TypeKind type; int explicit_type; int dynamic; } Symbol;
typedef struct { char *name; int arity; } FunctionDef;
typedef struct { char *name; int field_count; } StructDef;

typedef struct {
    Symbol *symbols;
    int count, capacity;
    FunctionDef *functions;
    int function_count, function_capacity;
    StructDef *structs;
    int struct_count, struct_capacity;
    int error;
} TypeChecker;

static void error(TypeChecker *tc, const char *message) {
    if (!tc->error) fprintf(stderr, "Type error: %s\n", message);
    tc->error = 1;
}

static TypeKind find_symbol(TypeChecker *tc, const char *name) {
    for (int i = tc->count - 1; i >= 0; --i)
        if (!strcmp(tc->symbols[i].name, name))
            return tc->symbols[i].type;
    return TY_UNDECLARED;
}

static void set_symbol(TypeChecker *tc, const char *name, TypeKind type,
                       int explicit_type) {
    for (int i = tc->count - 1; i >= 0; --i) {
        if (!strcmp(tc->symbols[i].name, name)) {
            Symbol *symbol = &tc->symbols[i];
            TypeKind old = symbol->type;
            if (old != TY_UNKNOWN && type != TY_UNKNOWN &&
                old != type && !(old == TY_INT && type == TY_FLOAT) &&
                !(old == TY_FLOAT && type == TY_INT)) {
                if (symbol->explicit_type || explicit_type) {
                    error(tc, "variable type changed incompatibly.");
                    return;
                }
                /* Inferred variables are dynamically typed. Once assignments
                   disagree, keep the symbol unknown instead of narrowing it
                   again on a later assignment. The backend will box it. */
                symbol->type = TY_UNKNOWN;
                symbol->dynamic = 1;
                return;
            }
            if (old == TY_UNKNOWN && !symbol->dynamic)
                symbol->type = type;
            if (explicit_type) symbol->explicit_type = 1;
            return;
        }
    }

    if (tc->count == tc->capacity) {
        int cap = tc->capacity ? tc->capacity * 2 : 32;
        Symbol *symbols = realloc(tc->symbols, sizeof(*symbols) * cap);
        if (!symbols) { error(tc, "out of memory."); return; }
        tc->symbols = symbols;
        tc->capacity = cap;
    }

    tc->symbols[tc->count].name = strdup(name);
    if (!tc->symbols[tc->count].name) { error(tc, "out of memory."); return; }
    tc->symbols[tc->count].type = type;
    tc->symbols[tc->count].explicit_type = explicit_type;
    tc->symbols[tc->count].dynamic = 0;
    tc->count++;
}

static int numeric(TypeKind t) { return t == TY_INT || t == TY_FLOAT; }

static TypeKind type_from_name(const char *name) {
    if (!strcmp(name, "int")) return TY_INT;
    if (!strcmp(name, "float")) return TY_FLOAT;
    if (!strcmp(name, "bool")) return TY_BOOL;
    if (!strcmp(name, "string")) return TY_STRING;
    if (!strcmp(name, "array")) return TY_ARRAY;
    if (!strcmp(name, "object")) return TY_OBJECT;
    if (!strcmp(name, "reference")) return TY_REFERENCE;
    return TY_UNDECLARED;
}

static FunctionDef *find_function(TypeChecker *tc, const char *name) {
    for (int i = 0; i < tc->function_count; ++i)
        if (!strcmp(tc->functions[i].name, name))
            return &tc->functions[i];
    return NULL;
}

static StructDef *find_struct(TypeChecker *tc, const char *name) {
    for (int i = 0; i < tc->struct_count; ++i)
        if (!strcmp(tc->structs[i].name, name))
            return &tc->structs[i];
    return NULL;
}

static int builtin_arity(const char *name, int argc) {
    if (!strcmp(name, "range")) return argc >= 1 && argc <= 3;
    if (!strcmp(name, "sqrt") || !strcmp(name, "sin") ||
        !strcmp(name, "cos") || !strcmp(name, "tan") ||
        !strcmp(name, "abs") || !strcmp(name, "floor") ||
        !strcmp(name, "ceil") || !strcmp(name, "len") ||
        !strcmp(name, "input")) return argc == 1;
    if (!strcmp(name, "min") || !strcmp(name, "max") ||
        !strcmp(name, "mod")) return argc == 2;
    if (!strcmp(name, "object")) return argc == 0;
    if (!strcmp(name, "ffi_int")) return argc >= 2 && argc <= 8;
    if (!strcmp(name, "cwd") || !strcmp(name, "time_ms")) return argc == 0;
    if (!strcmp(name, "fs_exists") || !strcmp(name, "fs_read") ||
        !strcmp(name, "fs_delete") || !strcmp(name, "dir_create") || !strcmp(name, "env_get") ||
        !strcmp(name, "sleep_ms") || !strcmp(name, "process_poll") ||
        !strcmp(name, "process_pid") || !strcmp(name, "process_terminate") ||
        !strcmp(name, "process_stdout") || !strcmp(name, "process_stderr") ||
        !strcmp(name, "process_exit_code")) return argc == 1;
    if (!strcmp(name, "process_wait") || !strcmp(name, "process_start")) return argc == 2;
    if (!strcmp(name, "fs_write") || !strcmp(name, "fs_append") ||
        !strcmp(name, "env_set")) return argc == 2;
    return -1;
}

static TypeKind expr_type(TypeChecker *tc, AstNode *n);

static void check_call(TypeChecker *tc, AstNode *n) {
    int argc = n->function_call.argument_count;
    for (AstNode *a = n->function_call.arguments; a; a = a->next)
        (void)expr_type(tc, a);

    int builtin = builtin_arity(n->function_call.name, argc);
    if (builtin != -1) {
        if (!builtin)
            error(tc, "invalid argument count for builtin function.");
        return;
    }

    /* Struct constructors are callable expressions, but are not functions. */
    if (find_struct(tc, n->function_call.name)) return;

    FunctionDef *fn = find_function(tc, n->function_call.name);
    if (!fn) {
        char message[256];
        snprintf(message, sizeof(message), "call to undeclared function '%s'.", n->function_call.name);
        error(tc, message);
        return;
    }
    if (fn->arity != argc)
        error(tc, "function called with the wrong number of arguments.");
}

static TypeKind expr_type(TypeChecker *tc, AstNode *n) {
    if (!n) return TY_UNKNOWN;

    switch (n->type) {
        case AST_INTEGER_LITERAL: return TY_INT;
        case AST_FLOAT_LITERAL: return TY_FLOAT;
        case AST_BOOLEAN_LITERAL: return TY_BOOL;
        case AST_STRING_LITERAL: return TY_STRING;

        case AST_ARRAY_LITERAL:
            for (AstNode *e = n->array_literal.elements; e; e = e->next)
                (void)expr_type(tc, e);
            return TY_ARRAY;

        case AST_VARIABLE: {
            TypeKind t = find_symbol(tc, n->variable.name);
            if (t == TY_UNDECLARED) {
                char message[256];
                snprintf(message, sizeof(message), "use of undeclared variable '%s'.", n->variable.name);
                error(tc, message);
            }
            return t == TY_UNDECLARED ? TY_UNKNOWN : t;
        }

        case AST_INDEX_EXPRESSION: {
            TypeKind container = expr_type(tc, n->index_expression.array);
            TypeKind index = expr_type(tc, n->index_expression.index);
            if (index != TY_INT && index != TY_UNKNOWN)
                error(tc, "array/string index must be an integer.");
            if (container != TY_ARRAY && container != TY_STRING &&
                container != TY_UNKNOWN && container != TY_OBJECT)
                error(tc, "indexing requires an array, string, or object.");
            return container == TY_STRING ? TY_STRING : TY_UNKNOWN;
        }

        case AST_MEMBER_EXPRESSION:
            (void)expr_type(tc, n->member_expression.object);
            return TY_UNKNOWN;

        case AST_FUNCTION_CALL:
            check_call(tc, n);
            if (find_struct(tc, n->function_call.name) &&
                n->function_call.argument_count != 0)
                error(tc, "struct constructors currently take no arguments.");
            if (!strcmp(n->function_call.name, "len") ||
                !strcmp(n->function_call.name, "mod") ||
                !strcmp(n->function_call.name, "ffi_int"))
                return TY_INT;
            if (!strcmp(n->function_call.name, "sqrt") ||
                !strcmp(n->function_call.name, "sin") ||
                !strcmp(n->function_call.name, "cos") ||
                !strcmp(n->function_call.name, "tan") ||
                !strcmp(n->function_call.name, "abs") ||
                !strcmp(n->function_call.name, "floor") ||
                !strcmp(n->function_call.name, "ceil"))
                return TY_FLOAT;
            if (!strcmp(n->function_call.name, "process_start")) return TY_OBJECT;
            if (!strcmp(n->function_call.name, "process_stdout") ||
                !strcmp(n->function_call.name, "process_stderr")) return TY_STRING;
            if (!strcmp(n->function_call.name, "input") ||
                !strcmp(n->function_call.name, "fs_read") ||
                !strcmp(n->function_call.name, "cwd")) return TY_STRING;
            if (!strcmp(n->function_call.name, "fs_exists") ||
                !strcmp(n->function_call.name, "fs_write") ||
                !strcmp(n->function_call.name, "fs_append") ||
                !strcmp(n->function_call.name, "fs_delete") ||
                !strcmp(n->function_call.name, "dir_create") ||
                !strcmp(n->function_call.name, "env_set")) return TY_BOOL;
            if (!strcmp(n->function_call.name, "time_ms")) return TY_FLOAT;
            if (!strcmp(n->function_call.name, "process_pid") ||
                !strcmp(n->function_call.name, "process_exit_code")) return TY_INT;
            if (!strcmp(n->function_call.name, "process_poll") ||
                !strcmp(n->function_call.name, "process_wait") ||
                !strcmp(n->function_call.name, "process_terminate")) return TY_BOOL;
            if (!strcmp(n->function_call.name, "object")) return TY_OBJECT;
            if (find_struct(tc, n->function_call.name)) return TY_OBJECT;
            return TY_UNKNOWN;

        case AST_UNARY_EXPRESSION: {
            TypeKind t = expr_type(tc, n->unary_expression.operand);
            if (n->unary_expression.operator == UNARY_NOT) {
                if (t != TY_BOOL && t != TY_UNKNOWN) error(tc, "'!' requires a boolean.");
                return TY_BOOL;
            }
            if (n->unary_expression.operator == UNARY_NEGATE) {
                if (!numeric(t) && t != TY_UNKNOWN) error(tc, "unary '-' requires a number.");
                return t;
            }
            if (n->unary_expression.operator == UNARY_REFERENCE) return TY_REFERENCE;
            if (n->unary_expression.operator == UNARY_DEREFERENCE) {
                if (t != TY_REFERENCE && t != TY_UNKNOWN)
                    error(tc, "dereference requires a reference.");
                return TY_UNKNOWN;
            }
            return TY_UNKNOWN;
        }

        case AST_BINARY_EXPRESSION: {
            TypeKind left = expr_type(tc, n->binary_expression.left);
            TypeKind right = expr_type(tc, n->binary_expression.right);
            BinaryOperator op = n->binary_expression.operator;

            if (op == BINARY_AND || op == BINARY_OR) {
                if ((left != TY_BOOL && left != TY_UNKNOWN) ||
                    (right != TY_BOOL && right != TY_UNKNOWN))
                    error(tc, "logical operators require booleans.");
                return TY_BOOL;
            }

            if (op == BINARY_EQUAL || op == BINARY_NOT_EQUAL ||
                op == BINARY_GREATER || op == BINARY_LESS ||
                op == BINARY_GREATER_EQUAL || op == BINARY_LESS_EQUAL)
                return TY_BOOL;

            if (op == BINARY_ADD &&
                (left == TY_STRING || right == TY_STRING)) {
                TypeKind other = left == TY_STRING ? right : left;
                if (other == TY_STRING || other == TY_INT ||
                    other == TY_FLOAT || other == TY_BOOL ||
                    other == TY_UNKNOWN)
                    return TY_STRING;
                error(tc, "string concatenation requires string, number, or boolean values.");
                return TY_UNKNOWN;
            }

            if (!numeric(left) || !numeric(right)) {
                if (left != TY_UNKNOWN && right != TY_UNKNOWN)
                    error(tc, "arithmetic requires compatible numeric values.");
                return TY_UNKNOWN;
            }

            return left == TY_FLOAT || right == TY_FLOAT ? TY_FLOAT : TY_INT;
        }

        default:
            return TY_UNKNOWN;
    }
}

static void collect_declarations(TypeChecker *tc, AstNode *n) {
    for (; n; n = n->next) {
        if (n->type == AST_FUNCTION_DECLARATION) {
            if (find_function(tc, n->function_declaration.name)) {
                error(tc, "duplicate function definition.");
                continue;
            }
            if (tc->function_count == tc->function_capacity) {
                int cap = tc->function_capacity ? tc->function_capacity * 2 : 16;
                FunctionDef *f = realloc(tc->functions, sizeof(*f) * cap);
                if (!f) { error(tc, "out of memory."); return; }
                tc->functions = f;
                tc->function_capacity = cap;
            }
            tc->functions[tc->function_count].name = strdup(n->function_declaration.name);
            tc->functions[tc->function_count].arity = n->function_declaration.parameter_count;
            if (!tc->functions[tc->function_count].name) { error(tc, "out of memory."); return; }
            tc->function_count++;
        }

        if (n->type == AST_STRUCT_DECLARATION) {
            if (find_struct(tc, n->struct_declaration.name)) {
                error(tc, "duplicate struct definition.");
                continue;
            }
            if (tc->struct_count == tc->struct_capacity) {
                int cap = tc->struct_capacity ? tc->struct_capacity * 2 : 8;
                StructDef *s = realloc(tc->structs, sizeof(*s) * cap);
                if (!s) { error(tc, "out of memory."); return; }
                tc->structs = s;
                tc->struct_capacity = cap;
            }
            tc->structs[tc->struct_count].name = strdup(n->struct_declaration.name);
            tc->structs[tc->struct_count].field_count = n->struct_declaration.field_count;
            if (!tc->structs[tc->struct_count].name) { error(tc, "out of memory."); return; }
            tc->struct_count++;
        }
    }
}

static void check_statements(TypeChecker *tc, AstNode *n) {
    for (; n && !tc->error; n = n->next) {
        switch (n->type) {
            case AST_VARIABLE_DECLARATION:
                /*
                 * Assignment syntax is intentionally implicit: the first
                 * assignment introduces a variable, later assignments update it.
                 * Record that distinction after name resolution so the parser
                 * does not need to guess about scopes.
                 */
                n->variable_declaration.is_reassignment =
                    find_symbol(tc, n->variable_declaration.name) != TY_UNDECLARED;

                TypeKind value_type =
                    expr_type(tc, n->variable_declaration.value);
                if (n->variable_declaration.declared_type) {
                    TypeKind declared_type =
                        type_from_name(n->variable_declaration.declared_type);
                    if (declared_type == TY_UNDECLARED) {
                        char message[256];
                        snprintf(message, sizeof(message),
                                 "unknown type '%s'.",
                                 n->variable_declaration.declared_type);
                        error(tc, message);
                        break;
                    }
                    if (n->variable_declaration.is_reassignment) {
                        error(tc, "'let' cannot redeclare an existing variable.");
                        break;
                    }
                    if (value_type != TY_UNKNOWN &&
                        value_type != declared_type) {
                        char message[256];
                        snprintf(message, sizeof(message),
                                 "initializer type does not match declared type '%s'.",
                                 n->variable_declaration.declared_type);
                        error(tc, message);
                        break;
                    }
                    set_symbol(tc, n->variable_declaration.name, declared_type, 1);
                } else {
                    set_symbol(tc, n->variable_declaration.name, value_type, 0);
                }
                break;

            case AST_ASSIGNMENT: {
                TypeKind target = expr_type(tc, n->assignment.target);
                TypeKind value = expr_type(tc, n->assignment.value);
                if (target != TY_UNKNOWN && target != TY_UNDECLARED &&
                    value != TY_UNKNOWN && target != value &&
                    !(numeric(target) && numeric(value)))
                    error(tc, "assignment changes an incompatible type.");
                break;
            }

            case AST_PRINT_STATEMENT:
                (void)expr_type(tc, n->print_statement.expression);
                break;

            case AST_IF_STATEMENT: {
                TypeKind condition = expr_type(tc, n->if_statement.condition);
                if (condition != TY_BOOL && condition != TY_UNKNOWN)
                    error(tc, "if condition must be boolean.");
                check_statements(tc, n->if_statement.then_branch);
                check_statements(tc, n->if_statement.else_branch);
                break;
            }

            case AST_WHILE_STATEMENT: {
                TypeKind condition = expr_type(tc, n->while_statement.condition);
                if (condition != TY_BOOL && condition != TY_UNKNOWN)
                    error(tc, "while condition must be boolean.");
                check_statements(tc, n->while_statement.body);
                break;
            }

            case AST_RETURN_STATEMENT:
                (void)expr_type(tc, n->return_statement.expression);
                break;

            case AST_FUNCTION_CALL:
                (void)expr_type(tc, n);
                break;

            case AST_FUNCTION_DECLARATION: {
                int old_count = tc->count;
                for (int i = 0; i < n->function_declaration.parameter_count; ++i)
                    set_symbol(tc, n->function_declaration.parameters[i], TY_UNKNOWN, 0);
                check_statements(tc, n->function_declaration.body);
                while (tc->count > old_count) {
                    free(tc->symbols[tc->count - 1].name);
                    tc->count--;
                }
                break;
            }

            case AST_STRUCT_DECLARATION:
            case AST_BREAK_STATEMENT:
            case AST_CONTINUE_STATEMENT:
                break;

            default:
                break;
        }
    }
}

int vnt_typecheck(AstNode *program) {
    if (!program || program->type != AST_PROGRAM) return 0;

    TypeChecker tc = {0};
    collect_declarations(&tc, program->program.statements);
    if (!tc.error) check_statements(&tc, program->program.statements);

    for (int i = 0; i < tc.count; ++i) free(tc.symbols[i].name);
    for (int i = 0; i < tc.function_count; ++i) free(tc.functions[i].name);
    for (int i = 0; i < tc.struct_count; ++i) free(tc.structs[i].name);
    free(tc.symbols);
    free(tc.functions);
    free(tc.structs);

    return !tc.error;
}

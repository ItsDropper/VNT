#include <vnt/typecheck.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TY_UNDECLARED, TY_UNKNOWN, TY_INT, TY_FLOAT, TY_BOOL, TY_STRING,
    TY_ARRAY, TY_OBJECT, TY_REFERENCE, TY_VOID
} TypeKind;

typedef struct { char *name; TypeKind type; int explicit_type; int dynamic; int scope_depth; } Symbol;
typedef struct { char *name; int arity; AstNode *node; } FunctionDef;
typedef struct { char *name; int field_count; } StructDef;

typedef struct {
    Symbol *symbols;
    int count, capacity;
    FunctionDef *functions;
    int function_count, function_capacity;
    StructDef *structs;
    int struct_count, struct_capacity;
    int error;
    TypeKind expected_return;
    int in_function;
    int scope_depth;
} TypeChecker;

static void error(TypeChecker *tc, const char *message) {
    if (!tc->error) fprintf(stderr, "Type error: %s\n", message);
    tc->error = 1;
}

static int find_symbol_index(TypeChecker *tc, const char *name) {
    for (int i = tc->count - 1; i >= 0; --i)
        if (!strcmp(tc->symbols[i].name, name)) return i;
    return -1;
}

static TypeKind find_symbol(TypeChecker *tc, const char *name) {
    int index = find_symbol_index(tc, name);
    return index < 0 ? TY_UNDECLARED : tc->symbols[index].type;
}

static int add_symbol(TypeChecker *tc, const char *name, TypeKind type,
                      int explicit_type) {
    if (tc->count == tc->capacity) {
        int cap = tc->capacity ? tc->capacity * 2 : 32;
        Symbol *symbols = realloc(tc->symbols, sizeof(*symbols) * cap);
        if (!symbols) { error(tc, "out of memory."); return 0; }
        tc->symbols = symbols;
        tc->capacity = cap;
    }
    Symbol *symbol = &tc->symbols[tc->count];
    memset(symbol, 0, sizeof(*symbol));
    symbol->name = strdup(name);
    if (!symbol->name) { error(tc, "out of memory."); return 0; }
    symbol->type = type;
    symbol->explicit_type = explicit_type;
    symbol->scope_depth = tc->scope_depth;
    tc->count++;
    return 1;
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

    (void)add_symbol(tc, name, type, explicit_type);
}

/* Remove declarations introduced by a lexical block. Updates to symbols that
   existed before the block stay attached to those outer symbols. */
static void discard_symbols_to(TypeChecker *tc, int count) {
    while (tc->count > count) {
        free(tc->symbols[tc->count - 1].name);
        tc->count--;
    }
}

/* Branch analysis must not make the else branch depend on the then branch's
   inferred types. Merge only symbols that existed before either branch. */
typedef struct {
    TypeKind type;
    int dynamic;
} SymbolState;

static SymbolState *snapshot_symbols(TypeChecker *tc, int count) {
    if (count <= 0) return NULL;
    SymbolState *states = malloc(sizeof(*states) * (size_t)count);
    if (!states) {
        error(tc, "out of memory.");
        return NULL;
    }
    for (int i = 0; i < count; ++i) {
        states[i].type = tc->symbols[i].type;
        states[i].dynamic = tc->symbols[i].dynamic;
    }
    return states;
}

static void restore_symbols(TypeChecker *tc, const SymbolState *states,
                            int count) {
    for (int i = 0; i < count; ++i) {
        tc->symbols[i].type = states[i].type;
        tc->symbols[i].dynamic = states[i].dynamic;
    }
}

static void merge_branch_symbols(TypeChecker *tc, const SymbolState *before,
                                 const SymbolState *then_state, int count) {
    for (int i = 0; i < count; ++i) {
        TypeKind a = then_state[i].type;
        TypeKind b = tc->symbols[i].type;
        if (then_state[i].dynamic || tc->symbols[i].dynamic ||
            (a != b && a != TY_UNKNOWN && b != TY_UNKNOWN)) {
            if (tc->symbols[i].explicit_type) {
                /* Explicit declarations are validated at each assignment. */
                tc->symbols[i].type = before[i].type;
                tc->symbols[i].dynamic = before[i].dynamic;
            } else {
                tc->symbols[i].type = TY_UNKNOWN;
                tc->symbols[i].dynamic = 1;
            }
        } else if (a == TY_UNKNOWN || b == TY_UNKNOWN) {
            tc->symbols[i].type = TY_UNKNOWN;
            tc->symbols[i].dynamic = then_state[i].dynamic || tc->symbols[i].dynamic;
        } else {
            tc->symbols[i].type = a;
        }
    }
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
    if (!strcmp(name, "void")) return TY_VOID;
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

    /* Native Windows GUI API. Keep its signatures in sync with the
       runtime dispatch table in x86_backend.c. */
    if (!strcmp(name, "gui_css")) return argc == 1;
    if (!strcmp(name, "gui_button") || !strcmp(name, "gui_text_at") ||
        !strcmp(name, "gui_title") || !strcmp(name, "gui_input_set")) return argc == 3;
    if (!strcmp(name, "gui_panel") || !strcmp(name, "gui_input") ||
        !strcmp(name, "gui_checkbox") || !strcmp(name, "gui_progress")) return argc == 4;
    if (!strcmp(name, "gui_button_sized") || !strcmp(name, "gui_textarea") ||
        !strcmp(name, "gui_panel_color") || !strcmp(name, "gui_text_style")) return argc == 5;
    if (!strcmp(name, "gui_separator")) return argc == 3;
    if (!strcmp(name, "gui_size")) return argc == 2;
    if (!strcmp(name, "gui_open") || !strcmp(name, "gui_text") ||
        !strcmp(name, "gui_fill") || !strcmp(name, "gui_rect")) return argc == 1;
    if (!strcmp(name, "gui_poll") || !strcmp(name, "gui_key") ||
        !strcmp(name, "gui_close") || !strcmp(name, "gui_present")) return argc == 0;
    return -1;
}

static TypeKind expr_type(TypeChecker *tc, AstNode *n);

/* One compatibility rule shared by initializers, calls, assignments, and returns.
   Numeric values interoperate because VNT arithmetic already supports int/float
   mixtures; unknown values remain dynamic by design. */
static int type_compatible(TypeKind expected, TypeKind actual) {
    return expected == TY_UNKNOWN || actual == TY_UNKNOWN ||
           expected == actual || (numeric(expected) && numeric(actual));
}

static void check_call(TypeChecker *tc, AstNode *n) {
    int argc = n->function_call.argument_count;
    for (AstNode *a = n->function_call.arguments; a; a = a->next) (void)expr_type(tc, a);
    int builtin = builtin_arity(n->function_call.name, argc);
    if (builtin != -1) { if (!builtin) error(tc, "invalid argument count for builtin function."); return; }
    if (find_struct(tc, n->function_call.name)) return;
    FunctionDef *fn = find_function(tc, n->function_call.name);
    if (!fn) {
        char message[256];
        snprintf(message, sizeof(message), "call to undeclared function '%s'.", n->function_call.name);
        error(tc, message); return;
    }
    if (fn->arity != argc) { error(tc, "function called with the wrong number of arguments."); return; }
    AstNode *arg = n->function_call.arguments;
    for (int i = 0; i < argc && arg; ++i, arg = arg->next) {
        AstNode *decl = fn->node;
        if (!decl || !decl->function_declaration.parameter_types || !decl->function_declaration.parameter_types[i]) continue;
        TypeKind expected = type_from_name(decl->function_declaration.parameter_types[i]);
        TypeKind actual = expr_type(tc, arg);
        if (expected == TY_UNDECLARED || expected == TY_VOID)
            error(tc, "function parameter uses an invalid type.");
        else if (!type_compatible(expected, actual))
            error(tc, "function argument type does not match its parameter annotation.");
    }
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
            if (!strcmp(n->function_call.name, "gui_open") ||
                !strcmp(n->function_call.name, "gui_poll") ||
                !strcmp(n->function_call.name, "gui_css") ||
                !strcmp(n->function_call.name, "gui_button") ||
                !strcmp(n->function_call.name, "gui_button_sized") ||
                !strcmp(n->function_call.name, "gui_text_at") ||
                !strcmp(n->function_call.name, "gui_title") ||
                !strcmp(n->function_call.name, "gui_panel") ||
                !strcmp(n->function_call.name, "gui_panel_color") ||
                !strcmp(n->function_call.name, "gui_text_style") ||
                !strcmp(n->function_call.name, "gui_input_set") ||
                !strcmp(n->function_call.name, "gui_checkbox") ||
                !strcmp(n->function_call.name, "gui_progress") ||
                !strcmp(n->function_call.name, "gui_separator") ||
                !strcmp(n->function_call.name, "gui_present")) return TY_BOOL;
            if (!strcmp(n->function_call.name, "gui_key")) return TY_INT;
            if (!strcmp(n->function_call.name, "gui_input") || !strcmp(n->function_call.name, "gui_textarea")) return TY_STRING;
            if (!strcmp(n->function_call.name, "gui_size") ||
                !strcmp(n->function_call.name, "gui_text") ||
                !strcmp(n->function_call.name, "gui_fill") ||
                !strcmp(n->function_call.name, "gui_rect") ||
                !strcmp(n->function_call.name, "gui_close")) return TY_BOOL;
            if (find_struct(tc, n->function_call.name)) return TY_OBJECT;
            {
                FunctionDef *fn = find_function(tc, n->function_call.name);
                if (fn && fn->node && fn->node->function_declaration.return_type) {
                    TypeKind result = type_from_name(fn->node->function_declaration.return_type);
                    return result == TY_UNDECLARED ? TY_UNKNOWN : result;
                }
            }
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

            if (op == BINARY_EQUAL || op == BINARY_NOT_EQUAL) {
                /*
                 * Equality is defined for values of the same static type and
                 * for mixed numeric values. Unknown values remain permitted
                 * because inferred variables can intentionally be dynamic.
                 */
                if (left != TY_UNKNOWN && right != TY_UNKNOWN &&
                    left != right && !(numeric(left) && numeric(right))) {
                    error(tc, "equality operands must have compatible types.");
                }
                return TY_BOOL;
            }

            if (op == BINARY_GREATER || op == BINARY_LESS ||
                op == BINARY_GREATER_EQUAL || op == BINARY_LESS_EQUAL) {
                if ((!numeric(left) && left != TY_UNKNOWN) ||
                    (!numeric(right) && right != TY_UNKNOWN)) {
                    error(tc, "ordered comparisons require numeric operands.");
                }
                return TY_BOOL;
            }

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
            tc->functions[tc->function_count].node = n;
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

static int statement_list_returns(AstNode *n) {
    for (; n; n = n->next) {
        if (n->type == AST_RETURN_STATEMENT) return 1;
        if (n->type == AST_IF_STATEMENT && n->if_statement.else_branch &&
            statement_list_returns(n->if_statement.then_branch) &&
            statement_list_returns(n->if_statement.else_branch)) return 1;
    }
    return 0;
}

static void check_statements(TypeChecker *tc, AstNode *n) {
    for (; n && !tc->error; n = n->next) {
        switch (n->type) {
            case AST_VARIABLE_DECLARATION: {
                /*
                 * Assignment syntax is intentionally implicit: the first
                 * assignment introduces a variable, later assignments update it.
                 * Record that distinction after name resolution so the parser
                 * does not need to guess about scopes.
                 */
                int existing_index =
                    find_symbol_index(tc, n->variable_declaration.name);
                int shadows_outer =
                    n->variable_declaration.declared_type &&
                    existing_index >= 0 &&
                    tc->symbols[existing_index].scope_depth < tc->scope_depth;
                n->variable_declaration.is_reassignment =
                    existing_index >= 0 && !shadows_outer;

                TypeKind value_type =
                    expr_type(tc, n->variable_declaration.value);
                if (n->variable_declaration.declared_type) {
                    TypeKind declared_type =
                        type_from_name(n->variable_declaration.declared_type);
                    if (declared_type == TY_UNDECLARED || declared_type == TY_VOID) {
                        char message[256];
                        snprintf(message, sizeof(message),
                                 "unknown type '%s'.",
                                 n->variable_declaration.declared_type);
                        error(tc, message);
                        break;
                    }
                    if (existing_index >= 0 && !shadows_outer) {
                        error(tc, "'let' cannot redeclare a variable in the same scope.");
                        break;
                    }
                    if (!type_compatible(declared_type, value_type)) {
                        char message[256];
                        snprintf(message, sizeof(message),
                                 "initializer type does not match declared type '%s'.",
                                 n->variable_declaration.declared_type);
                        error(tc, message);
                        break;
                    }
                    if (shadows_outer)
                        (void)add_symbol(tc, n->variable_declaration.name, declared_type, 1);
                    else
                        set_symbol(tc, n->variable_declaration.name, declared_type, 1);
                } else {
                    set_symbol(tc, n->variable_declaration.name, value_type, 0);
                }
                break;
            }

            case AST_ASSIGNMENT: {
                TypeKind target = expr_type(tc, n->assignment.target);
                TypeKind value = expr_type(tc, n->assignment.value);
                if (target != TY_UNKNOWN && target != TY_UNDECLARED &&
                    !type_compatible(target, value))
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
                {
                    int outer_count = tc->count;
                    SymbolState *before = snapshot_symbols(tc, outer_count);
                    if (outer_count && !before) break;

                    tc->scope_depth++;
                    check_statements(tc, n->if_statement.then_branch);
                    SymbolState *then_state = snapshot_symbols(tc, outer_count);
                    discard_symbols_to(tc, outer_count);
                    tc->scope_depth--;

                    restore_symbols(tc, before, outer_count);
                    if (!tc->error) {
                        tc->scope_depth++;
                        check_statements(tc, n->if_statement.else_branch);
                        discard_symbols_to(tc, outer_count);
                        tc->scope_depth--;
                        if (then_state)
                            merge_branch_symbols(tc, before, then_state, outer_count);
                    }
                    free(then_state);
                    free(before);
                }
                break;
            }

            case AST_WHILE_STATEMENT: {
                TypeKind condition = expr_type(tc, n->while_statement.condition);
                if (condition != TY_BOOL && condition != TY_UNKNOWN)
                    error(tc, "while condition must be boolean.");
                {
                    int outer_count = tc->count;
                    tc->scope_depth++;
                    check_statements(tc, n->while_statement.body);
                    discard_symbols_to(tc, outer_count);
                    tc->scope_depth--;
                }
                break;
            }

            case AST_RETURN_STATEMENT: {
                if (!tc->in_function) { error(tc, "return used outside a function."); break; }
                TypeKind actual = expr_type(tc, n->return_statement.expression);
                if (tc->expected_return == TY_VOID) {
                    if (n->return_statement.expression)
                        error(tc, "void function cannot return a value.");
                } else if (!n->return_statement.expression) {
                    error(tc, "non-void function must return a value.");
                } else if (tc->expected_return != TY_UNKNOWN &&
                           !type_compatible(tc->expected_return, actual)) {
                    error(tc, "return expression does not match the function return type.");
                }
                break;
            }

            case AST_FUNCTION_CALL:
                (void)expr_type(tc, n);
                break;

            case AST_FUNCTION_DECLARATION: {
                int old_count = tc->count;
                int old_scope_depth = tc->scope_depth;
                TypeKind old_return = tc->expected_return;
                int old_in_function = tc->in_function;
                tc->scope_depth = old_scope_depth + 1;
                tc->in_function = 1;
                tc->expected_return = n->function_declaration.return_type
                    ? type_from_name(n->function_declaration.return_type) : TY_UNKNOWN;
                if (n->function_declaration.return_type && tc->expected_return == TY_UNDECLARED)
                    error(tc, "function declares an unknown return type.");
                for (int i = 0; i < n->function_declaration.parameter_count; ++i) {
                    TypeKind pt = TY_UNKNOWN;
                    if (n->function_declaration.parameter_types && n->function_declaration.parameter_types[i]) {
                        pt = type_from_name(n->function_declaration.parameter_types[i]);
                        if (pt == TY_UNDECLARED || pt == TY_VOID) {
                            error(tc, "function declares an invalid parameter type."); break;
                        }
                    }
                    if (!add_symbol(tc, n->function_declaration.parameters[i],
                                     pt, pt != TY_UNKNOWN)) break;
                }
                check_statements(tc, n->function_declaration.body);
                if (tc->expected_return != TY_UNKNOWN && tc->expected_return != TY_VOID &&
                    !statement_list_returns(n->function_declaration.body))
                    error(tc, "not all paths in a typed function return a value.");
                while (tc->count > old_count) { free(tc->symbols[tc->count - 1].name); tc->count--; }
                tc->expected_return = old_return;
                tc->in_function = old_in_function;
                tc->scope_depth = old_scope_depth;
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

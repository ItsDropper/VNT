#include <vnt/typecheck.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TY_UNKNOWN, TY_INT, TY_FLOAT, TY_BOOL, TY_STRING,
    TY_ARRAY, TY_OBJECT, TY_REFERENCE
} TypeKind;

typedef struct { char *name; TypeKind type; } Symbol;

typedef struct {
    Symbol *symbols;
    int count;
    int capacity;
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
    return TY_UNKNOWN;
}

static void set_symbol(TypeChecker *tc, const char *name, TypeKind type) {
    for (int i = tc->count - 1; i >= 0; --i) {
        if (!strcmp(tc->symbols[i].name, name)) {
            tc->symbols[i].type = type;
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
    tc->symbols[tc->count++].type = type;
}

static int numeric(TypeKind t) { return t == TY_INT || t == TY_FLOAT; }

static TypeKind expr_type(TypeChecker *tc, AstNode *n);

static void check_call(TypeChecker *tc, AstNode *n) {
    for (AstNode *a = n->function_call.arguments; a; a = a->next)
        (void)expr_type(tc, a);
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

        case AST_VARIABLE:
            return find_symbol(tc, n->variable.name);

        case AST_INDEX_EXPRESSION: {
            TypeKind container = expr_type(tc, n->index_expression.array);
            TypeKind index = expr_type(tc, n->index_expression.index);
            if (index != TY_INT && index != TY_UNKNOWN)
                error(tc, "array/string index must be an integer.");
            if (container != TY_ARRAY && container != TY_STRING && container != TY_UNKNOWN)
                error(tc, "indexing requires an array or string.");
            return container == TY_STRING ? TY_STRING : TY_UNKNOWN;
        }

        case AST_MEMBER_EXPRESSION:
            (void)expr_type(tc, n->member_expression.object);
            return TY_UNKNOWN;

        case AST_FUNCTION_CALL:
            check_call(tc, n);
            if (!strcmp(n->function_call.name, "len") ||
                !strcmp(n->function_call.name, "mod") ||
                !strcmp(n->function_call.name, "ffi_int"))
                return TY_INT;
            if (!strcmp(n->function_call.name, "sqrt") ||
                !strcmp(n->function_call.name, "sin") ||
                !strcmp(n->function_call.name, "cos") ||
                !strcmp(n->function_call.name, "tan") ||
                !strcmp(n->function_call.name, "floor") ||
                !strcmp(n->function_call.name, "ceil"))
                return TY_FLOAT;
            if (!strcmp(n->function_call.name, "input")) return TY_STRING;
            if (!strcmp(n->function_call.name, "object")) return TY_OBJECT;
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
            if (n->unary_expression.operator == UNARY_DEREFERENCE) return TY_UNKNOWN;
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

            if (op == BINARY_ADD && left == TY_STRING && right == TY_STRING)
                return TY_STRING;

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

static void check_statements(TypeChecker *tc, AstNode *n) {
    for (; n && !tc->error; n = n->next) {
        switch (n->type) {
            case AST_VARIABLE_DECLARATION:
                set_symbol(tc, n->variable_declaration.name,
                           expr_type(tc, n->variable_declaration.value));
                break;

            case AST_ASSIGNMENT: {
                TypeKind target = expr_type(tc, n->assignment.target);
                TypeKind value = expr_type(tc, n->assignment.value);
                if (target != TY_UNKNOWN && value != TY_UNKNOWN &&
                    target != value &&
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
                    set_symbol(tc, n->function_declaration.parameters[i], TY_UNKNOWN);
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
    check_statements(&tc, program->program.statements);

    for (int i = 0; i < tc.count; ++i) free(tc.symbols[i].name);
    free(tc.symbols);

    return !tc.error;
}

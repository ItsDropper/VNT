#include <vnt/ir_lower.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static VntIrOpcode opcode_for(AstNodeType type) {
    switch (type) {
        case AST_PROGRAM: return VNT_IR_PROGRAM;
        case AST_PRINT_STATEMENT: return VNT_IR_PRINT;
        case AST_IF_STATEMENT: return VNT_IR_IF;
        case AST_WHILE_STATEMENT: return VNT_IR_WHILE;
        case AST_FUNCTION_DECLARATION: return VNT_IR_FUNCTION;
        case AST_STRUCT_DECLARATION: return VNT_IR_STRUCT;
        case AST_FUNCTION_CALL: return VNT_IR_CALL;
        case AST_RETURN_STATEMENT: return VNT_IR_RETURN;
        case AST_BREAK_STATEMENT: return VNT_IR_BREAK;
        case AST_CONTINUE_STATEMENT: return VNT_IR_CONTINUE;
        case AST_STRING_LITERAL: return VNT_IR_STRING;
        case AST_INTEGER_LITERAL: return VNT_IR_INTEGER;
        case AST_FLOAT_LITERAL: return VNT_IR_FLOAT;
        case AST_BOOLEAN_LITERAL: return VNT_IR_BOOLEAN;
        case AST_ARRAY_LITERAL: return VNT_IR_ARRAY;
        case AST_VARIABLE_DECLARATION: return VNT_IR_VARIABLE_DECL;
        case AST_VARIABLE: return VNT_IR_VARIABLE;
        case AST_INDEX_EXPRESSION: return VNT_IR_INDEX;
        case AST_MEMBER_EXPRESSION: return VNT_IR_MEMBER;
        case AST_ASSIGNMENT: return VNT_IR_ASSIGN;
        case AST_BINARY_EXPRESSION: return VNT_IR_BINARY;
        case AST_UNARY_EXPRESSION: return VNT_IR_UNARY;
        default: return (VntIrOpcode)-1;
    }
}

static void clear_nodes(VntIrProgram *ir) {
    for (size_t i = 0; i < ir->node_count; ++i) {
        VntIrOpcode op = ir->nodes[i].opcode;
        if (op == VNT_IR_STRING || op == VNT_IR_VARIABLE_DECL ||
            op == VNT_IR_REASSIGN || op == VNT_IR_VARIABLE ||
            op == VNT_IR_MEMBER || op == VNT_IR_CALL ||
            op == VNT_IR_FUNCTION || op == VNT_IR_STRUCT)
            free(ir->nodes[i].value.text);
        for (size_t j = 0; j < ir->nodes[i].name_count; ++j)
            free(ir->nodes[i].names[j]);
        free(ir->nodes[i].names);
        free(ir->nodes[i].type_name);
    }
    free(ir->nodes);
    ir->nodes = NULL;
    ir->node_count = 0;
    ir->node_capacity = 0;
    ir->root = VNT_IR_NO_NODE;
}

static int reserve_node(VntIrProgram *ir, size_t *index) {
    if (ir->node_count == ir->node_capacity) {
        size_t cap = ir->node_capacity ? ir->node_capacity * 2 : 64;
        if (cap < ir->node_capacity || cap > SIZE_MAX / sizeof(*ir->nodes)) return 0;
        VntIrNode *nodes = realloc(ir->nodes, cap * sizeof(*nodes));
        if (!nodes) return 0;
        ir->nodes = nodes;
        ir->node_capacity = cap;
    }
    *index = ir->node_count++;
    VntIrNode *node = &ir->nodes[*index];
    memset(node, 0, sizeof(*node));
    node->first_child = node->last_child = node->next_sibling = VNT_IR_NO_NODE;
    return 1;
}

static int add_child(VntIrProgram *ir, size_t parent, size_t child) {
    if (parent >= ir->node_count || child >= ir->node_count) return 0;
    if (ir->nodes[parent].last_child == VNT_IR_NO_NODE)
        ir->nodes[parent].first_child = child;
    else
        ir->nodes[ir->nodes[parent].last_child].next_sibling = child;
    ir->nodes[parent].last_child = child;
    ir->nodes[parent].child_count++;
    return 1;
}

static int lower_node(VntIrProgram *ir, const AstNode *ast, size_t *result);

static int lower_list(VntIrProgram *ir, size_t parent, const AstNode *ast, VntIrEdgeRole role) {
    for (const AstNode *item = ast; item; item = item->next) {
        size_t child;
        if (!lower_node(ir, item, &child)) return 0;
        ir->nodes[child].role = role;
        if (!add_child(ir, parent, child)) return 0;
    }
    return 1;
}

static int lower_one(VntIrProgram *ir, size_t parent, const AstNode *ast, VntIrEdgeRole role) {
    if (!ast) return 1;
    size_t child;
    if (!lower_node(ir, ast, &child)) return 0;
    ir->nodes[child].role = role;
    return add_child(ir, parent, child);
}

static int copy_text(char **destination, const char *source);

static int copy_names(VntIrNode *node, char *const *names, int count) {
    if (count <= 0) return 1;
    node->names = calloc((size_t)count, sizeof(*node->names));
    if (!node->names) return 0;
    node->name_count = (size_t)count;
    for (int i = 0; i < count; ++i)
        if (!copy_text(&node->names[i], names[i])) return 0;
    return 1;
}

static int copy_text(char **destination, const char *source) {
    if (!source) source = "";
    size_t len = strlen(source);
    char *copy = malloc(len + 1);
    if (!copy) return 0;
    memcpy(copy, source, len + 1);
    *destination = copy;
    return 1;
}

static int lower_node(VntIrProgram *ir, const AstNode *ast, size_t *result) {
    if (!ast || !result) return 0;
    VntIrOpcode op = opcode_for(ast->type);
    if (ast->type == AST_VARIABLE_DECLARATION &&
        ast->variable_declaration.is_reassignment)
        op = VNT_IR_REASSIGN;
    if ((int)op < 0 || !reserve_node(ir, result)) return 0;
    VntIrNode *node = &ir->nodes[*result];
    node->opcode = op;
    node->role = VNT_IR_EDGE_ROOT;
    if (ast->type == AST_VARIABLE_DECLARATION &&
        ast->variable_declaration.declared_type &&
        !copy_text(&node->type_name,
                   ast->variable_declaration.declared_type)) return 0;
    if (ast->type == AST_BINARY_EXPRESSION)
        node->operation = (int)ast->binary_expression.operator;
    else if (ast->type == AST_UNARY_EXPRESSION)
        node->operation = (int)ast->unary_expression.operator;

    switch (ast->type) {
        case AST_INTEGER_LITERAL: node->value.integer = ast->integer_literal.value; break;
        case AST_BOOLEAN_LITERAL: node->value.boolean = !!ast->boolean_literal.value; break;
        case AST_FLOAT_LITERAL: node->value.floating = ast->float_literal.value; break;
        case AST_STRING_LITERAL:
            if (!copy_text(&node->value.text, ast->string_literal.value)) return 0;
            break;
        case AST_VARIABLE_DECLARATION:
            if (!copy_text(&node->value.text, ast->variable_declaration.name)) return 0;
            break;
        case AST_VARIABLE:
            if (!copy_text(&node->value.text, ast->variable.name)) return 0;
            break;
        case AST_MEMBER_EXPRESSION:
            if (!copy_text(&node->value.text, ast->member_expression.member)) return 0;
            break;
        case AST_FUNCTION_CALL:
            if (!copy_text(&node->value.text, ast->function_call.name)) return 0;
            break;
        case AST_FUNCTION_DECLARATION:
            if (!copy_text(&node->value.text, ast->function_declaration.name) ||
                !copy_names(node, ast->function_declaration.parameters,
                            ast->function_declaration.parameter_count)) return 0;
            break;
        case AST_STRUCT_DECLARATION:
            if (!copy_text(&node->value.text, ast->struct_declaration.name) ||
                !copy_names(node, ast->struct_declaration.fields,
                            ast->struct_declaration.field_count)) return 0;
            break;
        default: break;
    }

    switch (ast->type) {
        case AST_PROGRAM: return lower_list(ir, *result, ast->program.statements, VNT_IR_EDGE_STATEMENT);
        case AST_PRINT_STATEMENT: return lower_one(ir, *result, ast->print_statement.expression, VNT_IR_EDGE_VALUE);
        case AST_IF_STATEMENT:
            return lower_one(ir, *result, ast->if_statement.condition, VNT_IR_EDGE_CONDITION) &&
                   lower_list(ir, *result, ast->if_statement.then_branch, VNT_IR_EDGE_THEN) &&
                   lower_list(ir, *result, ast->if_statement.else_branch, VNT_IR_EDGE_ELSE);
        case AST_WHILE_STATEMENT:
            return lower_one(ir, *result, ast->while_statement.condition, VNT_IR_EDGE_CONDITION) &&
                   lower_list(ir, *result, ast->while_statement.body, VNT_IR_EDGE_BODY);
        case AST_FUNCTION_DECLARATION: return lower_list(ir, *result, ast->function_declaration.body, VNT_IR_EDGE_BODY);
        case AST_VARIABLE_DECLARATION: return lower_one(ir, *result, ast->variable_declaration.value, VNT_IR_EDGE_VALUE);
        case AST_FUNCTION_CALL: return lower_list(ir, *result, ast->function_call.arguments, VNT_IR_EDGE_ARGUMENT);
        case AST_RETURN_STATEMENT: return lower_one(ir, *result, ast->return_statement.expression, VNT_IR_EDGE_VALUE);
        case AST_ARRAY_LITERAL: return lower_list(ir, *result, ast->array_literal.elements, VNT_IR_EDGE_ELEMENT);
        case AST_INDEX_EXPRESSION:
            return lower_one(ir, *result, ast->index_expression.array, VNT_IR_EDGE_OBJECT) &&
                   lower_one(ir, *result, ast->index_expression.index, VNT_IR_EDGE_INDEX);
        case AST_MEMBER_EXPRESSION: return lower_one(ir, *result, ast->member_expression.object, VNT_IR_EDGE_OBJECT);
        case AST_ASSIGNMENT:
            return lower_one(ir, *result, ast->assignment.target, VNT_IR_EDGE_TARGET) &&
                   lower_one(ir, *result, ast->assignment.value, VNT_IR_EDGE_VALUE);
        case AST_BINARY_EXPRESSION:
            return lower_one(ir, *result, ast->binary_expression.left, VNT_IR_EDGE_LEFT) &&
                   lower_one(ir, *result, ast->binary_expression.right, VNT_IR_EDGE_RIGHT);
        case AST_UNARY_EXPRESSION: return lower_one(ir, *result, ast->unary_expression.operand, VNT_IR_EDGE_OPERAND);
        default: return 1;
    }
}



/* Resolve every variable occurrence to a unique HIR storage key before
   optimization. The source spelling remains in the symbol table; the HIR
   payload is rewritten to an identity-bearing internal name so backend slot
   allocation can never merge shadowed declarations by source name alone. */
typedef struct {
    char *source_name;
    char *storage_key;
    size_t depth;
} HirSymbol;

typedef struct {
    VntIrProgram *ir;
    HirSymbol *symbols;
    size_t count, capacity, next_id, depth;
    int error;
} HirResolver;

static size_t hir_child_role(const VntIrProgram *ir, size_t index,
                             VntIrEdgeRole role) {
    if (index >= ir->node_count) return VNT_IR_NO_NODE;
    for (size_t c = ir->nodes[index].first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        if (ir->nodes[c].role == role) return c;
    return VNT_IR_NO_NODE;
}

static size_t hir_next_role(const VntIrProgram *ir, size_t index,
                            VntIrEdgeRole role) {
    if (index >= ir->node_count) return VNT_IR_NO_NODE;
    for (size_t c = ir->nodes[index].next_sibling; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        if (ir->nodes[c].role == role) return c;
    return VNT_IR_NO_NODE;
}

static void resolver_error(HirResolver *resolver) {
    resolver->error = 1;
}

static int resolver_reserve(HirResolver *resolver) {
    if (resolver->count < resolver->capacity) return 1;
    size_t cap = resolver->capacity ? resolver->capacity * 2 : 32;
    if (cap < resolver->capacity || cap > SIZE_MAX / sizeof(*resolver->symbols))
        return 0;
    HirSymbol *symbols = realloc(resolver->symbols, cap * sizeof(*symbols));
    if (!symbols) return 0;
    resolver->symbols = symbols;
    resolver->capacity = cap;
    return 1;
}

static HirSymbol *resolver_find(HirResolver *resolver, const char *name) {
    for (size_t i = resolver->count; i > 0; --i)
        if (!strcmp(resolver->symbols[i - 1].source_name, name))
            return &resolver->symbols[i - 1];
    return NULL;
}

static HirSymbol *resolver_declare(HirResolver *resolver, const char *name) {
    if (!resolver_reserve(resolver)) {
        resolver_error(resolver);
        return NULL;
    }
    size_t length = strlen(name);
    char suffix[48];
    int suffix_length = snprintf(suffix, sizeof(suffix), "__vnt_symbol_%zu_", resolver->next_id++);
    if (suffix_length < 0 || (size_t)suffix_length >= sizeof(suffix) ||
        length > SIZE_MAX - (size_t)suffix_length - 1) {
        resolver_error(resolver);
        return NULL;
    }
    char *source = malloc(length + 1);
    char *key = malloc((size_t)suffix_length + length + 1);
    if (!source || !key) {
        free(source);
        free(key);
        resolver_error(resolver);
        return NULL;
    }
    memcpy(source, name, length + 1);
    memcpy(key, suffix, (size_t)suffix_length);
    memcpy(key + suffix_length, name, length + 1);
    HirSymbol *symbol = &resolver->symbols[resolver->count++];
    symbol->source_name = source;
    symbol->storage_key = key;
    symbol->depth = resolver->depth;
    return symbol;
}

static int resolver_rename(char **payload, const char *key) {
    char *copy = strdup(key);
    if (!copy) return 0;
    free(*payload);
    *payload = copy;
    return 1;
}

static void resolver_bind_expr(HirResolver *resolver, size_t index);

static void resolver_bind_role(HirResolver *resolver, size_t index,
                               VntIrEdgeRole role) {
    for (size_t c = hir_child_role(resolver->ir, index, role);
         c != VNT_IR_NO_NODE && !resolver->error;
         c = hir_next_role(resolver->ir, c, role))
        resolver_bind_expr(resolver, c);
}

static void resolver_bind_expr(HirResolver *resolver, size_t index) {
    if (resolver->error || index == VNT_IR_NO_NODE ||
        index >= resolver->ir->node_count) return;
    VntIrNode *node = &resolver->ir->nodes[index];
    switch (node->opcode) {
        case VNT_IR_VARIABLE: {
            HirSymbol *symbol = resolver_find(resolver, node->value.text);
            if (!symbol || !resolver_rename(&node->value.text, symbol->storage_key))
                resolver_error(resolver);
            break;
        }
        case VNT_IR_VARIABLE_DECL:
        case VNT_IR_REASSIGN:
            resolver_bind_expr(resolver,
                hir_child_role(resolver->ir, index, VNT_IR_EDGE_VALUE));
            if (resolver->error) return;
            if (node->opcode == VNT_IR_REASSIGN) {
                HirSymbol *symbol = resolver_find(resolver, node->value.text);
                if (!symbol || !resolver_rename(&node->value.text, symbol->storage_key))
                    resolver_error(resolver);
            } else {
                HirSymbol *symbol = resolver_declare(resolver, node->value.text);
                if (!symbol || !resolver_rename(&node->value.text, symbol->storage_key))
                    resolver_error(resolver);
            }
            break;
        case VNT_IR_PROGRAM:
            resolver_bind_role(resolver, index, VNT_IR_EDGE_STATEMENT);
            break;
        case VNT_IR_PRINT:
        case VNT_IR_RETURN:
            resolver_bind_expr(resolver,
                hir_child_role(resolver->ir, index, VNT_IR_EDGE_VALUE));
            break;
        case VNT_IR_IF:
            resolver_bind_expr(resolver,
                hir_child_role(resolver->ir, index, VNT_IR_EDGE_CONDITION));
            if (resolver->error) return;
            resolver->depth++;
            resolver_bind_role(resolver, index, VNT_IR_EDGE_THEN);
            if (resolver->error) return;
            /* Remove declarations from the then branch before resolving else. */
            while (resolver->count &&
                   resolver->symbols[resolver->count - 1].depth >= resolver->depth) {
                free(resolver->symbols[resolver->count - 1].source_name);
                free(resolver->symbols[resolver->count - 1].storage_key);
                resolver->count--;
            }
            resolver_bind_role(resolver, index, VNT_IR_EDGE_ELSE);
            while (resolver->count &&
                   resolver->symbols[resolver->count - 1].depth >= resolver->depth) {
                free(resolver->symbols[resolver->count - 1].source_name);
                free(resolver->symbols[resolver->count - 1].storage_key);
                resolver->count--;
            }
            resolver->depth--;
            break;
        case VNT_IR_WHILE:
            resolver_bind_expr(resolver,
                hir_child_role(resolver->ir, index, VNT_IR_EDGE_CONDITION));
            if (resolver->error) return;
            resolver->depth++;
            resolver_bind_role(resolver, index, VNT_IR_EDGE_BODY);
            while (resolver->count &&
                   resolver->symbols[resolver->count - 1].depth >= resolver->depth) {
                free(resolver->symbols[resolver->count - 1].source_name);
                free(resolver->symbols[resolver->count - 1].storage_key);
                resolver->count--;
            }
            resolver->depth--;
            break;
        case VNT_IR_ASSIGN:
            resolver_bind_expr(resolver,
                hir_child_role(resolver->ir, index, VNT_IR_EDGE_TARGET));
            resolver_bind_expr(resolver,
                hir_child_role(resolver->ir, index, VNT_IR_EDGE_VALUE));
            break;
        case VNT_IR_CALL:
        case VNT_IR_ARRAY:
            for (size_t c = node->first_child; c != VNT_IR_NO_NODE && !resolver->error;
                 c = resolver->ir->nodes[c].next_sibling)
                resolver_bind_expr(resolver, c);
            break;
        case VNT_IR_INDEX:
            resolver_bind_expr(resolver, hir_child_role(resolver->ir, index, VNT_IR_EDGE_OBJECT));
            resolver_bind_expr(resolver, hir_child_role(resolver->ir, index, VNT_IR_EDGE_INDEX));
            break;
        case VNT_IR_MEMBER:
            resolver_bind_expr(resolver, hir_child_role(resolver->ir, index, VNT_IR_EDGE_OBJECT));
            break;
        case VNT_IR_BINARY:
            resolver_bind_expr(resolver, hir_child_role(resolver->ir, index, VNT_IR_EDGE_LEFT));
            resolver_bind_expr(resolver, hir_child_role(resolver->ir, index, VNT_IR_EDGE_RIGHT));
            break;
        case VNT_IR_UNARY:
            resolver_bind_expr(resolver, hir_child_role(resolver->ir, index, VNT_IR_EDGE_OPERAND));
            break;
        default:
            break;
    }
}

static int resolve_hir_symbols(VntIrProgram *ir) {
    HirResolver resolver = {0};
    resolver.ir = ir;
    const size_t root = ir->root;
    const VntIrNode *root_node = &ir->nodes[root];

    /* Predeclare top-level globals so functions can bind references to the
       same storage identity even when the function appears later in source. */
    for (size_t c = root_node->first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling) {
        const VntIrNode *node = &ir->nodes[c];
        if (node->opcode == VNT_IR_VARIABLE_DECL) {
            if (!resolver_declare(&resolver, node->value.text)) {
                resolver.error = 1;
                break;
            }
        }
    }
    const size_t global_count = resolver.count;

    for (size_t c = root_node->first_child; c != VNT_IR_NO_NODE && !resolver.error;
         c = ir->nodes[c].next_sibling) {
        VntIrNode *node = &ir->nodes[c];
        if (node->opcode == VNT_IR_FUNCTION) {
            size_t function_base = resolver.count;
            resolver.depth = 1;
            for (size_t p = 0; p < node->name_count && !resolver.error; ++p) {
                HirSymbol *parameter = resolver_declare(&resolver, node->names[p]);
                if (!parameter || !resolver_rename(&node->names[p], parameter->storage_key))
                    resolver_error(&resolver);
            }
            resolver_bind_role(&resolver, c, VNT_IR_EDGE_BODY);
            while (resolver.count > function_base) {
                free(resolver.symbols[resolver.count - 1].source_name);
                free(resolver.symbols[resolver.count - 1].storage_key);
                resolver.count--;
            }
            resolver.depth = 0;
        } else if (node->opcode == VNT_IR_VARIABLE_DECL) {
            HirSymbol *global = NULL;
            /* Globals occupy the first global_count entries in source order. */
            for (size_t g = 0; g < global_count; ++g) {
                if (!strcmp(resolver.symbols[g].source_name, node->value.text)) {
                    global = &resolver.symbols[g];
                    break;
                }
            }
            if (!global || !resolver_rename(&node->value.text, global->storage_key))
                resolver_error(&resolver);
            resolver_bind_expr(&resolver,
                hir_child_role(ir, c, VNT_IR_EDGE_VALUE));
        } else {
            resolver_bind_expr(&resolver, c);
        }
    }

    for (size_t i = 0; i < resolver.count; ++i) {
        free(resolver.symbols[i].source_name);
        free(resolver.symbols[i].storage_key);
    }
    free(resolver.symbols);
    return !resolver.error;
}

static int build_hir(VntIrProgram *ir, AstNode *program) {
    clear_nodes(ir);
    if (!program || program->type != AST_PROGRAM) return 0;
    if (!lower_node(ir, program, &ir->root)) {
        clear_nodes(ir);
        return 0;
    }
    if (!resolve_hir_symbols(ir)) {
        clear_nodes(ir);
        return 0;
    }
    return vnt_ir_validate(ir);
}

int vnt_ir_lower(VntIrProgram *ir, AstNode *program) {
    if (!ir || !program || program->type != AST_PROGRAM) return 0;
    memset(ir, 0, sizeof(*ir));
    ir->root = VNT_IR_NO_NODE;
    if (!build_hir(ir, program)) {
        vnt_ir_free(ir);
        return 0;
    }
    return 1;
}


#ifndef VNT_IR_H
#define VNT_IR_H

#include <stddef.h>
#include <stdio.h>
#include <vnt/ast.h>

/*
 * VNT HIR: a flat, index-addressed tree independent of AST allocation.
 * Nodes are stored in one growable array; child/sibling links are stable
 * integer indices, never pointers into a reallocatable array.
 *
 * source is retained only for diagnostics and the transitional x86 backend.
 * Code generation will migrate from source AST nodes to HIR instructions.
 */
typedef enum {
    VNT_IR_PROGRAM,
    VNT_IR_PRINT,
    VNT_IR_IF,
    VNT_IR_WHILE,
    VNT_IR_FUNCTION,
    VNT_IR_STRUCT,
    VNT_IR_CALL,
    VNT_IR_RETURN,
    VNT_IR_BREAK,
    VNT_IR_CONTINUE,
    VNT_IR_STRING,
    VNT_IR_INTEGER,
    VNT_IR_FLOAT,
    VNT_IR_BOOLEAN,
    VNT_IR_ARRAY,
    VNT_IR_VARIABLE_DECL,
    VNT_IR_VARIABLE,
    VNT_IR_INDEX,
    VNT_IR_MEMBER,
    VNT_IR_ASSIGN,
    VNT_IR_BINARY,
    VNT_IR_UNARY
} VntIrOpcode;

#define VNT_IR_NO_NODE ((size_t)-1)

typedef enum {
    VNT_IR_EDGE_ROOT,
    VNT_IR_EDGE_STATEMENT,
    VNT_IR_EDGE_CONDITION,
    VNT_IR_EDGE_THEN,
    VNT_IR_EDGE_ELSE,
    VNT_IR_EDGE_BODY,
    VNT_IR_EDGE_VALUE,
    VNT_IR_EDGE_TARGET,
    VNT_IR_EDGE_ARGUMENT,
    VNT_IR_EDGE_ELEMENT,
    VNT_IR_EDGE_OBJECT,
    VNT_IR_EDGE_INDEX,
    VNT_IR_EDGE_LEFT,
    VNT_IR_EDGE_RIGHT,
    VNT_IR_EDGE_OPERAND
} VntIrEdgeRole;

typedef struct {
    VntIrOpcode opcode;
    VntIrEdgeRole role;
    int operation;
    char **names;
    size_t name_count;
    size_t first_child;
    size_t last_child;
    size_t next_sibling;
    size_t child_count;
    const AstNode *source;
    union {
        int integer;
        int boolean;
        double floating;
        char *text;
    } value;
} VntIrNode;

typedef struct {
    /* Source tree kept for source mapping and the current x86 backend. */
    AstNode *program;
    VntIrNode *nodes;
    size_t node_count;
    size_t node_capacity;
    size_t root;
    size_t optimized_nodes;
} VntIrProgram;

int vnt_ir_lower(VntIrProgram *ir, AstNode *program);
int vnt_ir_optimize(VntIrProgram *ir);
int vnt_ir_validate(const VntIrProgram *ir);
void vnt_ir_dump(const VntIrProgram *ir, FILE *out);
void vnt_ir_free(VntIrProgram *ir);

#endif

#include <vnt/ir.h>

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int failures;

#define CHECK(condition, message) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL: %s (line %d)\n", message, __LINE__); \
        ++failures; \
    } \
} while (0)

static AstNode *binary(int left, int right, BinaryOperator op) {
    return ast_create_binary(ast_create_integer(left), ast_create_integer(right), op);
}

static AstNode *program_with(AstNode *expression) {
    return ast_create_program(ast_create_print(expression));
}

static size_t find_role(const VntIrProgram *ir, size_t parent, VntIrEdgeRole role) {
    for (size_t child = ir->nodes[parent].first_child;
         child != VNT_IR_NO_NODE;
         child = ir->nodes[child].next_sibling) {
        if (ir->nodes[child].role == role) return child;
    }
    return VNT_IR_NO_NODE;
}

static int check_fold_case(AstNode *expression, VntIrOpcode expected_opcode,
                           int expected_integer, const char *label) {
    VntIrProgram ir = {0};
    AstNode *program = program_with(expression);
    if (!program) {
        fprintf(stderr, "FAIL: could not allocate AST for %s\n", label);
        return 0;
    }
    int ok = vnt_ir_lower(&ir, program);
    CHECK(ok, "lowering succeeds");
    if (!ok) {
        ast_free(program);
        vnt_ir_free(&ir);
        return 0;
    }

    ok = vnt_ir_optimize(&ir);
    CHECK(ok, "optimization succeeds");
    size_t statement = find_role(&ir, ir.root, VNT_IR_EDGE_STATEMENT);
    size_t value = statement == VNT_IR_NO_NODE ? VNT_IR_NO_NODE
        : find_role(&ir, statement, VNT_IR_EDGE_VALUE);
    CHECK(value != VNT_IR_NO_NODE, "print expression exists after optimization");
    if (value != VNT_IR_NO_NODE) {
        CHECK(ir.nodes[value].opcode == expected_opcode, label);
        if (expected_opcode == VNT_IR_INTEGER)
            CHECK(ir.nodes[value].value.integer == expected_integer,
                  "folded integer has expected value");
    }

    /* Prove validation and materialization do not read source-AST pointers. */
    for (size_t i = 0; i < ir.node_count; ++i) ir.nodes[i].source = NULL;
    ir.program = NULL;
    ast_free(program);
    CHECK(vnt_ir_validate(&ir), "HIR validates after AST is detached and freed");

    AstNode *rebuilt = vnt_ir_materialize_program(&ir);
    CHECK(rebuilt != NULL, "HIR materializes after source AST is freed");
    if (rebuilt) ast_free(rebuilt);
    vnt_ir_free(&ir);
    return 1;
}

static void check_rejects_missing_text_payload(void) {
    AstNode *program = program_with(ast_create_string("payload"));
    VntIrProgram ir = {0};
    CHECK(vnt_ir_lower(&ir, program), "string test lowers");
    if (ir.nodes) {
        for (size_t i = 0; i < ir.node_count; ++i) {
            if (ir.nodes[i].opcode == VNT_IR_STRING) {
                char *owned_text = ir.nodes[i].value.text;
                ir.nodes[i].value.text = NULL;
                CHECK(!vnt_ir_validate(&ir), "validation rejects a string node without text");
                ir.nodes[i].value.text = owned_text;
                break;
            }
        }
    }
    vnt_ir_free(&ir);
    ast_free(program);
}

int main(void) {
    check_fold_case(binary(2, 3, BINARY_ADD), VNT_IR_INTEGER, 5,
                    "safe integer addition folds");
    check_fold_case(binary(INT_MAX, 1, BINARY_ADD), VNT_IR_BINARY, 0,
                    "integer overflow is not folded");
    check_fold_case(binary(1, 0, BINARY_DIVIDE), VNT_IR_BINARY, 0,
                    "integer division by zero is not folded");
    check_fold_case(binary(INT_MIN, -1, BINARY_DIVIDE), VNT_IR_BINARY, 0,
                    "signed division overflow is not folded");
    check_fold_case(ast_create_binary(ast_create_float(1.0), ast_create_float(0.0),
                                      BINARY_DIVIDE),
                    VNT_IR_BINARY, 0, "floating division by zero is not folded");
    check_fold_case(ast_create_binary(ast_create_float(DBL_MAX), ast_create_float(DBL_MAX),
                                      BINARY_MULTIPLY),
                    VNT_IR_BINARY, 0, "non-finite floating result is not folded");
    check_rejects_missing_text_payload();

    if (failures) {
        fprintf(stderr, "%d HIR invariant test(s) failed.\n", failures);
        return 1;
    }
    puts("HIR invariants: PASS");
    return 0;
}

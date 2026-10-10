#include <vnt/ir.h>
#include <vnt/cfg.h>
#include <vnt/ir_lower.h>

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    /* HIR owns its payloads and stores no pointers into the source AST. */
    ast_free(program);
 
    ok = vnt_ir_optimize(&ir);
    CHECK(ok, "optimization succeeds without the source AST");
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

    CHECK(vnt_ir_validate(&ir), "HIR validates after AST is freed");
    VntCfg cfg = {0};
    CHECK(vnt_cfg_build(&ir, &cfg), "CFG builds from optimized HIR");
    CHECK(vnt_cfg_validate(&cfg), "CFG validates after HIR optimization");
    vnt_cfg_free(&cfg);

    FILE *dump = tmpfile();
    CHECK(dump != NULL, "HIR dump stream can be created after AST destruction");
    if (dump) {
        vnt_ir_dump(&ir, dump);
        fclose(dump);
    }
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

static void check_rejects_disconnected_cycle(void) {
    AstNode *program = program_with(ast_create_integer(1));
    VntIrProgram ir = {0};
    CHECK(vnt_ir_lower(&ir, program), "cycle test lowers");
    if (!ir.nodes) { ast_free(program); return; }

    size_t old_count = ir.node_count;
    size_t needed = old_count + 2;
    if (ir.node_capacity < needed) {
        VntIrNode *nodes = realloc(ir.nodes, needed * sizeof(*nodes));
        if (!nodes) {
            CHECK(0, "cycle test allocates nodes");
            vnt_ir_free(&ir);
            ast_free(program);
            return;
        }
        ir.nodes = nodes;
        ir.node_capacity = needed;
    }
    memset(&ir.nodes[old_count], 0, 2 * sizeof(*ir.nodes));
    for (size_t i = old_count; i < needed; ++i) {
        ir.nodes[i].opcode = VNT_IR_PROGRAM;
        ir.nodes[i].role = VNT_IR_EDGE_STATEMENT;
        ir.nodes[i].first_child = ir.nodes[i].last_child =
            ir.nodes[i].next_sibling = VNT_IR_NO_NODE;
    }
    ir.node_count = needed;
    ir.nodes[old_count].first_child = ir.nodes[old_count].last_child = old_count + 1;
    ir.nodes[old_count].child_count = 1;
    ir.nodes[old_count + 1].first_child = ir.nodes[old_count + 1].last_child = old_count;
    ir.nodes[old_count + 1].child_count = 1;
    CHECK(!vnt_ir_validate(&ir), "validation rejects disconnected cycles");
    vnt_ir_free(&ir);
    ast_free(program);
}


static void check_cfg_control_flow(void) {
    AstNode *statements = NULL;
    ast_append(&statements,
        ast_create_if(ast_create_boolean(1),
                      ast_create_print(ast_create_integer(10)),
                      ast_create_print(ast_create_integer(20))));
    ast_append(&statements,
        ast_create_while(ast_create_boolean(1), ast_create_break()));
    ast_append(&statements,
        ast_create_function_declaration("cfg_function", NULL, 0,
                                        ast_create_return(ast_create_integer(7))));
    AstNode *program = ast_create_program(statements);
    VntIrProgram ir = {0};
    CHECK(program != NULL, "CFG control-flow AST allocated");
    if (!program) return;
    CHECK(vnt_ir_lower(&ir, program), "CFG control-flow HIR lowering succeeds");
    if (ir.nodes) {
        VntCfg cfg = {0};
        CHECK(vnt_cfg_build(&ir, &cfg), "CFG handles if, while, break, and function bodies");
        CHECK(vnt_cfg_validate(&cfg), "control-flow graph has valid edges");
        CHECK(cfg.entry_count == 2, "main and function have separate CFG entries");
        vnt_cfg_free(&cfg);
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
    check_rejects_disconnected_cycle();
    check_cfg_control_flow();

    if (failures) {
        fprintf(stderr, "%d HIR invariant test(s) failed.\n", failures);
        return 1;
    }
    puts("HIR invariants: PASS");
    return 0;
}

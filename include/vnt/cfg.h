#ifndef VNT_CFG_H
#define VNT_CFG_H

#include <stddef.h>
#include <vnt/ir.h>

typedef enum {
    VNT_CFG_FALLTHROUGH,
    VNT_CFG_BRANCH,
    VNT_CFG_JUMP,
    VNT_CFG_RETURN,
    VNT_CFG_EXIT
} VntCfgTerminator;

typedef struct {
    size_t instruction; /* HIR node for a statement, or VNT_IR_NO_NODE. */
    size_t condition;   /* HIR expression for a branch, or VNT_IR_NO_NODE. */
    size_t true_successor;
    size_t false_successor;
    VntCfgTerminator terminator;
} VntCfgBlock;

typedef struct {
    VntCfgBlock *blocks;
    size_t block_count;
    size_t block_capacity;
    size_t *entries; /* Main program followed by each function body. */
    size_t entry_count;
    size_t exit_block;
} VntCfg;

int vnt_cfg_build(const VntIrProgram *ir, VntCfg *cfg);
int vnt_cfg_validate(const VntCfg *cfg);
void vnt_cfg_free(VntCfg *cfg);

#endif

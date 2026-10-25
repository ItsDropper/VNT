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
/* Forward must-analysis: rejects reads not initialized on every incoming path. */
int vnt_cfg_check_definite_assignment(const VntIrProgram *ir, const VntCfg *cfg,
                                      char *diagnostic, size_t diagnostic_capacity);
/* Backward may-analysis used by later dead-store elimination and register allocation.
   names point into the supplied HIR and remain valid only while it is alive. */
typedef struct {
    const char **names;
    size_t variable_count;
    size_t block_count;
    unsigned char *live_in;  /* row-major [block_count][variable_count], bytes are 0/1 */
    unsigned char *live_out;
} VntCfgLiveness;

int vnt_cfg_compute_liveness(const VntIrProgram *ir, const VntCfg *cfg,
                             VntCfgLiveness *result);
int vnt_cfg_variable_live_in(const VntCfgLiveness *analysis, size_t block,
                             const char *name);
int vnt_cfg_variable_live_out(const VntCfgLiveness *analysis, size_t block,
                              const char *name);
void vnt_cfg_liveness_free(VntCfgLiveness *analysis);

/* Backward may-analysis for later dead-store elimination and register allocation. Names are borrowed from HIR. */
typedef struct {
    const char **names;
    size_t variable_count;
    size_t block_count;
    unsigned char *live_in;  /* row-major [block_count][variable_count], bytes are 0/1 */
    unsigned char *live_out;
} VntCfgLiveness;

int vnt_cfg_compute_liveness(const VntIrProgram *ir, const VntCfg *cfg, VntCfgLiveness *result);
int vnt_cfg_variable_live_in(const VntCfgLiveness *analysis, size_t block, const char *name);
int vnt_cfg_variable_live_out(const VntCfgLiveness *analysis, size_t block, const char *name);
void vnt_cfg_liveness_free(VntCfgLiveness *analysis);

void vnt_cfg_free(VntCfg *cfg);

#endif

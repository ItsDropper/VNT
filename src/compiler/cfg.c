#include <vnt/cfg.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const VntIrProgram *ir;
    VntCfg *cfg;
    int failed;
} Builder;

static size_t child_role(const VntIrProgram *ir, size_t parent, VntIrEdgeRole role) {
    if (parent >= ir->node_count) return VNT_IR_NO_NODE;
    for (size_t c = ir->nodes[parent].first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        if (ir->nodes[c].role == role) return c;
    return VNT_IR_NO_NODE;
}

static size_t new_block(Builder *b, VntCfgTerminator term, size_t instruction,
                        size_t condition, size_t yes, size_t no) {
    VntCfg *cfg = b->cfg;
    if (cfg->block_count == cfg->block_capacity) {
        size_t cap = cfg->block_capacity ? cfg->block_capacity * 2 : 32;
        if (cap < cfg->block_capacity || cap > (size_t)-1 / sizeof(*cfg->blocks)) {
            b->failed = 1;
            return VNT_IR_NO_NODE;
        }
        VntCfgBlock *blocks = realloc(cfg->blocks, cap * sizeof(*blocks));
        if (!blocks) { b->failed = 1; return VNT_IR_NO_NODE; }
        cfg->blocks = blocks;
        cfg->block_capacity = cap;
    }
    size_t index = cfg->block_count++;
    cfg->blocks[index] = (VntCfgBlock){instruction, condition, yes, no, term};
    return index;
}

static int add_entry(VntCfg *cfg, size_t entry) {
    if (entry == VNT_IR_NO_NODE) return 0;
    size_t *entries = realloc(cfg->entries, (cfg->entry_count + 1) * sizeof(*entries));
    if (!entries) return 0;
    cfg->entries = entries;
    cfg->entries[cfg->entry_count++] = entry;
    return 1;
}

static size_t build_sequence(Builder *b, size_t parent, VntIrEdgeRole role,
                             size_t next, size_t break_target, size_t continue_target,
                             int skip_definitions);

static size_t build_statement(Builder *b, size_t index, size_t next,
                              size_t break_target, size_t continue_target) {
    if (b->failed || index >= b->ir->node_count) return VNT_IR_NO_NODE;
    const VntIrNode *n = &b->ir->nodes[index];
    if (n->opcode == VNT_IR_PROGRAM)
        return build_sequence(b, index, VNT_IR_EDGE_STATEMENT, next,
                              break_target, continue_target, 0);
    if (n->opcode == VNT_IR_IF) {
        size_t cond = child_role(b->ir, index, VNT_IR_EDGE_CONDITION);
        if (cond == VNT_IR_NO_NODE) { b->failed = 1; return VNT_IR_NO_NODE; }
        size_t yes = build_sequence(b, index, VNT_IR_EDGE_THEN, next,
                                    break_target, continue_target, 0);
        size_t no = build_sequence(b, index, VNT_IR_EDGE_ELSE, next,
                                   break_target, continue_target, 0);
        return new_block(b, VNT_CFG_BRANCH, VNT_IR_NO_NODE, cond, yes, no);
    }
    if (n->opcode == VNT_IR_WHILE) {
        size_t cond = child_role(b->ir, index, VNT_IR_EDGE_CONDITION);
        if (cond == VNT_IR_NO_NODE) { b->failed = 1; return VNT_IR_NO_NODE; }
        size_t test = new_block(b, VNT_CFG_BRANCH, VNT_IR_NO_NODE, cond,
                                VNT_IR_NO_NODE, next);
        if (test == VNT_IR_NO_NODE) return test;
        size_t body = build_sequence(b, index, VNT_IR_EDGE_BODY, test, next, test, 0);
        if (body == VNT_IR_NO_NODE) { b->failed = 1; return VNT_IR_NO_NODE; }
        b->cfg->blocks[test].true_successor = body;
        return test;
    }
    if (n->opcode == VNT_IR_RETURN)
        return new_block(b, VNT_CFG_RETURN, index, VNT_IR_NO_NODE,
                         b->cfg->exit_block, VNT_IR_NO_NODE);
    if (n->opcode == VNT_IR_BREAK) {
        if (break_target == VNT_IR_NO_NODE) { b->failed = 1; return VNT_IR_NO_NODE; }
        return new_block(b, VNT_CFG_JUMP, index, VNT_IR_NO_NODE,
                         break_target, VNT_IR_NO_NODE);
    }
    if (n->opcode == VNT_IR_CONTINUE) {
        if (continue_target == VNT_IR_NO_NODE) { b->failed = 1; return VNT_IR_NO_NODE; }
        return new_block(b, VNT_CFG_JUMP, index, VNT_IR_NO_NODE,
                         continue_target, VNT_IR_NO_NODE);
    }
    if (n->opcode == VNT_IR_FUNCTION || n->opcode == VNT_IR_STRUCT) {
        /* Definitions are separate roots, not executable top-level statements. */
        return next;
    }
    return new_block(b, VNT_CFG_FALLTHROUGH, index, VNT_IR_NO_NODE,
                     next, VNT_IR_NO_NODE);
}

static size_t build_sequence(Builder *b, size_t parent, VntIrEdgeRole role,
                             size_t next, size_t break_target, size_t continue_target,
                             int skip_definitions) {
    if (parent >= b->ir->node_count) { b->failed = 1; return VNT_IR_NO_NODE; }
    size_t count = 0;
    for (size_t c = b->ir->nodes[parent].first_child; c != VNT_IR_NO_NODE;
         c = b->ir->nodes[c].next_sibling) {
        if (b->ir->nodes[c].role != role) continue;
        if (skip_definitions &&
            (b->ir->nodes[c].opcode == VNT_IR_FUNCTION ||
             b->ir->nodes[c].opcode == VNT_IR_STRUCT)) continue;
        ++count;
    }
    if (!count) return next;
    size_t *items = malloc(count * sizeof(*items));
    if (!items) { b->failed = 1; return VNT_IR_NO_NODE; }
    size_t at = 0;
    for (size_t c = b->ir->nodes[parent].first_child; c != VNT_IR_NO_NODE;
         c = b->ir->nodes[c].next_sibling) {
        if (b->ir->nodes[c].role != role) continue;
        if (skip_definitions &&
            (b->ir->nodes[c].opcode == VNT_IR_FUNCTION ||
             b->ir->nodes[c].opcode == VNT_IR_STRUCT)) continue;
        items[at++] = c;
    }
    size_t entry = next;
    while (count && !b->failed) entry = build_statement(b, items[--count], entry,
                                                         break_target, continue_target);
    free(items);
    return b->failed ? VNT_IR_NO_NODE : entry;
}

int vnt_cfg_build(const VntIrProgram *ir, VntCfg *cfg) {
    if (!ir || !cfg || !vnt_ir_validate(ir)) return 0;
    memset(cfg, 0, sizeof(*cfg));
    Builder b = {ir, cfg, 0};
    cfg->exit_block = new_block(&b, VNT_CFG_EXIT, VNT_IR_NO_NODE,
                                VNT_IR_NO_NODE, VNT_IR_NO_NODE, VNT_IR_NO_NODE);
    if (cfg->exit_block == VNT_IR_NO_NODE) goto fail;

    size_t main_entry = build_sequence(&b, ir->root, VNT_IR_EDGE_STATEMENT,
                                       cfg->exit_block, VNT_IR_NO_NODE,
                                       VNT_IR_NO_NODE, 1);
    if (b.failed || !add_entry(cfg, main_entry)) goto fail;

    for (size_t i = 0; i < ir->node_count; ++i) {
        if (ir->nodes[i].opcode != VNT_IR_FUNCTION) continue;
        size_t entry = build_sequence(&b, i, VNT_IR_EDGE_BODY, cfg->exit_block,
                                      VNT_IR_NO_NODE, VNT_IR_NO_NODE, 0);
        if (b.failed || !add_entry(cfg, entry)) goto fail;
    }
    if (!vnt_cfg_validate(cfg)) goto fail;
    return 1;

fail:
    vnt_cfg_free(cfg);
    return 0;
}

int vnt_cfg_validate(const VntCfg *cfg) {
    if (!cfg || !cfg->blocks || !cfg->block_count ||
        cfg->exit_block >= cfg->block_count || !cfg->entry_count || !cfg->entries)
        return 0;
    for (size_t i = 0; i < cfg->entry_count; ++i)
        if (cfg->entries[i] >= cfg->block_count) return 0;
    for (size_t i = 0; i < cfg->block_count; ++i) {
        const VntCfgBlock *b = &cfg->blocks[i];
        if ((unsigned)b->terminator > VNT_CFG_EXIT) return 0;
        if (b->true_successor != VNT_IR_NO_NODE &&
            b->true_successor >= cfg->block_count) return 0;
        if (b->false_successor != VNT_IR_NO_NODE &&
            b->false_successor >= cfg->block_count) return 0;
        if (b->terminator == VNT_CFG_BRANCH) {
            if (b->condition == VNT_IR_NO_NODE ||
                b->true_successor == VNT_IR_NO_NODE ||
                b->false_successor == VNT_IR_NO_NODE ||
                b->instruction != VNT_IR_NO_NODE) return 0;
        } else if (b->terminator == VNT_CFG_EXIT) {
            if (i != cfg->exit_block || b->true_successor != VNT_IR_NO_NODE ||
                b->false_successor != VNT_IR_NO_NODE) return 0;
        } else if (b->terminator == VNT_CFG_RETURN) {
            if (b->instruction == VNT_IR_NO_NODE ||
                b->true_successor != cfg->exit_block ||
                b->false_successor != VNT_IR_NO_NODE) return 0;
        } else {
            if (b->true_successor == VNT_IR_NO_NODE ||
                b->false_successor != VNT_IR_NO_NODE ||
                b->condition != VNT_IR_NO_NODE) return 0;
        }
    }
    return 1;
}

void vnt_cfg_free(VntCfg *cfg) {
    if (!cfg) return;
    free(cfg->blocks);
    free(cfg->entries);
    memset(cfg, 0, sizeof(*cfg));
}

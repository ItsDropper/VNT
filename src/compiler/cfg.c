#include <vnt/cfg.h>
#include <stdlib.h>
#include <stdio.h>
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

/* Remove blocks that cannot be reached from main or any function entry.
   This also removes CFG nodes after unconditional return/break/continue. */
static int prune_unreachable_blocks(VntCfg *cfg) {
    size_t count = cfg->block_count;
    unsigned char *reachable = calloc(count, sizeof(*reachable));
    size_t *stack = malloc(count * sizeof(*stack));
    size_t *map = malloc(count * sizeof(*map));
    if (!reachable || !stack || !map) {
        free(reachable); free(stack); free(map);
        return 0;
    }

    size_t top = 0;
    for (size_t i = 0; i < cfg->entry_count; ++i) {
        size_t entry = cfg->entries[i];
        if (entry >= count) { free(reachable); free(stack); free(map); return 0; }
        if (!reachable[entry]) {
            reachable[entry] = 1;
            stack[top++] = entry;
        }
    }
    while (top) {
        size_t at = stack[--top];
        const VntCfgBlock *block = &cfg->blocks[at];
        size_t successors[2] = {block->true_successor, block->false_successor};
        for (size_t i = 0; i < 2; ++i) {
            size_t next = successors[i];
            if (next == VNT_IR_NO_NODE) continue;
            if (next >= count) {
                free(reachable); free(stack); free(map);
                return 0;
            }
            if (!reachable[next]) {
                reachable[next] = 1;
                stack[top++] = next;
            }
        }
    }

    size_t kept = 0;
    for (size_t i = 0; i < count; ++i) {
        map[i] = reachable[i] ? kept++ : VNT_IR_NO_NODE;
    }
    if (!kept || map[cfg->exit_block] == VNT_IR_NO_NODE) {
        free(reachable); free(stack); free(map);
        return 0;
    }
    VntCfgBlock *blocks = malloc(kept * sizeof(*blocks));
    if (!blocks) {
        free(reachable); free(stack); free(map);
        return 0;
    }
    for (size_t old = 0; old < count; ++old) {
        if (!reachable[old]) continue;
        VntCfgBlock block = cfg->blocks[old];
        if (block.true_successor != VNT_IR_NO_NODE)
            block.true_successor = map[block.true_successor];
        if (block.false_successor != VNT_IR_NO_NODE)
            block.false_successor = map[block.false_successor];
        blocks[map[old]] = block;
    }
    for (size_t i = 0; i < cfg->entry_count; ++i)
        cfg->entries[i] = map[cfg->entries[i]];
    cfg->exit_block = map[cfg->exit_block];
    free(cfg->blocks);
    cfg->blocks = blocks;
    cfg->block_count = cfg->block_capacity = kept;

    free(reachable); free(stack); free(map);
    return 1;
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
    if (!prune_unreachable_blocks(cfg) || !vnt_cfg_validate(cfg)) goto fail;
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


typedef struct {
    const char **names;
    size_t count, capacity;
} VariableSet;

static int variable_set_add(VariableSet *set, const char *name) {
    if (!name) return 1;
    for (size_t i = 0; i < set->count; ++i)
        if (!strcmp(set->names[i], name)) return 1;
    if (set->count == set->capacity) {
        size_t cap = set->capacity ? set->capacity * 2 : 32;
        if (cap < set->capacity || cap > (size_t)-1 / sizeof(*set->names))
            return 0;
        const char **names = realloc(set->names, cap * sizeof(*names));
        if (!names) return 0;
        set->names = names;
        set->capacity = cap;
    }
    set->names[set->count++] = name;
    return 1;
}

static size_t variable_index(const VariableSet *set, const char *name) {
    if (!name) return VNT_IR_NO_NODE;
    for (size_t i = 0; i < set->count; ++i)
        if (!strcmp(set->names[i], name)) return i;
    return VNT_IR_NO_NODE;
}

static int collect_variables(const VntIrProgram *ir, VariableSet *set) {
    for (size_t i = 0; i < ir->node_count; ++i) {
        const VntIrNode *n = &ir->nodes[i];
        if ((n->opcode == VNT_IR_VARIABLE ||
             n->opcode == VNT_IR_VARIABLE_DECL ||
             n->opcode == VNT_IR_REASSIGN) &&
            !variable_set_add(set, n->value.text)) return 0;
        if (n->opcode == VNT_IR_FUNCTION) {
            for (size_t p = 0; p < n->name_count; ++p)
                if (!variable_set_add(set, n->names[p])) return 0;
        }
    }
    return 1;
}

static void mark_variable(unsigned char *bits, const VariableSet *set,
                          const char *name) {
    size_t index = variable_index(set, name);
    if (index != VNT_IR_NO_NODE) bits[index] = 1;
}

static void collect_expression_reads(const VntIrProgram *ir, size_t node,
                                    const VariableSet *set, unsigned char *uses) {
    if (node == VNT_IR_NO_NODE || node >= ir->node_count) return;
    const VntIrNode *n = &ir->nodes[node];
    if (n->opcode == VNT_IR_VARIABLE) {
        mark_variable(uses, set, n->value.text);
        return;
    }
    for (size_t c = n->first_child; c != VNT_IR_NO_NODE;
         c = ir->nodes[c].next_sibling)
        collect_expression_reads(ir, c, set, uses);
}

static void collect_block_effects(const VntIrProgram *ir, const VntCfgBlock *block,
                                  const VariableSet *set, unsigned char *uses,
                                  unsigned char *defs) {
    if (block->condition != VNT_IR_NO_NODE)
        collect_expression_reads(ir, block->condition, set, uses);
    if (block->instruction == VNT_IR_NO_NODE) return;

    const VntIrNode *n = &ir->nodes[block->instruction];
    switch (n->opcode) {
        case VNT_IR_VARIABLE_DECL:
        case VNT_IR_REASSIGN:
            collect_expression_reads(ir,
                child_role(ir, block->instruction, VNT_IR_EDGE_VALUE), set, uses);
            mark_variable(defs, set, n->value.text);
            break;
        case VNT_IR_PRINT:
        case VNT_IR_RETURN:
            collect_expression_reads(ir,
                child_role(ir, block->instruction, VNT_IR_EDGE_VALUE), set, uses);
            break;
        case VNT_IR_ASSIGN: {
            size_t target = child_role(ir, block->instruction, VNT_IR_EDGE_TARGET);
            if (target != VNT_IR_NO_NODE &&
                ir->nodes[target].opcode == VNT_IR_VARIABLE)
                mark_variable(defs, set, ir->nodes[target].value.text);
            else
                collect_expression_reads(ir, target, set, uses);
            collect_expression_reads(ir,
                child_role(ir, block->instruction, VNT_IR_EDGE_VALUE), set, uses);
            break;
        }
        default:
            collect_expression_reads(ir, block->instruction, set, uses);
            break;
    }
}

static const char *source_name_for_diagnostic(const char *key) {
    static const char marker[] = "__vnt_symbol_";
    if (!key) return "<unknown>";
    const char *prefix = strstr(key, marker);
    if (!prefix) return key;
    const char *separator = strchr(prefix + sizeof(marker) - 1, '_');
    return separator ? separator + 1 : key;
}

static int block_has_predecessor(const VntCfg *cfg, size_t predecessor,
                                 size_t target) {
    return cfg->blocks[predecessor].true_successor == target ||
           cfg->blocks[predecessor].false_successor == target;
}

int vnt_cfg_check_definite_assignment(const VntIrProgram *ir, const VntCfg *cfg,
                                      char *diagnostic, size_t diagnostic_capacity) {
    if (diagnostic && diagnostic_capacity) diagnostic[0] = '\0';
    if (!ir || !cfg || !vnt_ir_validate(ir) || !vnt_cfg_validate(cfg)) {
        if (diagnostic && diagnostic_capacity)
            snprintf(diagnostic, diagnostic_capacity, "invalid HIR or CFG for dataflow analysis");
        return 0;
    }

    VariableSet variables = {0};
    if (!collect_variables(ir, &variables)) goto allocation_failure;
    size_t blocks = cfg->block_count, vars = variables.count;
    if (vars && blocks > (size_t)-1 / vars) goto allocation_failure;
    size_t cells = blocks * vars;
    size_t allocation_cells = cells ? cells : 1;
    unsigned char *uses = calloc(allocation_cells, 1);
    unsigned char *defs = calloc(allocation_cells, 1);
    unsigned char *in = malloc(allocation_cells);
    unsigned char *out = malloc(allocation_cells);
    unsigned char *seed = calloc(allocation_cells, 1);
    unsigned char *is_entry = calloc(blocks ? blocks : 1, 1);
    unsigned char *globals = calloc(vars ? vars : 1, 1);
    if (!uses || !defs || !in || !out || !seed || !is_entry || !globals) {
        free(uses); free(defs); free(in); free(out); free(seed);
        free(is_entry); free(globals);
        goto allocation_failure;
    }

    for (size_t i = 0; i < blocks; ++i)
        collect_block_effects(ir, &cfg->blocks[i], &variables,
                              uses + i * vars, defs + i * vars);

    /* Root-level declarations are globals. They are initialized by the main
       entry before ordinary calls; function entries may therefore read them. */
    if (ir->root < ir->node_count) {
        for (size_t c = ir->nodes[ir->root].first_child; c != VNT_IR_NO_NODE;
             c = ir->nodes[c].next_sibling) {
            const VntIrNode *n = &ir->nodes[c];
            if (n->role == VNT_IR_EDGE_STATEMENT &&
                n->opcode == VNT_IR_VARIABLE_DECL)
                mark_variable(globals, &variables, n->value.text);
        }
    }

    for (size_t e = 0; e < cfg->entry_count; ++e) {
        size_t entry = cfg->entries[e];
        is_entry[entry] = 1;
        unsigned char *entry_seed = seed + entry * vars;
        if (e > 0 && vars) memcpy(entry_seed, globals, vars);
    }

    size_t function_entry = 1;
    for (size_t i = 0; i < ir->node_count && function_entry < cfg->entry_count; ++i) {
        const VntIrNode *n = &ir->nodes[i];
        if (n->opcode != VNT_IR_FUNCTION) continue;
        unsigned char *entry_seed = seed + cfg->entries[function_entry] * vars;
        for (size_t p = 0; p < n->name_count; ++p)
            mark_variable(entry_seed, &variables, n->names[p]);
        ++function_entry;
    }

    /* Must-analysis starts at the lattice top for non-entry blocks. */
    if (cells) {
        memset(in, 1, cells);
        memset(out, 1, cells);
    }
    int changed;
    do {
        changed = 0;
        for (size_t b = 0; b < blocks; ++b) {
            unsigned char *bin = in + b * vars;
            unsigned char *bout = out + b * vars;
            if (is_entry[b]) {
                if (vars && memcmp(bin, seed + b * vars, vars) != 0) {
                    memcpy(bin, seed + b * vars, vars);
                    changed = 1;
                }
            } else {
                int has_predecessor = 0;
                for (size_t v = 0; v < vars; ++v) bin[v] = 1;
                for (size_t p = 0; p < blocks; ++p) {
                    if (!block_has_predecessor(cfg, p, b)) continue;
                    const unsigned char *pout = out + p * vars;
                    if (!has_predecessor) {
                        if (vars) memcpy(bin, pout, vars);
                        has_predecessor = 1;
                    } else {
                        for (size_t v = 0; v < vars; ++v) bin[v] &= pout[v];
                    }
                }
                if (!has_predecessor && vars) memset(bin, 0, vars);
            }
            for (size_t v = 0; v < vars; ++v) {
                unsigned char value = (unsigned char)(bin[v] || defs[b * vars + v]);
                if (bout[v] != value) {
                    bout[v] = value;
                    changed = 1;
                }
            }
        }
    } while (changed);

    for (size_t b = 0; b < blocks; ++b) {
        const unsigned char *bin = in + b * vars;
        const unsigned char *buses = uses + b * vars;
        for (size_t v = 0; v < vars; ++v) {
            if (buses[v] && !bin[v]) {
                if (diagnostic && diagnostic_capacity)
                    snprintf(diagnostic, diagnostic_capacity,
                             "variable '%s' may be read before initialization",
                             source_name_for_diagnostic(variables.names[v]));
                free(uses); free(defs); free(in); free(out); free(seed);
                free(is_entry); free(globals); free(variables.names);
                return 0;
            }
        }
    }

    free(uses); free(defs); free(in); free(out); free(seed);
    free(is_entry); free(globals); free(variables.names);
    return 1;

allocation_failure:
    free(variables.names);
    if (diagnostic && diagnostic_capacity)
        snprintf(diagnostic, diagnostic_capacity, "out of memory during CFG dataflow analysis");
    return 0;
}


void vnt_cfg_free(VntCfg *cfg) {
    if (!cfg) return;
    free(cfg->blocks);
    free(cfg->entries);
    memset(cfg, 0, sizeof(*cfg));
}

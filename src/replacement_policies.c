#include "replacement_policies.h"
#include "memory_system.h"
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

// Data struct for LRU and LRU_PREFER_CLEAN policies
typedef struct {
    uint32_t sets;
    uint32_t assoc;
    uint64_t *timestamps;
    uint64_t counter;
} lru_data;

// LRU Replacement Policy
void lru_cache_access(struct replacement_policy *rp, struct cache_system *cs, uint32_t set_idx,
                      uint32_t tag)
{
    lru_data *d = (lru_data *)rp->data;
    d->counter++;
    uint32_t assoc = d->assoc;
    uint32_t start = set_idx * assoc;
    for (uint32_t i = 0; i < assoc; i++) {
        struct cache_line *cl = &cs->cache_lines[start + i];
        if (cl->tag == tag && cl->status != INVALID) {
            d->timestamps[start + i] = d->counter;
            break;
        }
    }
}

uint32_t lru_eviction_index(struct replacement_policy *rp, struct cache_system *cs, uint32_t set_idx)
{
    lru_data *d = (lru_data *)rp->data;
    uint32_t assoc = d->assoc;
    uint32_t start = set_idx * assoc;
    uint64_t min_ts = UINT64_MAX;
    uint32_t evict = 0;
    for (uint32_t i = 0; i < assoc; i++) {
        if (d->timestamps[start + i] < min_ts) {
            min_ts = d->timestamps[start + i];
            evict = i;
        }
    }
    return evict;
}

void lru_replacement_policy_cleanup(struct replacement_policy *rp)
{
    lru_data *d = (lru_data *)rp->data;
    free(d->timestamps);
    free(d);
}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *rp = calloc(1, sizeof(struct replacement_policy));
    rp->cache_access = &lru_cache_access;
    rp->eviction_index = &lru_eviction_index;
    rp->cleanup = &lru_replacement_policy_cleanup;

    lru_data *d = malloc(sizeof(lru_data));
    d->sets = sets;
    d->assoc = associativity;
    d->counter = 0;
    d->timestamps = calloc(sets * associativity, sizeof(uint64_t));
    rp->data = d;
    return rp;
}

// RAND Replacement Policy
void rand_cache_access(struct replacement_policy *rp, struct cache_system *cs, uint32_t set_idx,
                       uint32_t tag)
{
    // No state to track
}

uint32_t rand_eviction_index(struct replacement_policy *rp, struct cache_system *cs, uint32_t set_idx)
{
    return rand() % cs->associativity;
}

void rand_replacement_policy_cleanup(struct replacement_policy *rp)
{
    // Nothing to cleanup
}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    srand(time(NULL));
    struct replacement_policy *rp = calloc(1, sizeof(struct replacement_policy));
    rp->cache_access = &rand_cache_access;
    rp->eviction_index = &rand_eviction_index;
    rp->cleanup = &rand_replacement_policy_cleanup;
    rp->data = NULL;
    return rp;
}

// LRU_PREFER_CLEAN Replacement Policy
void lru_prefer_clean_cache_access(struct replacement_policy *rp, struct cache_system *cs,
                                   uint32_t set_idx, uint32_t tag)
{
    // Same as LRU
    lru_cache_access(rp, cs, set_idx, tag);
}

uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *rp, struct cache_system *cs,
                                         uint32_t set_idx)
{
    lru_data *d = (lru_data *)rp->data;
    uint32_t assoc = d->assoc;
    uint32_t start = set_idx * assoc;
    uint64_t min_clean_ts = UINT64_MAX;
    int clean_idx = -1;
    // First prefer clean lines
    for (uint32_t i = 0; i < assoc; i++) {
        struct cache_line *cl = &cs->cache_lines[start + i];
        if (cl->status != MODIFIED && d->timestamps[start + i] < min_clean_ts) {
            min_clean_ts = d->timestamps[start + i];
            clean_idx = i;
        }
    }
    if (clean_idx >= 0) {
        return (uint32_t)clean_idx;
    }
    // Fallback: evict LRU among all
    uint64_t min_ts = UINT64_MAX;
    uint32_t evict = 0;
    for (uint32_t i = 0; i < assoc; i++) {
        if (d->timestamps[start + i] < min_ts) {
            min_ts = d->timestamps[start + i];
            evict = i;
        }
    }
    return evict;
}

void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *rp)
{
    lru_replacement_policy_cleanup(rp);
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *rp = lru_replacement_policy_new(sets, associativity);
    rp->cache_access = &lru_prefer_clean_cache_access;
    rp->eviction_index = &lru_prefer_clean_eviction_index;
    rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;
    return rp;
}

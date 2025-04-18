#include "prefetchers.h"
#include "memory_system.h"
#include <stdlib.h>
#include <stdint.h>

// Data struct for sequential prefetcher
struct sequential_data {
    uint32_t prefetch_amount;
};

// Null Prefetcher already implemented
uint32_t null_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                uint32_t address, bool is_miss)
{
    return 0;
}
void null_cleanup(struct prefetcher *prefetcher) {}

struct prefetcher *null_prefetcher_new()
{
    struct prefetcher *null_prefetcher = calloc(1, sizeof(struct prefetcher));
    null_prefetcher->handle_mem_access = &null_handle_mem_access;
    null_prefetcher->cleanup = &null_cleanup;
    null_prefetcher->data = NULL;
    return null_prefetcher;
}

// Sequential Prefetcher
uint32_t sequential_handle_mem_access(struct prefetcher *prefetcher,
                                      struct cache_system *cache_system, uint32_t address,
                                      bool is_miss)
{
    struct sequential_data *d = (struct sequential_data *)prefetcher->data;
    uint32_t count = 0;
    uint32_t line_size = cache_system->line_size;
    for (uint32_t i = 1; i <= d->prefetch_amount; i++) {
        uint32_t prefetch_addr = address + i * line_size;
        cache_system_mem_access(cache_system, prefetch_addr, 'R', true);
        count++;
    }
    return count;
}

void sequential_cleanup(struct prefetcher *prefetcher)
{
    free(prefetcher->data);
}

struct prefetcher *sequential_prefetcher_new(uint32_t prefetch_amount)
{
    struct prefetcher *sequential_prefetcher = calloc(1, sizeof(struct prefetcher));
    sequential_prefetcher->handle_mem_access = &sequential_handle_mem_access;
    sequential_prefetcher->cleanup = &sequential_cleanup;
    struct sequential_data *d = malloc(sizeof(struct sequential_data));
    d->prefetch_amount = prefetch_amount;
    sequential_prefetcher->data = d;
    return sequential_prefetcher;
}

// Adjacent Prefetcher
uint32_t adjacent_handle_mem_access(struct prefetcher *prefetcher,
                                    struct cache_system *cache_system, uint32_t address,
                                    bool is_miss)
{
    uint32_t line_size = cache_system->line_size;
    uint32_t prefetch_addr = address + line_size;
    cache_system_mem_access(cache_system, prefetch_addr, 'R', true);
    return 1;
}

void adjacent_cleanup(struct prefetcher *prefetcher) {}

struct prefetcher *adjacent_prefetcher_new()
{
    struct prefetcher *adjacent_prefetcher = calloc(1, sizeof(struct prefetcher));
    adjacent_prefetcher->handle_mem_access = &adjacent_handle_mem_access;
    adjacent_prefetcher->cleanup = &adjacent_cleanup;
    adjacent_prefetcher->data = NULL;
    return adjacent_prefetcher;
}

// Custom Prefetcher (simple next-line on miss)
uint32_t custom_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                  uint32_t address, bool is_miss)
{
    if (is_miss) {
        uint32_t line_size = cache_system->line_size;
        uint32_t prefetch_addr = address + line_size;
        cache_system_mem_access(cache_system, prefetch_addr, 'R', true);
        return 1;
    }
    return 0;
}

void custom_cleanup(struct prefetcher *prefetcher) {}

struct prefetcher *custom_prefetcher_new()
{
    struct prefetcher *custom_prefetcher = calloc(1, sizeof(struct prefetcher));
    custom_prefetcher->handle_mem_access = &custom_handle_mem_access;
    custom_prefetcher->cleanup = &custom_cleanup;
    custom_prefetcher->data = NULL;
    return custom_prefetcher;
}
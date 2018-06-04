#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "membase.h"
#include "memory.h"
#include "cache.h"


#define TESTMEM_SIZE 65536
#define NUM_WRITES 50000


/* Setting this to 1 will cause the program to output the details of
 * each write performed against the cached memory.
 */
#define DEBUG_TESTMEM 0


/* This program exercises the memory and the cache implementation by
 * performing a series of writes against a cached memory, then flushing
 * the cache, and then reading the contents of the memory directly to see
 * if the values properly reflect what they ought to be.
 */
int main() {
    cache_t cache;
    memory_t memory;
    unsigned char *p_raw;

    int i, count;

    p_raw = malloc(TESTMEM_SIZE);
    bzero(p_raw, TESTMEM_SIZE);

    init_memory(&memory, TESTMEM_SIZE);
    init_cache(&cache, /* block_size */ 64, /* num_sets */ 16,
        /* lines_per_set */ 64, (membase_t *) &memory);

    printf("Running test.\n");
    
    for (i = 0; i < NUM_WRITES; i++) {
        addr_t addr = rand() % TESTMEM_SIZE;
        unsigned char value = rand() % 256;

#if DEBUG_TESTMEM
        printf("Writing value %u to address %u\n", value, addr);
#endif

        p_raw[addr] = value;
        write_byte((membase_t *) &cache, addr, value);
    }

    flush_cache(&cache);

    count = 0;
    for (i = 0; i < TESTMEM_SIZE; i++) {
        if (p_raw[i] != memory.mem[i]) {
            count++;
            printf("Values at index %d don't match:  raw[i] = %u, mem[i] = %u\n",
                i, p_raw[i], memory.mem[i]);
        }
    }

    if (count == 0)
        printf("Memories are identical.\n");

    cache.free((membase_t *) &cache);
    memory.free((membase_t *) &memory);
    free(p_raw);

    return 0;
}


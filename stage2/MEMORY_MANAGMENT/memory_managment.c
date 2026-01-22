#include "memory_managment.h"


static struct mem_entry *table = (struct mem_entry *)TABLE_BASE;
static uint16_t heap_top = 0;
static uint8_t initialized = 0;

/* ---------- internal ---------- */

static void mem_init(void)
{
    for (uint16_t i = 0; i < MAX_ENTRIES; i++) {
        table[i].offset = 0xFFFF;
        table[i].size = 0;
    }
    heap_top = 0;
    initialized = 1;
}

/* ---------- malloc ---------- */

void *malloc(uint16_t size)
{
    if (!initialized)
        mem_init();

    if (size == 0)
        return 0;

    /* align to 2 bytes */
    if (size & 1)
        size++;

    for (uint16_t i = 0; i < MAX_ENTRIES; i++) {
        if (table[i].offset == 0xFFFF) {
            table[i].offset = heap_top;
            table[i].size = size;

            void *ptr = (void *)(HEAP_BASE + heap_top);
            heap_top += size;

            return ptr;
        }
    }

    return 0; /* out of metadata */
}

/* ---------- free ---------- */

void free(void *ptr)
{
    if (!ptr)
        return;

    uint16_t off = (uint16_t)((uintptr_t)ptr - HEAP_BASE);

    for (uint16_t i = 0; i < MAX_ENTRIES; i++) {
        if (table[i].offset == off) {
            table[i].offset = 0xFFFF;
            table[i].size = 0;
            return;
        }
    }
}

/* ---------- realloc ---------- */

void *realloc(void *src, uint16_t newSize)
{
    if (!src)
        return malloc(newSize);

    if (newSize == 0) {
        free(src);
        return 0;
    }

    uint16_t off = (uint16_t)((uintptr_t)src - HEAP_BASE);

    for (uint16_t i = 0; i < MAX_ENTRIES; i++) {
        if (table[i].offset == off) {

            uint16_t oldSize = table[i].size;

            if (newSize <= oldSize)
                return src;

            void *dst = malloc(newSize);
            if (!dst)
                return 0;

            uint8_t *d = dst;
            uint8_t *s = src;

            for (uint16_t j = 0; j < oldSize; j++)
                d[j] = s[j];

            free(src);
            return dst;
        }
    }

    return 0;
}

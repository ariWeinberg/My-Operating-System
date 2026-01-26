#include "memory_managment.h"
#include "../STD/defs.h"


static struct mem_entry *table = (struct mem_entry *)TABLE_BASE;
static uint16_t heap_top = 0;
static uint8_t initialized = 0;


static mem_error_t mem_last_error = MEM_OK;
mem_error_t mem_get_last_error(void)
{
    return mem_last_error;
}
static inline void mem_set_error(mem_error_t err)
{
    mem_last_error = err;
}


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
    {
        mem_set_error(MEM_ERR_ZERO_SIZE);
        return NULL;
    }

    /* align to 2 bytes */
    if (size & 1)
        size++;

    if((HEAP_BASE + heap_top + size) > HEAP_LIMIT)
    {
        mem_set_error(MEM_ERR_HEAP_EXHAUSTED);
        return NULL;
    }

    for (uint16_t i = 0; i < MAX_ENTRIES; i++) {
        if (table[i].offset == 0xFFFF) {
            table[i].offset = heap_top;
            table[i].size = size;

            void *ptr = (void *)(HEAP_BASE + heap_top);
            heap_top += size;

            mem_set_error(MEM_OK);
            return ptr;
        }
    }
    mem_set_error(MEM_ERR_NO_METADATA);
    return NULL; /* out of metadata */
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
            mem_set_error(MEM_OK);
            return;
        }
    }
    mem_set_error(MEM_ERR_INVALID_PTR);
}

/* ---------- realloc ---------- */

void *realloc(void *src, uint16_t newSize)
{
    if (!src)
        return malloc(newSize);

    if (newSize == 0) {
        free(src);
        return NULL;
    }

    uint16_t off = (uint16_t)((uintptr_t)src - HEAP_BASE);

    for (uint16_t i = 0; i < MAX_ENTRIES; i++) {
        if (table[i].offset == off) {

            uint16_t oldSize = table[i].size;

            if (newSize <= oldSize)
            {
                mem_set_error(MEM_OK);
                return src;
            }

            void *dst = malloc(newSize);
            if (!dst)
                return NULL;

            uint8_t *d = (uint8_t *)dst;
            uint8_t *s = (uint8_t *)src;

            for (uint16_t j = 0; j < oldSize; j++)
                d[j] = s[j];

            free(src);
            mem_set_error(MEM_OK);
            return dst;
        }
    }

    mem_set_error(MEM_ERR_REALLOC_NOT_FOUND);
    return NULL;
}

/* ---------- zalloc ---------- */

void *zalloc(uint16_t size)
{
    uint8_t *alloc = (uint8_t*)malloc(size);
    if(!alloc)
    {return NULL;}
    for (int i = 0; i < size; i++)
    {
        alloc[i] = 0;
    }
    return alloc;
}
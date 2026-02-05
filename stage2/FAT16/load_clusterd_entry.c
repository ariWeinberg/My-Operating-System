#include "fat16.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"


void *load_clustered_entry(Fat16 *fat, DirEntry *entry)
{
    ClusterChain *chain = traverse_chain(fat, entry->first_cluster_low);

    if (!chain)
    {
        fat16_fail(FAT16_CHAIN_TRAVERSAL_FAIL);
        return NULL;
    }

    uint32_t total_bytes = 
        chain->cluster_count *
        fat->sectors_per_cluster *
        fat->bytes_per_sector;
     
    DirEntry *dest = (DirEntry *)zalloc(total_bytes);
    if (!dest)
    {
        free(chain->chain);
        free(chain);
        fat16_fail(FAT16_MEMORY_ALLOCATION_FAIL);
        return NULL;
    }

    load_cluster_chain(fat, chain, dest);

    free(chain->chain);
    free(chain);
    fat16_ok();
    return dest;
}

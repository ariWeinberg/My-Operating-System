#include "fat16.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"

ClusterChain *traverse_chain(Fat16 *fat, uint16_t chain_start)
{   
    int i = 0;
    uint16_t current_cluster = chain_start;

    ClusterChain *chain = (ClusterChain*)zalloc(sizeof(ClusterChain));
    chain->cluster_count = 0;
    chain->chain = NULL;

    while (current_cluster < FAT_EOC_START && chain->cluster_count < (fat->fat_bytes / 2))
    {
        if (current_cluster == FAT_BAD_SECTOR || current_cluster == FAT_FREE_SECTOR)
        {
            free(chain->chain);
            free(chain);
            return NULL;
        }
        // 
        // TODO: handle loops, and other FAT corruption problems....
        // 
        
        uint32_t fat_entries = fat->fat_bytes / 2;
        if (current_cluster >= fat_entries) {
            free(chain->chain);
            free(chain);
            return NULL;
        }
        
        chain->cluster_count++;
        chain->chain = realloc(chain->chain, chain->cluster_count * sizeof(uint16_t));
        if(!chain->chain)
        {
            free(chain->chain);
            free(chain);
            return NULL;
        }
        chain->chain[i] = current_cluster;
        current_cluster = fat->fat[current_cluster];
        
        i++;
    }
    if (chain->cluster_count == 0) {
        free(chain);
        return NULL;
    }        
    return chain;
}

void *load_cluster_chain(Fat16 *fat, ClusterChain *chain, void *destination)
{
    // 
    // TODO: return sectors read
    // 
    for (int i = 0; i < chain->cluster_count; i++)
    {
        uint8_t *ptr = (uint8_t *)destination;
        ptr += i * fat->sectors_per_cluster * fat->bytes_per_sector;

        read_cluster(fat, chain->chain[i], ptr);
        
    }
    return destination;
}

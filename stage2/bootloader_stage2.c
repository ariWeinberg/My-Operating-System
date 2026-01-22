#include "STD/int.h"
#include "ATA_DRIVER/ata_driver.h"
#include "UTILS/asm_utils.h"
#include "UTILS/utils.h"
#include "FAT16/fat16.h"
#include "SCREEN_DRIVER/screen_driver.h"
#include "FAT16/fat16.h"
#include "MEMORY_MANAGMENT/memory_managment.h"



uint16_t identify_result[256]; 
Fat16 *fat;


void c_main()
{
    clear_screen();
    /* detect master */
    int type0 = detect_devtype(0, &ata_primary);
    print_string("master device is of type: ");
    print_string(devtype_to_string(type0));
    print_string("\n\r");

    ata_init();
    ata_identify(identify_result);
    
    clear_screen();
    print_string("test print\n\r");
    
    
    fat = malloc(sizeof(Fat16));
    fat_init(fat);

    DirEntry *found_dir;

    parsed_dir *dir = parse_dir(
        fat->root_dir,
        fat->bs.bpb.root_dir_entries
    );
    
    for (int i = 0; i < dir->count; i++)
    {
        print_string(dir->entries[i].name);
    
        if ((dir->entries[i].attributes & 0x10) &&
            strcmp(dir->entries[i].name, "NEWDIR1    ") == 0)
        {
            print_string("   |  found\n\r");
    
            found_dir = load_dir(fat, dir->entries[i].raw_entry);
    
            free_parsed_dir(dir);
    
            dir = parse_dir(
                found_dir,
                dir->entries[i].raw_entry->file_size / sizeof(DirEntry)
            );
            break;
        }
    
        print_string("\n\r");
    }
    
    for (int i = 0; i < dir->count; i++)
    {
        print_string(dir->entries[i].name);
        print_string("\n\r");
    }
    
        
while (1){;}
}

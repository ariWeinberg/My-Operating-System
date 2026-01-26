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
    
    
    fat = zalloc(sizeof(Fat16));
    fat_init(fat);
    print_string("fat initialized!\n\r");
    
    void *file = open(fat, "/NEWDIR1/NEWFILE3.TXT");
    if (file)
    {
        print_string("file loaded");
        print_string("\n\n\r===============\n\r file contents\n\r===============\n\n\r");
        print_string(file);
        print_string("\n\n");
    }
    else
    {print_string("file not loaded");}
        
while (1){;}
}

#include "STD/int.h"
#include "ATA_DRIVER/ATA.h"
#include "UTILS/asm_utils.h"
#include "UTILS/utils.h"
#include "FAT16/FAT16_API.h"
#include "SCREEN_DRIVER/screen_driver.h"
#include "MEMORY_MANAGMENT/memory_managment.h"



// uint16_t identify_result[256];
ata_identify_u identify_result;
Fat16 *fat;

void load_raw_kernel(Fat16 *fat, const char *path, uint32_t load_addr)
{
    print_string("Loading Raw Binary...\n");
    
    // 1. Read the file bytes
    uint8_t *file_data = open(fat, path);
    if (!file_data) return;

    // 2. Copy to the fixed address defined in your linker.ld (0x100000)
    // Assuming you have the file size from your FAT16 driver
    uint32_t file_size = get_file_size(fat, path); 
    memcpy((void*)load_addr, file_data, file_size);

    print_string("Jumping to Raw Binary...\n");

    // 3. Simple Jump
    typedef void (*kernel_entry)();
    kernel_entry start = (kernel_entry)load_addr;
    
    // Set up segments and jump
    __asm__ volatile (
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "mov %0, %%esp\n\t"    // Use a safe stack pointer
        "jmp *%1\n\t"          // Jump to 0x100000
        : : "r"(0x90000), "r"(start) : "eax"
    );
}


void c_main()
{
    clear_screen();
    /* detect master */
    int type0 = detect_devtype(0, &ata_primary);
    print_string("master device is of type: ");
    print_string(devtype_to_string(type0));
    print_string("\n\r");

    ata_init();
    ata_identify(&ata_primary, &identify_result);
    
    clear_screen();
    print_string("test print\n\r");
    
    
    fat = zalloc(sizeof(Fat16));
    fat_init(fat);
    print_string("fat initialized!\n\r");

    // load raw kernel from file "/OS/KERNEL" and run it.
    load_raw_kernel(fat, "/OS/KERNEL", 0x100000);
    print_string("\n\n");

        
while (1){;}
}

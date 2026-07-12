build_dir := ../build
stage2_dir := .
etc_dir := ../etc

FAT16_dir := $(stage2_dir)/FAT16
MEMORY_MANAGMENT_dir := $(stage2_dir)/MEMORY_MANAGMENT
ATA_DRIVER_dir := $(stage2_dir)/ATA_DRIVER
UTILS_dir := $(stage2_dir)/UTILS
SCREEN_DRIVER_dir := $(stage2_dir)/SCREEN_DRIVER
STRING_dir := $(stage2_dir)/STRING

GCC_FLAGS := -m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD := gcc $(GCC_FLAGS)

FAT16_OBJECTS := \
	$(build_dir)/fat16.o \
	$(build_dir)/fat16_helpers.o \
	$(build_dir)/fat16_error.o \
	$(build_dir)/open.o \
	$(build_dir)/chain.o \
	$(build_dir)/lfn.o \
	$(build_dir)/load_clusterd_entry.o \
	$(build_dir)/parse_dir.o \
	$(build_dir)/get_file_size.o

ATA_OBJECTS := \
	$(build_dir)/ata_driver.o \
	$(build_dir)/ata_helpers.o \
	$(build_dir)/pio_read28.o \
	$(build_dir)/ata_error.o \
	$(build_dir)/ata_identify.o \
	$(build_dir)/software_reset.o \
	$(build_dir)/detect_device_type.o

UTILS_OBJECTS := $(build_dir)/asm_utils.o $(build_dir)/utils.o
SCREEN_OBJECTS := $(build_dir)/screen_driver.o
STRING_OBJECTS := $(build_dir)/string.o
MEMORY_OBJECTS := $(build_dir)/memory_managment.o

STAGE2_LINK_OBJECTS := \
	$(build_dir)/bootloader_stage2.o \
	$(build_dir)/kernel.o \
	$(build_dir)/load_clusterd_entry.o \
	$(build_dir)/fat16_helpers.o \
	$(build_dir)/fat16_error.o \
	$(build_dir)/fat16.o \
	$(build_dir)/chain.o \
	$(build_dir)/lfn.o \
	$(build_dir)/memory_managment.o \
	$(build_dir)/ata_driver.o \
	$(build_dir)/utils.o \
	$(build_dir)/asm_utils.o \
	$(build_dir)/screen_driver.o \
	$(build_dir)/string.o \
	$(build_dir)/ata_helpers.o \
	$(build_dir)/pio_read28.o \
	$(build_dir)/ata_error.o \
	$(build_dir)/ata_identify.o \
	$(build_dir)/software_reset.o \
	$(build_dir)/detect_device_type.o \
	$(build_dir)/open.o \
	$(build_dir)/parse_dir.o \
	$(build_dir)/get_file_size.o

.PHONY: all clean fat16 memory ata utils screen string

all: $(build_dir)/stage2.bin

$(build_dir):
	mkdir -p $@

$(build_dir)/bootloader_stage2.o: $(stage2_dir)/bootloader_stage2.s | $(build_dir)
	nasm -f elf32 $< -o $@ -l $(build_dir)/bootloader_stage2.lst

$(build_dir)/kernel.o: $(stage2_dir)/bootloader_stage2.c | $(build_dir)
	$(GCC_CMD) -I$(stage2_dir) -c $< -o $@

$(FAT16_OBJECTS): fat16

fat16:
	$(MAKE) -C $(FAT16_dir) all

$(MEMORY_OBJECTS): memory

memory:
	$(MAKE) -C $(MEMORY_MANAGMENT_dir) all

$(ATA_OBJECTS): ata

ata:
	$(MAKE) -C $(ATA_DRIVER_dir) all

$(UTILS_OBJECTS): utils

utils:
	$(MAKE) -C $(UTILS_dir) all

$(SCREEN_OBJECTS): screen

screen:
	$(MAKE) -C $(SCREEN_DRIVER_dir) all

$(STRING_OBJECTS): string

string:
	$(MAKE) -C $(STRING_dir) all

$(build_dir)/stage2.elf: $(STAGE2_LINK_OBJECTS)
	ld -m elf_i386 -T $(etc_dir)/linker.ld $^ -o $@

$(build_dir)/stage2.bin: $(build_dir)/stage2.elf
	objcopy -O binary $< $@

clean:
	rm -f $(build_dir)/bootloader_stage2.lst \
	$(STAGE2_LINK_OBJECTS) \
	$(build_dir)/stage2.elf \
	$(build_dir)/stage2.bin

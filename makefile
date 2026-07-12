stage1_dir=stage1
stage2_dir=stage2
build_dir=build
out_dir=out
etc_dir=etc

FAT16_dir=$(stage2_dir)/FAT16
MEMORY_MANAGMENT_dir=$(stage2_dir)/MEMORY_MANAGMENT
ATA_DRIVER_dir=$(stage2_dir)/ATA_DRIVER
UTILS_dir=$(stage2_dir)/UTILS
SCREEN_DRIVER_dir=$(stage2_dir)/SCREEN_DRIVER
STRING_dir=$(stage2_dir)/STRING


final_kernel=./kernel

# Stage 1 bootloader
$(build_dir)/bootloader.o: $(stage1_dir)/bootloader.s
	nasm -f bin -o $(build_dir)/bootloader.o $(stage1_dir)/bootloader.s \
	-l $(build_dir)/bootloader.lst

# # memory_mapper
# $(build_dir)/memory_mapper.o: $(stage2_dir)/mem_map.s
# 	nasm -f elf32 -o $(build_dir)/memory_mapper.o $(stage2_dir)/mem_map.s

# Stage 2 bootloader
$(build_dir)/bootloader_stage2.o: $(stage2_dir)/bootloader_stage2.s
	nasm -f elf32 $(stage2_dir)/bootloader_stage2.s -o $(build_dir)/bootloader_stage2.o \
	-l $(build_dir)/bootloader_stage2.lst

# Kernel (C)
$(build_dir)/kernel.o: $(stage2_dir)/bootloader_stage2.c
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
	-c $(stage2_dir)/bootloader_stage2.c -o $(build_dir)/kernel.o

# Build FAT16 module via its own Makefile
$(build_dir)/fat16.o \
$(build_dir)/fat16_helpers.o \
$(build_dir)/fat16_error.o \
$(build_dir)/get_file_size.o \
$(build_dir)/chain.o \
$(build_dir)/open.o \
$(build_dir)/lfn.o \
$(build_dir)/load_clusterd_entry.o \
$(build_dir)/parse_dir.o:
	$(MAKE) -C $(FAT16_dir) all

# Build memory_managment module via its own Makefile
$(build_dir)/memory_managment.o:
	$(MAKE) -C $(MEMORY_MANAGMENT_dir) all

# Build ata_driver module via its own Makefile
$(build_dir)/ata_driver.o \
$(build_dir)/ata_helpers.o \
$(build_dir)/pio_read28.o \
$(build_dir)/ata_error.o \
$(build_dir)/ata_identify.o \
$(build_dir)/software_reset.o \
$(build_dir)/detect_device_type.o:
	$(MAKE) -C $(ATA_DRIVER_dir) all

# Build ata_driver module via its own Makefile
$(build_dir)/utils.o $(build_dir)/asm_utils.o:
	$(MAKE) -C $(UTILS_dir) all

# Build ata_driver module via its own Makefile
$(build_dir)/screen_driver.o:
	$(MAKE) -C $(SCREEN_DRIVER_dir) all

# Build ata_driver module via its own Makefile
$(build_dir)/string.o:
	$(MAKE) -C $(STRING_dir) all


kernel/build/kernel.bin:
	$(MAKE) -C $(final_kernel) all
	

$(build_dir)/stage2.elf: $(build_dir)/bootloader_stage2.o \
$(build_dir)/chain.o \
$(build_dir)/kernel.o \
$(build_dir)/fat16.o \
$(build_dir)/lfn.o \
$(build_dir)/memory_managment.o \
$(build_dir)/ata_driver.o \
$(build_dir)/utils.o \
$(build_dir)/asm_utils.o \
$(build_dir)/screen_driver.o \
$(build_dir)/string.o \
$(build_dir)/pio_read28.o \
$(build_dir)/ata_helpers.o \
$(build_dir)/ata_error.o \
$(build_dir)/ata_identify.o \
$(build_dir)/load_clusterd_entry.o \
$(build_dir)/software_reset.o \
$(build_dir)/detect_device_type.o \
$(build_dir)/fat16_error.o \
$(build_dir)/open.o \
$(build_dir)/fat16_helpers.o \
$(build_dir)/parse_dir.o \
# $(build_dir)/memory_mapper.o \
$(build_dir)/get_file_size.o 
	ld -m elf_i386 -T $(etc_dir)/linker.ld \
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
	$(build_dir)/get_file_size.o \
	-o	$(build_dir)/stage2.elf

# 	$(build_dir)/memory_mapper.o 
$(build_dir)/stage2.bin: $(build_dir)/stage2.elf $(build_dir)/bootloader_stage2.o
	objcopy -O binary $(build_dir)/stage2.elf $(build_dir)/stage2.bin

$(out_dir)/disk.img: $(build_dir)/bootloader.o $(build_dir)/stage2.bin kernel/build/kernel.bin
	dd if=/dev/zero of=$(out_dir)/disk.img bs=512 count=32768
	mkfs.fat -F 16 -f 2 -R 40 $(out_dir)/disk.img
	dd if=$(build_dir)/bootloader.o of=$(out_dir)/disk.img conv=notrunc bs=1 seek=61 skip=61
	dd if=$(build_dir)/stage2.bin of=$(out_dir)/disk.img conv=notrunc bs=512 seek=1
	mmd -i $(out_dir)/disk.img ::NEWDIR1
	mmd -i $(out_dir)/disk.img ::NEWDIR2
	mmd -i $(out_dir)/disk.img ::NEWDIR3
	mmd -i $(out_dir)/disk.img ::_TEST_LFN_ENTRIES_NEWDIR3
	mcopy -i $(out_dir)/disk.img $(etc_dir)/test.txt ::NEWFILE1
	mcopy -i $(out_dir)/disk.img $(etc_dir)/test.txt ::NEWFILE2
	mcopy -i $(out_dir)/disk.img $(etc_dir)/test_sub.txt ::/NEWDIR1/NEWFILE3.txt
	mdel -i $(out_dir)/disk.img ::NEWFILE1
	mmd -i $(out_dir)/disk.img ::OS
	mcopy -i $(out_dir)/disk.img kernel/build/kernel.bin ::/OS/KERNEL



run_disk: $(out_dir)/disk.img
	qemu-system-i386 -drive file=$(out_dir)/disk.img,format=raw -d int,cpu_reset -D qemu.log

debug_disk: $(out_dir)/disk.img
# 	qemu-system-i386 -m 5G -drive file=$(out_dir)/disk.img,format=raw -monitor stdio
	qemu-system-x86_64 -m 5G -drive file=$(out_dir)/disk.img,format=raw -monitor stdio


clean:
	rm -f $(build_dir)/bootloader.o \
	$(build_dir)/bootloader_stage2.o \
	floppy.img \
	$(build_dir)/bootloader.lst \
	$(build_dir)/bootloader_stage2.lst \
	$(build_dir)/kernel.o \
	$(build_dir)/utils.o \
	$(build_dir)/asm_utils.o \
	$(build_dir)/fat16.o \
	$(build_dir)/memory_managment.o \
	$(build_dir)/ata_driver.o \
	$(out_dir)/disk.img \
	$(build_dir)/stage2.bin \
	$(build_dir)/stage2.elf \
	$(build_dir)/screen_driver.o \
	$(build_dir)/string.o \
	$(build_dir)/ata_helpers.o \
	$(build_dir)/pio_read28.o \
	$(build_dir)/ata_error.o \
	$(build_dir)/ata_identify.o \
	$(build_dir)/software_reset.o \
	$(build_dir)/detect_device_type.o \
	$(build_dir)/fat16_error.o\
	$(build_dir)/fat16_helpers.o \
	$(build_dir)/chain.o \
	$(build_dir)/open.o \
	$(build_dir)/lfn.o \
	$(build_dir)/load_clusterd_entry.o \
	$(build_dir)/parse_dir.o \
	$(build_dir)/get_file_size.o \
	$(build_dir)/memory_mapper.o \
	kernel/build/kernel.bin

	$(MAKE) -C $(final_kernel) clean
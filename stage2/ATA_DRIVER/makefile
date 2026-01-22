build_dir=../../build
ATA_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/ata_driver.o: $(ATA_DRIVER_dir)/ata_driver.c $(ATA_DRIVER_dir)/ata_driver.h $(ATA_DRIVER_dir)/ata_defs.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/ata_driver.c -o $(build_dir)/ata_driver.o

# Default target
all: $(build_dir)/ata_driver.o

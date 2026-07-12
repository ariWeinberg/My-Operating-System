build_dir=../../build
ATA_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/ata_driver.o: $(ATA_DRIVER_dir)/ata_driver.c $(ATA_DRIVER_dir)/ATA.h
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/ata_driver.c -o $(build_dir)/ata_driver.o

# Compile FAT16 main file
$(build_dir)/ata_helpers.o: $(ATA_DRIVER_dir)/ata_helpers.c $(ATA_DRIVER_dir)/ATA.h
	gcc -m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/ata_helpers.c -o $(build_dir)/ata_helpers.o

# Compile FAT16 main file
$(build_dir)/pio_read28.o: $(ATA_DRIVER_dir)/pio_read28.c $(ATA_DRIVER_dir)/ATA.h
	gcc -m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/pio_read28.c -o $(build_dir)/pio_read28.o

# Compile FAT16 main file
$(build_dir)/ata_error.o: $(ATA_DRIVER_dir)/ata_error.c $(ATA_DRIVER_dir)/ATA.h
	gcc -m32 -g -std=gnu17 -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/ata_error.c -o $(build_dir)/ata_error.o

$(build_dir)/ata_identify.o: $(ATA_DRIVER_dir)/ata_identify.c $(ATA_DRIVER_dir)/ATA.h
	gcc -m32 -g -std=gnu17 -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/ata_identify.c -o $(build_dir)/ata_identify.o

$(build_dir)/software_reset.o: $(ATA_DRIVER_dir)/software_reset.c $(ATA_DRIVER_dir)/ATA.h
	gcc -m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/software_reset.c -o $(build_dir)/software_reset.o

$(build_dir)/detect_device_type.o: $(ATA_DRIVER_dir)/detect_device_type.c $(ATA_DRIVER_dir)/ATA.h
	gcc -m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(ATA_DRIVER_dir)/detect_device_type.c -o $(build_dir)/detect_device_type.o


# Default target
all: $(build_dir)/ata_driver.o \
$(build_dir)/ata_helpers.o \
$(build_dir)/pio_read28.o \
$(build_dir)/ata_error.o \
$(build_dir)/ata_identify.o\
$(build_dir)/software_reset.o \
$(build_dir)/detect_device_type.o

build_dir=../../build
FAT16_dir=.

# Compile FAT16 helpers first
$(build_dir)/fat16_helpers.o: $(FAT16_dir)/fat16_helpers.c $(FAT16_dir)/fat16_helpers.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/fat16_helpers.c -o $(build_dir)/fat16_helpers.o

# Compile FAT16 main file
$(build_dir)/fat16.o: $(FAT16_dir)/fat16.c $(FAT16_dir)/fat16.h $(FAT16_dir)/fat16_defs.h \
                       $(FAT16_dir)/fat16_helpers.h $(FAT16_dir)/fat16_ondisk.h \
                       $(FAT16_dir)/fat16_structures.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/fat16.c -o $(build_dir)/fat16.o

# Default target
all: $(build_dir)/fat16.o $(build_dir)/fat16_helpers.o

build_dir=../../build
FAT16_dir=.

# Compile FAT16 helpers first
$(build_dir)/fat16_helpers.o: $(FAT16_dir)/fat16_helpers.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/fat16_helpers.c -o $(build_dir)/fat16_helpers.o

# Compile FAT16 helpers first
$(build_dir)/fat16_error.o: $(FAT16_dir)/fat16_error.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/fat16_error.c -o $(build_dir)/fat16_error.o

# Compile FAT16 main file
$(build_dir)/fat16.o: $(FAT16_dir)/fat16.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/fat16.c -o $(build_dir)/fat16.o

$(build_dir)/open.o: $(FAT16_dir)/open.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/open.c -o $(build_dir)/open.o

$(build_dir)/chain.o: $(FAT16_dir)/chain.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/chain.c -o $(build_dir)/chain.o

$(build_dir)/lfn.o: $(FAT16_dir)/lfn.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/lfn.c -o $(build_dir)/lfn.o

$(build_dir)/load_clusterd_entry.o: $(FAT16_dir)/load_clusterd_entry.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/load_clusterd_entry.c -o $(build_dir)/load_clusterd_entry.o

$(build_dir)/parse_dir.o: $(FAT16_dir)/parse_dir.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/parse_dir.c -o $(build_dir)/parse_dir.o

$(build_dir)/get_file_size.o: $(FAT16_dir)/get_file_size.c $(FAT16_dir)/fat16.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(FAT16_dir)/get_file_size.c -o $(build_dir)/get_file_size.o

# Default target
all: $(build_dir)/fat16.o \
$(build_dir)/fat16_helpers.o \
$(build_dir)/fat16_error.o \
$(build_dir)/open.o \
$(build_dir)/chain.o \
$(build_dir)/lfn.o \
$(build_dir)/load_clusterd_entry.o \
$(build_dir)/parse_dir.o \
$(build_dir)/get_file_size.o

build_dir=../build
MEMORY_MANAGMENT_dir=.

# Compile FAT16 main file
$(build_dir)/memory_managment.o: $(MEMORY_MANAGMENT_dir)/memory_managment.c $(MEMORY_MANAGMENT_dir)/memory_managment.h
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(MEMORY_MANAGMENT_dir)/memory_managment.c -o $(build_dir)/memory_managment.o

# Default target
all: $(build_dir)/memory_managment.o

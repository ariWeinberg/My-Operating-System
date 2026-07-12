build_dir=../build
MEMORY_MANAGMENT_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/memory_managment.o: $(MEMORY_MANAGMENT_dir)/memory_managment.c $(MEMORY_MANAGMENT_dir)/memory_managment.h
	$(GCC_CMD) \
        -I../../stage2 -c $(MEMORY_MANAGMENT_dir)/memory_managment.c -o $(build_dir)/memory_managment.o

# Default target
all: $(build_dir)/memory_managment.o

build_dir=../../build
UTILS_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/asm_utils.o: $(UTILS_dir)/asm_utils.c $(UTILS_dir)/asm_utils.h
	$(GCC_CMD) \
        -I../../stage2 -c $(UTILS_dir)/asm_utils.c -o $(build_dir)/asm_utils.o

# Compile FAT16 main file
$(build_dir)/utils.o: $(UTILS_dir)/utils.c $(UTILS_dir)/utils.h
	$(GCC_CMD) \
        -I../../stage2 -c $(UTILS_dir)/utils.c -o $(build_dir)/utils.o

# Default target
all: $(build_dir)/asm_utils.o $(build_dir)/utils.o

build_dir=../build
STRING_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/string.o: $(STRING_dir)/string.c $(STRING_dir)/string.h
	@echo "compiling string module."
	@$(GCC_CMD) \
        -I../../stage2 -c $(STRING_dir)/string.c -o $(build_dir)/string.o

# Default target
all: $(build_dir)/string.o

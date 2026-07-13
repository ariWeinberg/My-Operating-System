build_dir=../build
STDIN_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/stdin.o: $(STDIN_dir)/stdin.c $(STDIN_dir)/stdin.h
	$(GCC_CMD) \
        -I../../kernel -c $(STDIN_dir)/stdin.c -o $(build_dir)/stdin.o

# Default target
all: $(build_dir)/stdin.o

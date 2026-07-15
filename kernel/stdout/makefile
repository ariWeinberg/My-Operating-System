build_dir=../build
STDOUT_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/stdout.o: $(STDOUT_dir)/stdout.c $(STDOUT_dir)/stdout.h
	$(GCC_CMD) \
        -I../../stage2 -c $(STDOUT_dir)/stdout.c -o $(build_dir)/stdout.o

# Default target
all: $(build_dir)/stdout.o

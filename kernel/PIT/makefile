build_dir=../build
SCREEN_DRIVER_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/PIT.o: $(SCREEN_DRIVER_dir)/PIT.c $(SCREEN_DRIVER_dir)/PIT.h
	$(GCC_CMD) \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/PIT.c -o $(build_dir)/PIT.o

# Default target
all: $(build_dir)/PIT.o

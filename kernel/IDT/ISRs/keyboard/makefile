build_dir=../../../build
SCREEN_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/keyboard.o: $(SCREEN_DRIVER_dir)/keyboard.c $(SCREEN_DRIVER_dir)/keyboard.h
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/keyboard.c -o $(build_dir)/keyboard.o

# Default target
all: $(build_dir)/keyboard.o

build_dir=../build
SCREEN_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/PIT.o: $(SCREEN_DRIVER_dir)/PIT.c $(SCREEN_DRIVER_dir)/PIT.h
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-PIC -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/PIT.c -o $(build_dir)/PIT.o

# Default target
all: $(build_dir)/PIT.o

build_dir=../build
SCREEN_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/PIC.o: $(SCREEN_DRIVER_dir)/PIC.c $(SCREEN_DRIVER_dir)/PIC.h
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/PIC.c -o $(build_dir)/PIC.o

# Default target
all: $(build_dir)/PIC.o

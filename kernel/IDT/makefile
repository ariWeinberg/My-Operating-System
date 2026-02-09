build_dir=../build
SCREEN_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/IDT.o: $(SCREEN_DRIVER_dir)/IDT.c $(SCREEN_DRIVER_dir)/IDT.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/IDT.c -o $(build_dir)/IDT.o

# Default target
all: $(build_dir)/IDT.o

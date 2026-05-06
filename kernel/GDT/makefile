build_dir=../build
SCREEN_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/GDT.o: $(SCREEN_DRIVER_dir)/GDT.c $(SCREEN_DRIVER_dir)/GDT.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/GDT.c -o $(build_dir)/GDT.o

# Default target
all: $(build_dir)/GDT.o

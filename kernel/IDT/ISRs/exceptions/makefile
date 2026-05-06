build_dir=../../../build
SCREEN_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/exceptions.o: $(SCREEN_DRIVER_dir)/exceptions.c $(SCREEN_DRIVER_dir)/exceptions.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/exceptions.c -o $(build_dir)/exceptions.o

# Default target
all: $(build_dir)/exceptions.o

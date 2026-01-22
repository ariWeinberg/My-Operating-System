build_dir=../../build
STRING_dir=.

# Compile FAT16 main file
$(build_dir)/string.o: $(STRING_dir)/string.c $(STRING_dir)/string.h
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(STRING_dir)/string.c -o $(build_dir)/string.o

# Default target
all: $(build_dir)/string.o

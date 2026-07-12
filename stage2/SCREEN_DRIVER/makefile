build_dir=../../build
SCREEN_DRIVER_dir=.

# Compile FAT16 main file
$(build_dir)/screen_driver.o: $(SCREEN_DRIVER_dir)/screen_driver.c $(SCREEN_DRIVER_dir)/screen_driver.h
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(SCREEN_DRIVER_dir)/screen_driver.c -o $(build_dir)/screen_driver.o

# Default target
all: $(build_dir)/screen_driver.o

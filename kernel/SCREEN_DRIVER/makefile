build_dir=../build
SCREEN_DRIVER_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/screen_driver.o: $(SCREEN_DRIVER_dir)/screen_driver.c $(SCREEN_DRIVER_dir)/screen_driver.h
	$(GCC_CMD) \
        -I../../stage2 -c $(SCREEN_DRIVER_dir)/screen_driver.c -o $(build_dir)/screen_driver.o

# Default target
all: $(build_dir)/screen_driver.o

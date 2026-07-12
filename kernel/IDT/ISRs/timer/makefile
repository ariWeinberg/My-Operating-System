build_dir=../../../build
TIMER_IRQ_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/timer_irq.o: $(TIMER_IRQ_dir)/timer.c $(TIMER_IRQ_dir)/timer.h
	$(GCC_CMD) \
        -I../../../kernel -c $(TIMER_IRQ_dir)/timer.c -o $(build_dir)/timer_irq.o

# Default target
all: $(build_dir)/timer_irq.o

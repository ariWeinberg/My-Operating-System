build_dir=../build
TASKS_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/tasks.o: $(TASKS_dir)/tasks.c $(TASKS_dir)/task.h $(TASKS_dir)/tasks.h
	$(GCC_CMD) \
        -I../../kernel -c $(TASKS_dir)/tasks.c -o $(build_dir)/tasks.o

# Default target
all: $(build_dir)/tasks.o

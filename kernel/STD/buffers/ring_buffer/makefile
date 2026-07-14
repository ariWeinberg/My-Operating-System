build_dir=../../../build
RING_BUFFER_dir=.

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

$(build_dir)/ring_buffer.o: $(RING_BUFFER_dir)/ring_buffer.c $(RING_BUFFER_dir)/ring_buffer.h
	$(GCC_CMD) \
		-I../../../kernel -c $(RING_BUFFER_dir)/ring_buffer.c -o $(build_dir)/ring_buffer.o

all: $(build_dir)/ring_buffer.o

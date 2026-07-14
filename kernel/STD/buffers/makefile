build_dir=../../build
BUFFERS_dir=.

.PHONY: all ring_buffer

$(build_dir)/ring_buffer.o: ring_buffer

ring_buffer:
	$(MAKE) -C ./ring_buffer all

all: $(build_dir)/ring_buffer.o

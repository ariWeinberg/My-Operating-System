build_dir=../build
SCREEN_DRIVER_dir=.

ISRs_DIR=./ISRs
EXCEPTIONS_DIR=$(ISRs_DIR)/exceptions
KEYBOARD_DIR=$(ISRs_DIR)/keyboard

GCC_FLAGS=-m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD=gcc $(GCC_FLAGS)

# Compile FAT16 main file
$(build_dir)/IDT.o: $(SCREEN_DRIVER_dir)/IDT.c $(SCREEN_DRIVER_dir)/IDT.h
	gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/IDT.c -o $(build_dir)/IDT.o

# Compile FAT16 main file
$(build_dir)/interrupt_stub.o: $(SCREEN_DRIVER_dir)/interrupt_stub.c $(SCREEN_DRIVER_dir)/IDT.h
	$(GCC_CMD) \
        -I../../kernel -c $(SCREEN_DRIVER_dir)/interrupt_stub.c -o $(build_dir)/interrupt_stub.o


$(build_dir)/keyboard.o:
	@$(MAKE) -C $(KEYBOARD_DIR) all

$(build_dir)/exceptions.o:
	@$(MAKE) -C $(EXCEPTIONS_DIR) all

# Default target
all: $(build_dir)/IDT.o $(build_dir)/interrupt_stub.o $(build_dir)/exceptions.o $(build_dir)/keyboard.o

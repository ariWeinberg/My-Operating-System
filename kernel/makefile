# Directories
build_dir=./build
kernel_dir=.

UTILS_dir=$(kernel_dir)/UTILS
STRING_dir=$(kernel_dir)/STRING
SCREEN_DRIVER_dir=$(kernel_dir)/SCREEN_DRIVER
MEMORY_MANAGMENT_dir=$(kernel_dir)/MEMORY_MANAGMENT
STD_dir=$(kernel_dir)/STD

PIC_DIR=$(kernel_dir)/PIC
IDT_DIR=$(kernel_dir)/IDT
KEYBOARD_DIR=$(IDT_DIR)/ISRs

# Kernel object
$(build_dir)/kernel.o: $(kernel_dir)/main.c
	gcc -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -c $< -o $@
        # -I$(UTILS_dir) -I$(STRING_dir) -I$(SCREEN_DRIVER_dir) -I$(MEMORY_MANAGMENT_dir) -I$(STD_dir) \

# Build modules
$(build_dir)/asm_utils.o $(build_dir)/utils.o:
	@$(MAKE) -C $(UTILS_dir) all

$(build_dir)/string.o:
	@$(MAKE) -C $(STRING_dir) all

$(build_dir)/screen_driver.o:
	@$(MAKE) -C $(SCREEN_DRIVER_dir) all

$(build_dir)/memory_managment.o:
	@$(MAKE) -C $(MEMORY_MANAGMENT_dir) all

$(build_dir)/keyboard.o:
	@$(MAKE) -C $(KEYBOARD_DIR) all

$(build_dir)/IDT.o $(build_dir)/interrupt_stub.o:
	@$(MAKE) -C $(IDT_DIR) all

$(build_dir)/PIC.o:
	@$(MAKE) -C $(PIC_DIR) all

# Link all into ELF
$(build_dir)/kernel.elf: $(build_dir)/kernel.o \
                        $(build_dir)/asm_utils.o $(build_dir)/utils.o \
                        $(build_dir)/string.o \
                        $(build_dir)/screen_driver.o \
                        $(build_dir)/memory_managment.o \
						$(build_dir)/PIC.o \
						$(build_dir)/IDT.o \
						$(build_dir)/keyboard.o	\
						$(build_dir)/interrupt_stub.o
	ld -m elf_i386 -T linker.ld -o $@ $^



# Convert to flat binary
$(build_dir)/kernel.bin: $(build_dir)/kernel.elf
	@objcopy -O binary $< $@

# Clean
clean:
	@rm -f $(build_dir)/kernel.o \
	      $(build_dir)/asm_utils.o $(build_dir)/utils.o \
	      $(build_dir)/string.o \
	      $(build_dir)/screen_driver.o \
	      $(build_dir)/memory_managment.o \
	      $(build_dir)/kernel.elf \
	      $(build_dir)/kernel.bin \
	      $(build_dir)/IDT.o \
	      $(build_dir)/keyboard.o \
	      $(build_dir)/PIC.o \
		  $(build_dir)/interrupt_stub.o
	@echo "successfully deleted all build files."

all: $(build_dir)/kernel.bin
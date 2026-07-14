build_dir := ./build
kernel_dir := .

UTILS_dir := $(kernel_dir)/UTILS
STRING_dir := $(kernel_dir)/STRING
SCREEN_DRIVER_dir := $(kernel_dir)/SCREEN_DRIVER
MEMORY_MANAGMENT_dir := $(kernel_dir)/MEMORY_MANAGMENT
PIC_DIR := $(kernel_dir)/PIC
PIT_DIR := $(kernel_dir)/PIT
IDT_DIR := $(kernel_dir)/IDT
GDT_DIR := $(kernel_dir)/GDT
TASKS_DIR := $(kernel_dir)/tasks
STDIN_DIR := $(kernel_dir)/stdin

GCC_FLAGS := -m32 -std=gnu17 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs
GCC_CMD := gcc $(GCC_FLAGS)

KERNEL_LINK_OBJECTS := \
	$(build_dir)/kernel.o \
	$(build_dir)/asm_utils.o \
	$(build_dir)/utils.o \
	$(build_dir)/string.o \
	$(build_dir)/screen_driver.o \
	$(build_dir)/memory_managment.o \
	$(build_dir)/PIC.o \
	$(build_dir)/PIT.o \
	$(build_dir)/IDT.o \
	$(build_dir)/exceptions.o \
	$(build_dir)/keyboard.o \
	$(build_dir)/GDT.o \
	$(build_dir)/interrupt_stub.o \
	$(build_dir)/timer_irq.o \
	$(build_dir)/stdin.o \
	$(build_dir)/semaphore.o \
	$(build_dir)/tasks.o \
	$(build_dir)/mutex.o

.PHONY: all clean utils string screen memory pic pit idt gdt tasks stdin

all: $(build_dir)/kernel.bin

$(build_dir):
	mkdir -p $@

$(build_dir)/kernel.o: $(kernel_dir)/main.c | $(build_dir)
	$(GCC_CMD) -c $< -o $@

$(build_dir)/asm_utils.o $(build_dir)/utils.o: utils

utils:
	$(MAKE) -C $(UTILS_dir) all

$(build_dir)/string.o: string

string:
	$(MAKE) -C $(STRING_dir) all

$(build_dir)/screen_driver.o: screen

screen:
	$(MAKE) -C $(SCREEN_DRIVER_dir) all

$(build_dir)/memory_managment.o: memory

memory:
	$(MAKE) -C $(MEMORY_MANAGMENT_dir) all

$(build_dir)/IDT.o $(build_dir)/interrupt_stub.o $(build_dir)/exceptions.o $(build_dir)/keyboard.o $(build_dir)/timer_irq.o: idt

idt:
	$(MAKE) -C $(IDT_DIR) all

$(build_dir)/PIC.o: pic

pic:
	$(MAKE) -C $(PIC_DIR) all

$(build_dir)/PIT.o: pit

pit:
	$(MAKE) -C $(PIT_DIR) all

$(build_dir)/GDT.o: gdt

$(build_dir)/tasks.o $(build_dir)/semaphore.o $(build_dir)/mutex.o: tasks

tasks:
	$(MAKE) -C $(TASKS_DIR) all

$(build_dir)/stdin.o: stdin

stdin:
	$(MAKE) -C $(STDIN_DIR) all

gdt:
	$(MAKE) -C $(GDT_DIR) all

$(build_dir)/kernel.elf: $(KERNEL_LINK_OBJECTS)
	ld -m elf_i386 -T linker.ld $^ -o $@

$(build_dir)/kernel.bin: $(build_dir)/kernel.elf
	objcopy -O binary $< $@

clean:
	rm -f $(KERNEL_LINK_OBJECTS) \
	$(build_dir)/timer_irq.o \
	$(build_dir)/kernel.elf \
	$(build_dir)/kernel.bin

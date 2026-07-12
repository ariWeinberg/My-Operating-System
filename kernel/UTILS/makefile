build_dir=../build
UTILS_dir=.

# Compile FAT16 main file
$(build_dir)/asm_utils.o: $(UTILS_dir)/asm_utils.c $(UTILS_dir)/asm_utils.h
	@echo "compiling asm_utils module."
	@gcc -std=gnu17 -m32 -g -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(UTILS_dir)/asm_utils.c -o $(build_dir)/asm_utils.o

# Compile FAT16 main file
$(build_dir)/utils.o: $(UTILS_dir)/utils.c $(UTILS_dir)/utils.h
	@echo "compiling utils module."
	@gcc -m32 -g -std=gnu17 -ffreestanding -fno-pic -fno-pie -nostdlib -nostartfiles -nodefaultlibs \
        -I../../stage2 -c $(UTILS_dir)/utils.c -o $(build_dir)/utils.o

# Default target
all: $(build_dir)/asm_utils.o $(build_dir)/utils.o

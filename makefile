stage1_dir := stage1
stage2_dir := stage2
kernel_dir := kernel
build_dir := build
out_dir := out
etc_dir := etc

.PHONY: all run_disk debug_disk container-build verify-reproducible clean FORCE

all: $(out_dir)/disk.img

$(build_dir):
	mkdir -p $@

$(out_dir):
	mkdir -p $@

# Stage 1 boot sector.
$(build_dir)/bootloader.o: $(stage1_dir)/bootloader.s | $(build_dir)
	nasm -f bin -o $@ $(stage1_dir)/bootloader.s -l $(build_dir)/bootloader.lst

# Stage 2 and kernel builds are delegated to their own makefiles.
$(build_dir)/stage2.bin: FORCE
	$(MAKE) -C $(stage2_dir) all

$(kernel_dir)/build/kernel.bin: FORCE
	$(MAKE) -C $(kernel_dir) all

$(out_dir)/disk.img: $(build_dir)/bootloader.o $(build_dir)/stage2.bin $(kernel_dir)/build/kernel.bin | $(out_dir)
	dd if=/dev/zero of=$@ bs=512 count=32768
	mkfs.fat -F 16 -f 2 -R 40 -i 4152494f $@
	dd if=$(build_dir)/bootloader.o of=$@ conv=notrunc bs=1 seek=61 skip=61
	dd if=$(build_dir)/stage2.bin of=$@ conv=notrunc bs=512 seek=1
	mmd -i $@ ::NEWDIR1
	mmd -i $@ ::NEWDIR2
	mmd -i $@ ::NEWDIR3
	mmd -i $@ ::_TEST_LFN_ENTRIES_NEWDIR3
	mcopy -i $@ $(etc_dir)/test.txt ::NEWFILE1
	mcopy -i $@ $(etc_dir)/test.txt ::NEWFILE2
	mcopy -i $@ $(etc_dir)/test_sub.txt ::/NEWDIR1/NEWFILE3.txt
	mdel -i $@ ::NEWFILE1
	mmd -i $@ ::OS
	mcopy -i $@ $(kernel_dir)/build/kernel.bin ::/OS/KERNEL

container-build:
	sh scripts/build.sh

verify-reproducible:
	@set -eu; \
	tmp=$$(mktemp -d); \
	trap 'rm -rf "$$tmp"' EXIT INT TERM; \
	sh scripts/build.sh; \
	cp $(out_dir)/SHA256SUMS "$$tmp/first"; \
	sh scripts/build.sh; \
	cmp "$$tmp/first" $(out_dir)/SHA256SUMS; \
	echo "reproducible: $$(cat $(out_dir)/SHA256SUMS)"

run_disk: $(out_dir)/disk.img
	qemu-system-i386 -drive file=$<,format=raw -d int,cpu_reset -D qemu.log

debug_disk: $(out_dir)/disk.img
# 	qemu-system-i386 -m 5G -drive file=$<,format=raw -monitor stdio
	qemu-system-x86_64 -m 5G -drive file=$<,format=raw -monitor stdio

clean:
	rm -f $(build_dir)/bootloader.o \
	$(build_dir)/bootloader.lst \
	$(out_dir)/disk.img \
	$(out_dir)/SHA256SUMS \
	$(out_dir)/build-manifest.txt \
	qemu.log
	$(MAKE) -C $(stage2_dir) clean
	$(MAKE) -C $(kernel_dir) clean

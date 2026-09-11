# Legacy i386 virtual-hardware contract

Status: Frozen regression contract  
Baseline: commit `a7812974ef4440f5aa52523fee5a8b144325135e`  
Product branch: `ai-native-main`  
Contract name: `legacy-i386`

## Purpose

This document freezes the virtual-hardware assumptions of the audited BIOS/i386
kernel. The path remains a learning and regression target while the product path
moves to x86_64 UEFI. Changes that break this contract require an ADR, a migration
plan, and an explicit update to the legacy regression tests.

This contract describes compatibility; it does not endorse the current
implementation as production-safe.

## Canonical QEMU profile

The reproducible regression profile is:

| Item | Frozen value |
|---|---|
| Emulator | `qemu-system-i386` |
| Machine family | legacy PC (`pc`/i440FX-compatible) |
| Firmware | legacy BIOS, not UEFI |
| CPU architecture | i386-compatible CPU capable of 32-bit protected mode |
| RAM | 128 MiB |
| Boot device | first fixed disk |
| Disk attachment | primary-master IDE-compatible disk |
| Disk format | raw |
| Display | VGA-compatible 80x25 text mode |
| Keyboard | PS/2-compatible |
| Serial | not required by baseline commit |
| Network | not required |
| SMP | one vCPU; the kernel is not SMP-aware |

Canonical invocation:

```sh
qemu-system-i386 \
  -machine pc \
  -m 128M \
  -smp 1 \
  -drive file=out/disk.img,format=raw,if=ide,index=0 \
  -boot c
```

The current `make run_disk` command is compatible but relies on QEMU defaults.
Future test automation must spell out the frozen values above.

## CPU and firmware contract

1. Firmware loads sector 0 at physical address `0x00007C00`, enters in 16-bit
   real mode, and passes the BIOS boot-drive number in `DL`.
2. BIOS video interrupt `INT 10h`, mode 03h, is available to establish an
   80x25 VGA text console.
3. BIOS disk interrupt `INT 13h`, function 02h, supports CHS reads from the
   boot disk.
4. BIOS memory service `INT 15h E820h` returns a valid SMAP memory map.
5. Port `0x92` supports the fast A20 gate.
6. The CPU supports the 80386 protected-mode instructions and 32-bit flat
   segmentation used by the stage-2 GDT.
7. Paging, PAE, long mode, APIC, SMP, and Ring 3 are not part of this contract.

## Fixed physical-memory layout

| Physical range/address | Owner and assumption |
|---|---|
| `0x00000000`–`0x000003FF` | Real-mode interrupt vector table; firmware-owned |
| `0x00000400`–`0x000004FF` | BIOS data area; firmware-owned |
| `0x00007A00` downward | Stage-1 real-mode stack |
| `0x00007C00`–`0x00007DFF` | BIOS-loaded boot sector |
| `0x00008000` onward | Stage-2 image, linked and entered at `0000:8000` |
| `0x00090000` downward | Stage-2 protected-mode stack and kernel-entry stack |
| `0x00100000` | Flat kernel binary load and entry address |
| E820 buffer | Defined by `stage2/mem_map.s`; must remain outside images/stacks |
| VGA text memory `0x000B8000` | Kernel/stage-2 text output |

All addresses are physical because the legacy path has no paging. The kernel,
stage 2, and tasks share one Ring 0 address space.

A change to any fixed address is a contract change until `BootInfoV1` replaces
these implicit handoffs.

## Disk and filesystem contract

The build creates `out/disk.img` as exactly 32,768 sectors of 512 bytes:
16 MiB.

The image is formatted by:

```sh
mkfs.fat -F 16 -f 2 -R 40 out/disk.img
```

Frozen assumptions:

- Logical sector size is 512 bytes.
- Sector 0 contains the BIOS boot sector and the FAT BPB/EBR fields populated
  by `mkfs.fat`; build logic preserves the generated BPB bytes.
- Stage 1 begins reading at CHS cylinder 0, head 0, sector 2.
- The stage-2 read count is taken from the BPB reserved-sector count.
- Stage 2 is stored starting at LBA 1 inside the reserved region.
- The boot disk is visible as the primary ATA device at command block
  `0x1F0` and control block `0x3F6`.
- Stage 2 uses ATA PIO28 and little-endian FAT16 parsing.
- The kernel file path is exactly `/OS/KERNEL`, matched using the existing
  FAT16 path/name behavior.
- The kernel is a flat binary copied to physical `0x00100000`.
- The legacy boot path is read-only after image construction.

The partition-table entry currently emitted by stage 1 is not the source of
truth for stage-2 loading. Code must not begin depending on it without a
separate contract change and regression coverage.

## Build contract at the audit baseline

Required host commands are:

- `nasm`
- `gcc` with 32-bit code-generation support
- GNU `ld` and `objcopy`
- `dd`
- `mkfs.fat`
- mtools: `mmd`, `mcopy`, and `mdel`
- GNU `make`

The baseline build is:

```sh
make clean
make
test "$(stat -c %s out/disk.img)" -eq 16777216
```

Until the pinned toolchain work is complete, byte-for-byte image identity across
different host versions is not guaranteed. This issue freezes inputs and
hardware behavior; issue T02 owns tool versions and deterministic cross-host
output.

## VirtualBox compatibility profile

Use these settings when importing the raw image after converting it to VDI:

- Type: Other; Version: Other/Unknown (32-bit)
- Legacy BIOS enabled; EFI disabled
- 128 MiB RAM
- 1 processor; PAE/NX disabled for the baseline
- I/O APIC disabled
- VDI attached as IDE primary master
- PIIX3/PIIX4-compatible IDE controller
- VMSVGA/VGA-compatible display
- PS/2 keyboard
- Network and audio disabled unless being tested separately

## VMware compatibility profile

Use these settings when importing the raw image after converting it to VMDK:

- Guest type: Other 32-bit
- Legacy BIOS firmware
- 128 MiB RAM
- 1 processor and 1 core
- VMDK attached to the first IDE disk position
- VGA-compatible display and PS/2-compatible keyboard
- Network, USB, and audio not required by the baseline

VirtualBox and VMware are release-compatibility lanes. QEMU is the mandatory
per-change regression gate.

## Required regression evidence

A change preserves `legacy-i386` only when all of the following are true:

1. A clean checkout builds `out/disk.img` successfully.
2. The image is exactly 16 MiB.
3. The canonical QEMU profile reaches the existing kernel/task interaction
   state without reset, triple fault, or hang during boot.
4. The boot disk remains readable through the primary ATA PIO path.
5. Stage 2 loads `/OS/KERNEL` at 1 MiB and transfers control successfully.
6. The evidence records the source commit, build commands, host/tool versions,
   QEMU command, outcome, and any deviations.

Automated serial markers and machine-readable success are intentionally deferred
to T03 and T04. Until then, the final state must be confirmed interactively and
recorded in the change report.

## Known deviations and hazards at the baseline

These are frozen observations, not accepted product behavior:

- The build is not yet version-pinned or proven reproducible across clean hosts.
- Stage 1 uses CHS rather than an LBA BIOS extension.
- Fixed load addresses and stacks have no general overlap validator.
- Stage 1 derives a BIOS read count directly from the BPB reserved-sector field.
- The stage-2 and kernel linker outputs currently trigger RWX/executable-stack
  warnings; T07 owns their removal.
- There is no paging or memory protection.
- The kernel and all tasks execute in Ring 0.
- The ATA/FAT path has limited error handling and is not safe for arbitrary
  hostile media.
- The existing QEMU run target relies on emulator defaults.
- The legacy path has no deterministic completion signal or headless boot
  assertion yet.

## Change-control rule

Any proposal that alters firmware type, disk interface/position, sector size,
FAT layout, fixed physical addresses, CPU mode, or required emulated devices
must do one of the following:

- preserve this profile as a passing regression lane; or
- supersede it through an accepted ADR that names the migration and retirement
  policy.

The x86_64 UEFI product path is additive and must not silently redefine
`legacy-i386`.

# My Operating System -> Production AI-Native OS Master Plan

**Status:** Proposed for approval<br>
**Repository:** `ariWeinberg/My-Operating-System`<br>
**Audit snapshot:** commit `a781297` on `main`, 2026-08-28<br>
**Planning horizon:** multi-year, custom-kernel product path<br>
**Estimate class:** architecture/WBS estimate, not a delivery commitment

## 1. Executive decision

Continue this kernel into a real operating system. Do not replace it with Linux.

The production target should be narrowed to a headless x86_64 AI node that boots through UEFI on QEMU and a small, published reference-PC matrix. The current i386 BIOS boot path remains as a learning and regression target, but the product path moves to x86_64, Ring 3 processes, virtual memory, a stable syscall ABI, a Python-first userspace, local model inference, and clustered compute/storage.

“Everything in userspace is Python” is defined as:

- All AriOS-owned services, task logic, orchestration, policy, UI, cluster control, and generated programs are Python.
- CPython, the C library/ABI shim, audited cryptography, and the local inference engine are native runtime dependencies. CPython itself cannot exist without a native runtime.
- No model-generated code runs in the kernel or in a privileged native service.
- Native additions require an ADR, a stable Python binding, fuzzing, and a narrow privilege boundary.

This is a feasible but very large program. The task-level baseline estimate is **37,398 human hours**, with a realistic range of **28,000-56,000 hours**. That is roughly 16-32 experienced engineer-years at 1,750 productive hours/year. A stable team of 6-8 systems engineers could plausibly reach the narrow v1.0 target in 4-6 calendar years; one developer should expect well over a decade.

## 2. What exists today

The repository is a serious bootable learning kernel, not yet an operating-system platform.

### Confirmed strengths

- A BIOS boot sector loads a second stage at a fixed address.
- Stage 2 enters 32-bit protected mode, reads an E820 memory map, identifies ATA, parses FAT16, and loads `/OS/KERNEL` at 1 MiB.
- The kernel has GDT, IDT, PIC, PIT, exception stubs, PS/2 keyboard input, VGA text output, a small allocator, and timer-driven cooperative/preemptive-style context switching.
- Five created tasks plus the bootstrap task can run, and a line reader feeds an echo task.
- The image builds successfully in a clean Debian Bookworm toolchain and produces a 16 MiB FAT16 disk image.

### Critical gaps and defects to resolve before extending features

- The linker emits RWX load segments, and stage-2 assembly lacks a non-executable stack marker.
- The kernel is 32-bit, single-address-space, Ring 0 only, fixed-address, and has no paging.
- The allocator accepts 16-bit sizes, does not coalesce or reclaim heap-top space, and is duplicated in stage 2 and the kernel.
- Scheduler state is only an `esp`; there are no task states, blocking, priorities, ownership, process address spaces, or bounds checks beyond a six-entry static array.
- Sleep and input waits spin instead of blocking.
- Exception entry does not normalize CPU-pushed error codes or preserve a complete trap frame.
- The keyboard interrupt reads port `0x60` twice and writes to fixed buffers without complete bounds/wrap handling.
- FAT16 is bootloader-only and read-only; the kernel has no VFS or storage driver.
- There are no syscalls, users, permissions, ELF loader, libc, processes, IPC, networking, entropy, clocks, logging, tests, CI, license, or release engineering.
- README and architecture documentation are effectively absent.

### Baseline build result

The current build completed successfully with:

```text
nasm + gcc -m32 + ld -m elf_i386 + objcopy
mkfs.fat + mtools -> out/disk.img (16 MiB)
```

Warnings about RWX segments and executable-stack assumptions are release blockers, not cosmetic issues.

## 3. Production definition

v1.0 is “production” only when all of the following are true:

- Boots via UEFI on QEMU/KVM and at least three named reference-PC configurations.
- Supports x86_64, SMP, preemptive processes, virtual memory, Ring 3, NX/W^X, and an explicitly versioned syscall ABI.
- Runs CPython with files, sockets, TLS, threads, `asyncio`, SQLite, package installation, and isolated virtual environments.
- Provides a Python task manager with Inbox, Planned, Running, Waiting, Review, Done, Failed, and Cancelled states.
- A local model can classify, plan, summarize, and generate typed Python task bundles offline.
- A model router can call a larger remote model through a policy-controlled HTTPS provider adapter.
- Generated Python runs with CPU, memory, time, filesystem, network, secret, and device capabilities denied by default.
- Three or more nodes form an authenticated cluster, schedule DAG work, retain checkpoints, and distribute immutable artifacts.
- The cluster survives loss of one worker and one storage replica without acknowledged-data loss.
- Signed A/B updates roll back automatically after failed health checks.
- Reproducible builds, SBOMs, fuzzing, QEMU integration tests, hardware regression tests, crash dumps, metrics, security response, and restore drills are routine.

Not in v1.0: consumer desktop polish, broad laptop support, Windows/macOS binary compatibility, arbitrary third-party kernel drivers, or transparently sharding one huge model across weak LAN-connected CPUs.

## 4. Target architecture

Open the standalone diagram: [AI-native architecture](docs/ai-native-os-architecture.html).

```text
UEFI -> AriOS loader -> x86_64 monolithic kernel
                         | memory, scheduler, IPC, VFS, net, security, drivers
                         v
                    stable syscall ABI
                         v
         native runtime substrate: libc shim, CPython, TLS, llama runtime
                         v
        trusted Python control plane: tasks, policy, model router, node agent
                         v
       isolated generated Python bundles -> artifacts/checkpoints/events
                         v
          authenticated cluster compute + replicated content storage
```

The kernel should remain monolithic and modular rather than becoming a microkernel during this program. A microkernel rewrite would invalidate too much of the existing work and enlarge the driver/IPC burden before Python can boot.

## 5. Work breakdown and estimates

The ordering below is a dependency graph. Work inside a phase can be parallelized only where interfaces are frozen. Each phase ends in a bootable release tag and preserves the previous milestone.

### P0 - Establish continuous VM delivery and trust the current kernel (568 h)

The first execution milestone is a CI-produced virtual machine, not a new kernel feature. Every accepted change must preserve a downloadable image that can be booted and diagnosed independently of a developer workstation.

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P0.01 | Pin a containerized cross-toolchain and every image/package version | 48 | Same raw disk image is produced from clean hosts |
| P0.02 | Add serial logging before VGA and subsystem-tagged boot markers | 40 | Headless CI observes stage 1, stage 2, and kernel-ready markers |
| P0.03 | Add QEMU smoke tests, timeout, and machine-readable success/failure exit | 72 | Every pull request proves the image boots or fails diagnostically |
| P0.04 | Package and publish raw IMG, VirtualBox VDI, and VMware VMDK outputs with checksums, manifest, and boot log | 48 | Main builds retain artifacts and version tags create downloadable GitHub Releases |
| P0.05 | Add LICENSE, ownership, contribution, security, and release policies | 24 | Repository has an explicit legal/security basis |
| P0.06 | Replace the 17-byte README with build, boot, VM-import, architecture, and debug documentation | 32 | New contributor boots the image unaided |
| P0.07 | Add warnings-as-errors, freestanding flags, linker map, symbols, and size budgets | 48 | No accidental host ABI usage |
| P0.08 | Remove RWX segments and executable-stack assumptions; define section permissions | 40 | `readelf` policy test passes |
| P0.09 | Add host-side unit tests for strings, FAT parsing, allocation metadata, and path parsing | 72 | Sanitizer-enabled host tests pass |
| P0.10 | Add fuzz harnesses for FAT16, LFN, boot metadata, and path parsing | 64 | Seed corpus and crash minimization work |
| P0.11 | Fix current interrupt, keyboard, bounds, error-path, leak, and scheduler-capacity defects | 64 | Targeted regressions pass |
| P0.12 | Establish ADRs, coding standard, formatters, static analysis, and review gates | 16 | CI enforces the repository rules |

QEMU is the required per-commit boot gate because it can run headlessly and expose deterministic serial/exit status. Native VirtualBox and VMware boot checks are a separate release-compatibility lane on self-hosted runners with those hypervisors installed. Add an OVA appliance only after the BIOS/IDE/RAM virtual-hardware profile is frozen; until then, publish VDI and VMDK disks plus explicit import settings.

**Milestone K0:** CI produces integrity-verifiable IMG, VDI, and VMDK artifacts; the current i386 image boots deterministically under QEMU and is release-tested on VirtualBox and VMware.

### P1 - UEFI and x86_64 product boot path (860 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P1.01 | Freeze the BIOS/i386 path as `legacy-i386`; define x86_64 platform ABI | 32 | Architecture ADR accepted |
| P1.02 | Implement a PE/COFF UEFI loader using UEFI 2.11 boot services | 120 | Loader starts in OVMF |
| P1.03 | Read GOP framebuffer, ACPI RSDP, UEFI memory map, SMBIOS, and entropy | 96 | Structured handoff printed and tested |
| P1.04 | Load an ELF64 kernel plus initramfs by pathname with hash verification | 96 | Corrupt kernel is rejected |
| P1.05 | Build four-level page tables and enter long mode safely | 120 | Higher-half kernel runs with NX enabled |
| P1.06 | Define a versioned boot-information structure and ownership rules | 48 | Loader/kernel compatibility tests pass |
| P1.07 | Add early serial, framebuffer panic screen, and boot trace buffer | 48 | Failures observable without debugger |
| P1.08 | Implement UEFI variable boot-success/rollback contract | 72 | Failed image returns to prior slot |
| P1.09 | Add OVMF secure-boot development keys and signed-image pipeline | 80 | Enrolled VM rejects unsigned image |
| P1.10 | Add BIOS/UEFI parity tests and migration documentation | 48 | Both paths remain bootable |
| P1.11 | Hardware-spike on two reference PCs before freezing interfaces | 100 | Firmware deviations documented |

**Milestone K1:** signed x86_64 kernel boots through UEFI on OVMF and two physical systems.

### P2 - Physical and virtual memory (1,900 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P2.01 | Parse/normalize UEFI and E820 maps; reserve firmware, MMIO, kernel, modules | 96 | Overlap/property tests pass |
| P2.02 | Implement page-frame allocator with zones and bitmap/buddy back end | 180 | Exhaustion/recovery stress passes |
| P2.03 | Implement kernel virtual allocator, mapping API, TLB invalidation, guard pages | 180 | Random map/unmap test passes |
| P2.04 | Implement slab/size-class allocator and large allocations | 220 | Long-running allocation stress has no corruption |
| P2.05 | Add per-process page tables and user/kernel address split | 180 | Cross-process isolation test passes |
| P2.06 | Implement page faults, copy-on-write, anonymous memory, lazy zero pages | 240 | Fork/COW stress passes |
| P2.07 | Implement file-backed `mmap`, shared mappings, and page cache contracts | 240 | Coherency tests pass |
| P2.08 | Implement swapping contract and pressure/OOM policy; defer swap backend if needed | 120 | Deterministic OOM behavior |
| P2.09 | Add NUMA/topology abstraction even if v1 schedules one NUMA domain | 80 | ABI does not block later NUMA |
| P2.10 | Add NX, W^X, SMAP/SMEP preparation, stack guards, ASLR primitives | 160 | Exploit-oriented negative tests pass |
| P2.11 | Add memory accounting, leak detector, poison/red zones, crash diagnostics | 120 | CI stress reports allocations by owner |
| P2.12 | Fuzz page-table and allocator state machines with a host model | 84 | Model and kernel traces agree |

**Milestone K2:** isolated 64-bit address spaces and production-grade kernel allocation.

### P3 - Interrupts, clocks, SMP, and synchronization (2,100 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P3.01 | Replace generated ad-hoc ISR code with assembly trap stubs and normalized frames | 120 | Every architectural exception decoded correctly |
| P3.02 | Implement APIC, x2APIC, IOAPIC, MSI/MSI-X; retain PIC only for legacy boot | 260 | IRQ routing passes under QEMU and hardware |
| P3.03 | Parse ACPI MADT/HPET/FADT and validate tables/checksums | 160 | Corrupt table tests fail safely |
| P3.04 | Implement monotonic/realtime clocks, HPET/TSC calibration, per-CPU timers | 180 | Drift and timer conformance measured |
| P3.05 | Bootstrap application processors and per-CPU state/stacks | 240 | All CPUs online/offline repeatedly |
| P3.06 | Implement atomics, spinlocks, mutexes, semaphores, RCU/read-mostly primitive | 280 | Lock correctness/stress suite passes |
| P3.07 | Implement interrupt-safe queues, deferred work, and softirq/workqueue equivalent | 180 | ISRs remain bounded and allocation-free |
| P3.08 | Implement inter-processor interrupts and TLB shootdown | 160 | Concurrent map/unmap stress passes |
| P3.09 | Add lock ordering, deadlock detector, watchdog/NMI diagnostics | 160 | Injected deadlocks yield actionable crash data |
| P3.10 | Add suspend/reboot/poweroff primitives for reference platforms | 120 | Automated power-cycle tests pass |
| P3.11 | SMP race fuzzing and weeks-long soak tests | 240 | No unexplained hangs/data races |

**Milestone K3:** stable multi-core interrupt and time foundation.

### P4 - Processes, scheduler, syscalls, and IPC (1,900 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P4.01 | Define process/thread IDs, credentials, lifecycle, parents, sessions, groups | 120 | State-machine tests cover every transition |
| P4.02 | Replace six-slot scheduler with per-CPU run queues and blocking states | 240 | Thousands of threads schedule safely |
| P4.03 | Add priorities, fairness, affinity, CPU quotas, and priority inheritance | 220 | Latency/fairness benchmarks meet targets |
| P4.04 | Implement syscall entry/exit (`syscall/sysret`) with safe user-copy routines | 180 | Invalid pointers never crash kernel |
| P4.05 | Freeze v0 syscall numbering, calling convention, errors, and compatibility policy | 80 | ABI tests generated from spec |
| P4.06 | Implement `spawn/exec`, `exit`, `wait`, `clone/thread`, and optional `fork` | 240 | Process lifecycle suite passes |
| P4.07 | Implement signals/exceptions, cancellation, timers, and core dumps | 220 | Faulted process cannot take down node |
| P4.08 | Implement pipes, local sockets, shared memory, event objects, futexes | 260 | IPC stress and cancellation pass |
| P4.09 | Implement handles/file descriptors, poll/select/epoll-style readiness | 200 | Async I/O conformance passes |
| P4.10 | Add process accounting, limits, namespaces/capability containers | 140 | Resource denial is deterministic |

**Milestone K4:** multiple isolated Ring 3 programs run, communicate, block, and fail independently.

### P5 - Executables, VFS, filesystems, and block I/O (2,390 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P5.01 | Implement ELF64 loader, relocations policy, shared objects, TLS, aux vector | 220 | Static and dynamic test programs run |
| P5.02 | Design VFS objects, path resolution, mount namespaces, permissions, caching | 240 | Reference model tests pass |
| P5.03 | Implement initramfs/CPIO read-only root and device pseudo-filesystem | 120 | Userspace boots without disk driver |
| P5.04 | Implement block layer, request queues, scatter/gather, flush/barrier semantics | 220 | Power-loss ordering tests pass in model |
| P5.05 | Harden FAT16/FAT32 for EFI/removable media with read/write and fuzzing | 180 | Malformed media cannot corrupt memory |
| P5.06 | Implement a journaled production root filesystem or a rigorously scoped ext4 subset | 560 | Crash-consistency suite passes thousands of cut points |
| P5.07 | Implement page cache, writeback, fsync, direct I/O contract, quotas | 260 | Durability semantics documented/measured |
| P5.08 | Add tmpfs, proc/task diagnostics FS, and configuration FS | 180 | System introspection works without special syscalls |
| P5.09 | Add filesystem encryption hooks and key lifecycle | 160 | Locked disk reveals no plaintext metadata beyond policy |
| P5.10 | Implement disk/image formatter, checker, repair, snapshot/export tools | 250 | Offline recovery drill succeeds |

**Milestone K5:** durable local storage supports packages, Python, logs, and crash-safe updates.

### P6 - Driver and hardware platform (3,100 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P6.01 | PCI/PCIe enumeration, BAR mapping, capabilities, MSI/MSI-X, hotplug model | 260 | Device tree inventory matches firmware |
| P6.02 | ACPI power, reboot, thermal, battery-minimal, and PCI routing support | 220 | Reference systems operate without firmware hacks |
| P6.03 | Virtio transport plus blk, net, console, RNG, balloon, and vsock | 420 | QEMU/KVM production profile passes |
| P6.04 | AHCI/SATA driver with NCQ/flush/error recovery | 320 | Storage fault injection passes |
| P6.05 | NVMe driver with multiqueue, MSI-X, reset, timeout, SMART basics | 440 | Sustained I/O and reset tests pass |
| P6.06 | Network drivers: e1000/e1000e plus one modern Intel/Realtek reference NIC | 420 | Line-rate/stress on reference hardware |
| P6.07 | USB xHCI core, HID keyboard, and mass storage | 420 | Cold/hotplug cycles pass |
| P6.08 | UEFI GOP/framebuffer console, font, scrolling, panic UI | 180 | Local recovery console works |
| P6.09 | Serial/UART and emergency debug transport | 100 | Hardware crash logs captured remotely |
| P6.10 | Device/driver binding, permissions, lifecycle, DMA API, IOMMU preparation | 220 | Unprivileged process cannot DMA arbitrary memory |
| P6.11 | Hardware compatibility lab, firmware matrix, driver soak automation | 100 | Published supported-hardware list |

**Milestone K6:** QEMU and reference PCs have reliable disk, network, console, entropy, and USB.

### P7 - Network stack and secure transport (2,650 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P7.01 | Network buffer ownership, interfaces, routing table, neighbor cache | 180 | Leak-free saturation tests |
| P7.02 | Ethernet, ARP, IPv4, ICMP, fragmentation/reassembly policy | 280 | Packet conformance/fuzz tests |
| P7.03 | UDP and socket API | 180 | Loss/reorder tests pass |
| P7.04 | TCP state machine, congestion, retransmit, windows, keepalive | 620 | Interop and multi-day stress pass |
| P7.05 | DHCP client/server-minimal, DNS resolver/cache, mDNS discovery | 280 | Node obtains identity without manual config |
| P7.06 | IPv6, NDP, SLAAC, DHCPv6-minimal, dual-stack policy | 360 | IPv6-only cluster works |
| P7.07 | Firewall, connection tracking-minimal, egress policy, rate limiting | 260 | Policy bypass tests fail closed |
| P7.08 | Port audited TLS/crypto library; entropy, certificates, hostname validation | 280 | Standard TLS test suites pass |
| P7.09 | HTTP/1.1 client/server primitives and streaming; defer HTTP/2 if necessary | 160 | Remote model API works reliably |
| P7.10 | Packet capture, counters, diagnostics, fuzzing, hostile-network testing | 50 | Actionable network support bundle |

**Milestone K7:** secure Internet and cluster networking works under failure and hostile input.

### P8 - Kernel and platform security (2,450 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P8.01 | Threat model, trust boundaries, security invariants, abuse cases | 120 | Reviewed threat model gates design |
| P8.02 | Users/groups, credentials, file permissions, ACL/capability model | 280 | Privilege tests cover every syscall |
| P8.03 | Enforce NX, W^X, SMEP, SMAP, stack canaries, ASLR/KASLR | 300 | Exploit regression suite passes |
| P8.04 | Secure random subsystem with health tests and reseeding | 180 | Statistical/boot-blocking policy documented |
| P8.05 | Signed boot chain, measured boot hooks, TPM 2.0 reference support | 360 | Attestation identifies exact image |
| P8.06 | Sandboxing: namespaces, syscall filters, resource controls, capability broker | 420 | Generated-code escape tests fail |
| P8.07 | Secrets service with non-exportable handles and audit events | 220 | Model never receives raw secrets by default |
| P8.08 | Signed packages/updates, key rotation, rollback protection | 280 | Compromised old key can be revoked |
| P8.09 | Static analysis, fuzzing, sanitizers-in-emulation, exploit triage | 180 | Security CI has enforced SLAs |
| P8.10 | Disclosure/CVE process, incident response, reproducible advisories | 110 | Tabletop incident completes |

**Milestone K8:** the OS can safely host untrusted generated code under a documented threat model.

### P9 - POSIX-like runtime and developer platform (3,200 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P9.01 | Specify the AriOS userspace ABI and supported POSIX subset | 160 | Versioned standards matrix |
| P9.02 | Port/build a small libc or implement the required libc surface | 620 | libc conformance subset passes |
| P9.03 | Dynamic linker, environment, locales-minimal, time zones, math, pthreads | 520 | Multi-threaded dynamic programs run |
| P9.04 | Shell/bootstrap utilities used only for recovery and development | 220 | System repair possible without Python packages |
| P9.05 | TTY/PTY, terminal modes, job control, console login | 260 | Interactive recovery shell works |
| P9.06 | Build SDK, headers, cross compiler, sysroot, debugger, symbols | 340 | Third-party native runtime cross-builds reproducibly |
| P9.07 | Port GDB stub or equivalent remote debugger and profiler sampling | 240 | Kernel and userspace debugging documented |
| P9.08 | Package/archive format, dependency metadata, signatures, atomic install | 340 | Interrupted install leaves prior state intact |
| P9.09 | Service manager contract and Python process supervisor bootstrap | 260 | Dependency/restart/health semantics tested |
| P9.10 | Compatibility and ABI test farm across releases | 240 | N-1 applications still run or fail explicitly |

**Milestone U0:** a stable non-Python runtime foundation exists for CPython and inference.

### P10 - Python-native userspace (2,600 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P10.01 | Select/pin CPython line and create AriOS platform triplet/config site | 120 | Cross-configuration is reproducible |
| P10.02 | Bring up minimal CPython with frozen stdlib on initramfs | 300 | `python -c` runs as PID 1 child |
| P10.03 | Implement/port `os`, files, mmap, subprocess, signal, selectors, threads, time | 480 | Core stdlib tests pass |
| P10.04 | Bring up sockets, `ssl`, `hashlib`, `asyncio`, HTTP client/server | 360 | TLS asyncio integration passes |
| P10.05 | Port SQLite and Python `sqlite3` with crash/locking tests | 220 | Task DB survives power-cut suite |
| P10.06 | Define AriOS wheel tags, offline installer, lockfiles, virtual environments | 300 | Reproducible Python environment install |
| P10.07 | Implement Python service manager, structured logs, config, health API | 260 | Services recover and expose health |
| P10.08 | Implement Python CLI/TUI and local web UI foundation | 220 | Same API drives all three surfaces |
| P10.09 | Port selected numeric/runtime dependencies with explicit native exceptions | 220 | Inference prerequisites benchmarked |
| P10.10 | Run and triage CPython regression suite; publish supported-module matrix | 120 | No silent unsupported modules |

**Milestone U1:** AriOS boots into a Python service environment with storage, networking, TLS, and SQLite.

### P11 - Single-node AI-native task system (4,000 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P11.01 | Define Task, Step, Run, Artifact, Approval, Bundle, ModelCall schemas | 180 | Migrations and invariants tested |
| P11.02 | Implement task lifecycle: Inbox/Planned/Running/Waiting/Review/Done/Failed/Cancelled | 260 | State-machine/property tests pass |
| P11.03 | Implement dependency DAG, retries, deadlines, budgets, cancellation, checkpoints | 360 | Failure-injection workflows converge |
| P11.04 | Implement CLI/TUI/web task views, event stream, logs, artifact inspection | 320 | Full workflow usable without shell access |
| P11.05 | Port a compact local inference runtime and Python binding | 620 | Reference quantized model runs offline |
| P11.06 | Model router with local/remote providers, policy, cost, latency, privacy labels | 340 | Sensitive data never egresses without policy |
| P11.07 | Remote GPT-like provider adapter over HTTPS with streaming/tool calls | 220 | Contract tests against mock and live opt-in endpoint |
| P11.08 | Typed tool/capability registry and deterministic tool-call protocol | 280 | Invalid calls rejected before execution |
| P11.09 | Generated Python bundle format: source, manifest, lock, tests, capabilities, provenance | 320 | Every executable bundle is content-addressed |
| P11.10 | Build-test-policy-sign-promote pipeline for generated bundles | 380 | Broken/malicious candidates never reach trusted state |
| P11.11 | Sandboxed executor with default-deny filesystem/network/secrets/devices | 420 | Escape/exfiltration suite fails closed |
| P11.12 | Human approval gates and immutable audit trail for external side effects | 180 | Replay explains who/model/tool changed what |
| P11.13 | Evaluation harness for planning quality, hallucination, recovery, and cost | 120 | Release gated by fixed eval set |

**Milestone AI1:** one AriOS node accepts a task, plans locally, optionally escalates, generates Python, executes safely, and returns auditable artifacts.

### P12 - Distributed compute plane (4,480 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P12.01 | Define node identity, enrollment token, mTLS certificate rotation, revocation | 280 | Unauthorized node cannot join |
| P12.02 | Node discovery on LAN plus explicit seed/join for routed networks | 180 | New node joins with one command/code |
| P12.03 | Implement membership, heartbeats, failure detector, epochs, drain/remove | 360 | Partition simulations have defined outcomes |
| P12.04 | Capability inventory: CPU, SIMD, RAM, disk, model, accelerator, trust labels | 220 | Scheduler sees accurate allocatable resources |
| P12.05 | Cluster task scheduler with placement, locality, quotas, fairness, backpressure | 520 | Mixed-workload benchmarks meet SLOs |
| P12.06 | Worker lease protocol, idempotency keys, at-least-once execution semantics | 340 | Duplicate delivery cannot duplicate side effects |
| P12.07 | Distributed DAG coordination, retries, speculative work, cancellation | 380 | Worker loss resumes from checkpoints |
| P12.08 | Streaming logs/events/results with bounded memory and replay | 240 | Disconnect/reconnect loses no committed events |
| P12.09 | Data-local scheduling and artifact prefetch/cache eviction | 280 | Network bytes and cold-start targets met |
| P12.10 | Map/reduce, batch inference, fan-out/fan-in, pipeline templates | 300 | Complex reference workloads scale across nodes |
| P12.11 | Distributed model-serving registry, health, routing, batching | 300 | Requests avoid unhealthy/saturated replicas |
| P12.12 | Optional collective communication and tensor/pipeline parallel research track | 420 | Go/no-go benchmark versus single strong node |
| P12.13 | Cluster admin, quotas, maintenance, rolling drain, version skew policy | 280 | One node upgrades without corrupting jobs |
| P12.14 | Chaos, partition, clock-skew, duplicate, and overload test harness | 380 | Documented safety/liveness matrix passes |

**Milestone AI2:** a cluster executes large task graphs across heterogeneous nodes. This milestone guarantees workflow parallelism, not efficient cross-node sharding of every model.

### P13 - Distributed durable storage (2,500 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P13.01 | Define immutable content-addressed blob/manifest API and hash policy | 160 | Same bytes always map to same verified ID |
| P13.02 | Local chunk store, atomic writes, checksums, garbage collection, quotas | 280 | Crash/ENOSPC tests preserve committed data |
| P13.03 | Replication protocol, placement, failure domains, repair queue | 360 | Replica loss heals automatically |
| P13.04 | Raft-backed metadata/catalog with snapshots and membership changes | 520 | Jepsen-style safety tests find no split-brain commit |
| P13.05 | Multi-part upload/download, resumability, ranges, deduplication | 240 | Large checkpoints resume after failures |
| P13.06 | Consistency contract, leases/locks, versioned mutable pointers | 200 | Concurrent update histories are explainable |
| P13.07 | Encryption at rest/in transit, per-tenant keys, rotation, secure erase policy | 220 | Revoked tenant loses access |
| P13.08 | Scrub, audit, rebalance, decommission, backup/export/restore | 260 | Full restore drill meets RPO/RTO |
| P13.09 | Optional erasure coding after replication is proven | 220 | Reconstruction verified under shard loss |
| P13.10 | Python SDK, task/artifact integration, observability and capacity planner | 40 | Operators can predict exhaustion |

**Milestone AI3:** task inputs, code bundles, outputs, and checkpoints remain durable across node loss.

### P14 - Production operations, qualification, and v1.0 (2,700 h)

| ID | Task | Hours | Exit evidence |
|---|---|---:|---|
| P14.01 | Installer/provisioner, disk layout, first-boot identity, cluster join | 280 | Bare reference PC becomes node repeatably |
| P14.02 | Signed A/B image builder, delta/full update, health rollback | 320 | Power loss at every update step is safe |
| P14.03 | Kernel/user/cluster version compatibility and staged rollout | 200 | Mixed N/N-1 cluster passes |
| P14.04 | Metrics, logs, traces, alerts, crash dumps, support bundles | 260 | SLO breach is diagnosable remotely |
| P14.05 | Backup, restore, disaster recovery, key recovery, node replacement runbooks | 220 | Quarterly drills meet declared objectives |
| P14.06 | Performance suite: boot, syscall, disk, network, Python, inference, cluster | 240 | Regressions block release |
| P14.07 | Reliability suite: 30/90-day soak, memory pressure, disk full, brownout | 360 | No unresolved severity-1 defects |
| P14.08 | Security audit, penetration test, supply-chain audit, threat-model refresh | 360 | Critical/high findings closed or explicitly accepted |
| P14.09 | Hardware qualification and published compatibility matrix | 180 | Every claimed configuration continuously tested |
| P14.10 | Operator/user documentation, API reference, tutorials, troubleshooting | 180 | Fresh operator deploys 3-node cluster unaided |
| P14.11 | Release candidate program, telemetry opt-in policy, support/SLA process | 100 | Release operations rehearsed |

**Milestone v1.0:** narrow production AI-native OS release.

## 6. Total effort and staffing

| Phase | Midpoint hours |
|---|---:|
| P0 Continuous VM delivery/current-kernel trust | 568 |
| P1 UEFI/x86_64 boot | 860 |
| P2 Memory | 1,900 |
| P3 SMP/interrupts/time | 2,100 |
| P4 Processes/syscalls/IPC | 1,900 |
| P5 Executables/storage/VFS | 2,390 |
| P6 Drivers/hardware | 3,100 |
| P7 Networking | 2,650 |
| P8 Security | 2,450 |
| P9 POSIX-like runtime | 3,200 |
| P10 CPython userspace | 2,600 |
| P11 Single-node AI | 4,000 |
| P12 Distributed compute | 4,480 |
| P13 Distributed storage | 2,500 |
| P14 Production qualification | 2,700 |
| **Total** | **37,398** |

Recommended core team after P1:

- 2 kernel/memory/scheduler engineers
- 2 driver/storage/network engineers
- 1 security/runtime/crypto engineer
- 2 Python/AI/distributed-systems engineers
- 1 test/release/SRE engineer

Before P4, adding many Python/AI engineers will not accelerate the critical path because Ring 3, virtual memory, syscalls, storage, and networking do not yet exist.

## 7. Critical path

```text
P0 CI-built, downloadable VM baseline
 -> P1 UEFI/x86_64
 -> P2 virtual memory
 -> P3 SMP/synchronization
 -> P4 Ring 3 + syscalls + processes
 -> P5 VFS + durable storage
 -> P6 essential drivers
 -> P7 sockets + TLS
 -> P9 libc/runtime
 -> P10 CPython
 -> P11 safe AI task system
 -> P12/P13 cluster compute and storage
 -> P14 production qualification
```

Security P8 starts during P1 and gates every later phase. It must not be postponed until after Python or model-generated code exists.

## 8. First 20 implementation issues

These are the only issues that should be opened for the first execution tranche. Keep each PR within the repository's existing review limits.

1. Document and freeze the current BIOS/IDE/RAM virtual-hardware contract at commit `a781297`.
2. Add the pinned containerized toolchain and one-command deterministic build.
3. Add COM1 boot markers to stage 1, stage 2, and the kernel.
4. Add the QEMU exit device, timeout, and boot-stage assertions.
5. Publish commit-addressed IMG, VDI, and VMDK artifacts, `SHA256SUMS`, build manifest, and boot log.
6. Add tagged GitHub Release packaging and the self-hosted VirtualBox/VMware compatibility lane.
7. Split linker sections and eliminate RWX/executable-stack warnings.
8. Replace exception handlers with normalized assembly trap frames.
9. Correct keyboard single-read behavior and ring-buffer bounds.
10. Add task-array bounds, task IDs/states, and safe task exit.
11. Replace busy-loop sleeping/input with a wait queue prototype.
12. Host-test duplicated string/memory/FAT functions.
13. Fuzz FAT path/LFN/cluster-chain parsing and fix all findings.
14. Add allocator invariants, ownership tags, and exhaustion tests.
15. Write ADR-001: x86_64 UEFI product target and legacy-i386 policy.
16. Define `BootInfoV1` independent of BIOS/UEFI.
17. Create a minimal OVMF UEFI loader that prints over serial.
18. Load ELF64 kernel sections with requested permissions.
19. Establish four-level paging, the higher-half kernel mapping, and NX enforcement.
20. Boot the first x86_64 K1 kernel in CI and on one reference PC.

Do not start CPython, LLM integration, cluster code, or a GUI in this tranche. Their required contracts do not exist yet.

## 9. Required ADRs before implementation crosses each gate

| ADR | Decision |
|---|---|
| ADR-001 | x86_64 UEFI product target and supported hardware matrix |
| ADR-002 | Monolithic modular kernel and in-kernel driver policy |
| ADR-003 | BootInfo and syscall ABI/versioning policy |
| ADR-004 | Physical/virtual allocator architecture |
| ADR-005 | Scheduler and SMP synchronization model |
| ADR-006 | VFS and production filesystem choice |
| ADR-007 | Native userspace exception policy under “Python-first” |
| ADR-008 | CPython version, port strategy, supported stdlib modules |
| ADR-009 | Generated-code sandbox/capability model |
| ADR-010 | Local inference runtime/model packaging |
| ADR-011 | Cluster consistency, delivery, and partition semantics |
| ADR-012 | Distributed storage replication and metadata consensus |
| ADR-013 | Secure boot, signing, update, rollback, and key rotation |

## 10. Quality gates applied to every milestone

- Reproducible clean build from pinned toolchain.
- Commit-addressed IMG, VDI, and VMDK artifacts with checksums, build manifest, and retained boot log.
- Unit/property/fuzz tests for parsers and state machines.
- QEMU boot/integration suite with deterministic serial assertions.
- Negative tests for invalid pointers, malformed disk/network input, exhaustion, and cancellation.
- No unexplained warnings, RWX mappings, unbounded buffers, or infinite waits without watchdog semantics.
- Documented ABI/data-format change and migration plan.
- Crash dump contains build ID, registers, trap frame, task, locks, and recent logs.
- Performance baseline compared with previous release.
- Security review proportional to privilege and input exposure.
- Manual hardware verification only after emulated tests pass.

## 11. Risks that can multiply the estimate

- Supporting arbitrary commodity hardware rather than a small reference matrix: +20,000 to 100,000+ hours.
- Building a modern graphical desktop/browser: +15,000 to 50,000+ hours.
- Implementing cryptography rather than porting an audited library: unacceptable security risk.
- Implementing a fully compatible ext4, POSIX, or Linux ABI: +10,000 to 50,000 hours each depending on scope.
- Efficiently sharding one large transformer across commodity nodes: separate research program; network topology and accelerators dominate.
- Literal zero-native userspace: incompatible with CPython and practical local inference.
- Changing architecture to ARM/RISC-V midstream: repeat much of P1, P3, and P6.
- Skipping fuzzing/crash-consistency/chaos work: shortens the schedule only by redefining “production” downward.

## 12. Primary technical references

- [UEFI Specification 2.11](https://uefi.org/specs/UEFI/2.11/)
- [Intel 64 and IA-32 Software Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [CPython configure and cross-compiling documentation](https://docs.python.org/3.12/using/configure.html)
- [CPython embedding documentation](https://docs.python.org/3.13/extending/embedding.html)
- [llama.cpp server and OpenAI-compatible endpoints](https://github.com/ggml-org/llama.cpp/blob/master/tools/server/README.md)
- [Raft extended paper](https://raft.github.io/raft.pdf)

## 13. Approval gate

Approval of this plan authorizes only P0 and the ADR/design work for P1. It does not authorize broad implementation, hardware flashing, deployment, or remote API credentials.

Before code work begins, approve or change these three assumptions:

1. Product architecture is **x86_64 UEFI**, while i386 BIOS becomes legacy/regression-only.
2. v1.0 hardware scope is **QEMU/KVM plus three named reference-PC configurations**, headless first.
3. “Python userspace” permits **CPython, libc/ABI shims, audited crypto, and the inference engine** as native runtime dependencies while all AriOS-owned services and generated programs are Python.

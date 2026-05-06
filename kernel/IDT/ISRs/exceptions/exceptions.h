#include "../../IDT.h"


//	0 (0x0)	Fault	#DE	
// No

// The Division Error occurs when dividing any number by 0 using the DIV or IDIV instruction,
// or when the division result is too large to be represented in the destination.
// Since a faulting DIV or IDIV instruction is very easy to insert anywhere in the code,
// many OS developers use this exception to test whether their exception handling code works.

// The saved instruction pointer points to the DIV or IDIV instruction which caused the exception.
void exception_division_error(void);

//	1 (0x1)	Fault/Trap	#DB	
// No

// The Debug exception occurs on the following conditions:

// Instruction fetch breakpoint (Fault)
// General detect condition (Fault)
// Data read or write breakpoint (Trap)
// I/O read or write breakpoint (Trap)
// Single-step (Trap)
// Task-switch (Trap)
// When the exception is a fault, the saved instruction pointer points to the instruction which caused the exception.
// When the exception is a trap, the saved instruction pointer points to the instruction after the instruction which caused the exception.

// Error code: The Debug exception does not set an error code. However,
// exception information is provided in the debug registers (CPU_Registers_x86#Debug_Registers).
void exception_debug(void);

//	2 (0x2)	Interrupt	-	
// No
void exception_non_maskable_interrupt(void);

//	3 (0x3)	Trap	#BP	
// No

// A Breakpoint exception occurs at the execution of the INT3 instruction.
// Some debug software replace an instruction by the INT3 instruction.
// When the breakpoint is trapped, it replaces the INT3 instruction with the original instruction,
// and decrements the instruction pointer by one.

// The saved instruction pointer points to the byte after the INT3 instruction.
void exception_breakpoint(void);

//	4 (0x4)	Trap	#OF	
// No

// An Overflow exception is raised when the INTO instruction is executed while the overflow bit in RFLAGS is set to 1.

// The saved instruction pointer points to the instruction after the INTO instruction.
void exception_overflow(void);

//	5 (0x5)	Fault	#BR 
// No

// This exception can occur when the BOUND instruction is executed.
// The BOUND instruction compares an array index with the lower and upper bounds of an array.
// When the index is out of bounds, the Bound Range Exceeded exception occurs.

// The saved instruction pointer points to the BOUND instruction which caused the exception.
void exception_bound_range_exceeded(void);

//	6 (0x6)	Fault	#UD	
// No

// The Invalid Opcode exception occurs when the processor tries to execute an invalid or undefined opcode,
// or an instruction with invalid prefixes. It also occurs in other cases, such as:

// The instruction length exceeds 15 bytes, but this only occurs with redundant prefixes.
// The instruction tries to access a non-existent control register (for example, mov cr6, eax).
// The UD instruction is executed.
// The saved instruction pointer points to the instruction which caused the exception.
void exception_invalid_opcode(void);

//	7 (0x7)	Fault	#NM	
// No

// The Device Not Available exception occurs when an FPU instruction is attempted but there is no FPU.
// This is not likely, as modern processors have built-in FPUs. However,
// there are flags in the CR0 register that disable the FPU/MMX/SSE instructions,
// causing this exception when they are attempted.
// This feature is useful because the operating system can detect when a user program uses -
// the FPU or XMM registers and then save/restore them appropriately when multitasking.

// The saved instruction pointer points to the instruction that caused the exception.
void exception_device_not_available(void);

//	8 (0x8)	Abort	#DF	
// Yes (Zero)

// A Double Fault occurs when an exception is unhandled or when an exception occurs while the CPU is trying to call an exception handler. Normally,
// two exception at the same time are handled one after another, but in some cases that is not possible. For example, if a page fault occurs,
// but the exception handler is located in a not-present page, two page faults would occur and neither can be handled. A double fault would occur.

// A double fault will always generate an error code with a value of zero.

// The saved instruction pointer is undefined. A double fault cannot be recovered. The faulting process must be terminated.

// In several starting hobby OSes, a double fault is also quite often a misdiagnosed IRQ0 in the cases where the PIC hasn't been reprogrammed yet.
void exception_double_fault(void);

//	9 (0x9)	Fault	-	
// No

// When the FPU was still external to the processor, it had separate segment checking in protected mode.
// Since the 486 this is handled by a GPF instead like it already did with non-FPU memory accesses.
void exception_coprocessor_segment_overrun(void);

//	10 (0xA)	Fault	#TS	
// Yes

// An Invalid TSS exception occurs when an invalid segment selector is referenced as part of a task switch,
// or as a result of a control transfer through a gate descriptor,
// which results in an invalid stack-segment reference using an SS selector in the TSS.

// When the exception occurred before loading the segment selectors from the TSS,
// the saved instruction pointer points to the instruction which caused the exception.
// Otherwise, and this is more common, it points to the first instruction in the new task.

// Error code: The Invalid TSS exception sets an error code, which is a selector index.
void exception_invalid_tss(void);

//	11 (0xB)	Fault	#NP	
// Yes

// The Segment Not Present exception occurs when trying to load a segment or gate which has its `Present` bit set to 0.
// However when loading a stack-segment selector which references a descriptor which is not present, a Stack-Segment Fault occurs.

// If the exception happens during a hardware task switch, the segment values should not be relied upon by the handler.
// That is, the handler should check them before trying to resume the new task. There are three ways to do this, according to the Intel documentation.

// The saved instruction pointer points to the instruction which caused the exception.

// Error code: The Segment Not Present exception sets an error code, which is the segment selector index of the segment descriptor which caused the exception.
void exception_segment_not_present(void);

//	12 (0xC)	Fault	#SS	
// Yes

// The Stack-Segment Fault occurs when:

// Loading a stack-segment referencing a segment descriptor which is not present.
// Any PUSH or POP instruction or any instruction using ESP or EBP as a base register is executed,
// while the stack address is not in canonical form.
// When the stack-limit check fails.
// If the exception happens during a hardware task switch, the segment values should not be relied upon by the handler.
// That is, the handler should check them before trying to resume the new task. There are three ways to do this,
// according to the Intel documentation.

// The saved instruction pointer points to the instruction which caused the exception,
// unless the fault occurred because of loading a non-present stack segment during a hardware task switch,
// in which case it points to the next instruction of the new task.

// Error code: The Stack-Segment Fault sets an error code,
// which is the stack segment selector index when a non-present segment descriptor was referenced or a limit check failed during a hardware task switch.
// Otherwise (for present segments and already in use), the error code is 0.
void exception_stack_segment_fault(void);

//	13 (0xD)	Fault	#GP	
// Yes

// A General Protection Fault may occur for various reasons. The most common are:

// Segment error (privilege, type, limit, read/write rights).
// Executing a privileged instruction while CPL != 0.
// Writing a 1 in a reserved register field or writing invalid value combinations (e.g. CR0 with PE=0 and PG=1).
// Referencing or accessing a null-descriptor.
// Accessing a memory address with bits 48-63 not matching bit 47 (e.g. 0x_0000_8000_0000_0000 instead of 0x_ffff_8000_0000_0000) in 64 bit mode.
// Executing an instruction that requires memory operands to be aligned (e.g. movaps) without the proper alignment.
// The saved instruction pointer points to the instruction which caused the exception.

// Error code: The General Protection Fault sets an error code, which is the segment selector index when the exception is segment related. Otherwise, 0.
void exception_general_protection_fault(void);

//	14 (0xE)	Fault	#PF	
// Yes

// A Page Fault occurs when:

// A page directory or table entry is not present in physical memory.
// Attempting to load the instruction TLB with a translation for a non-executable page.
// A protection check (privileges, read/write) failed.
// A reserved bit in the page directory or table entries is set to 1.
// The saved instruction pointer points to the instruction which caused the exception.

// Error code
// The Page Fault sets an error code:

//  31              15                             4               0
// +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
// |   Reserved   | SGX |   Reserved   | SS | PK | I | R | U | W | P |
// +---+--  --+---+-----+---+--  --+---+----+----+---+---+---+---+---+
// Length	Name	Description
// P	1 bit	Present	When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.
// W	1 bit	Write	When set, the page fault was caused by a write access. When not set, it was caused by a read access.
// U	1 bit	User	When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
// R	1 bit	Reserved write	When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1.
// Note that this includes invalid physical addresses (e.g. 1<<40 on a processor with only 39 physical bits).

// I	1 bit	Instruction Fetch	When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled.
// PK	1 bit	Protection key	When set, the page fault was caused by a protection-key violation. The PKRU register (for user-mode accesses) or PKRS MSR (for supervisor-mode accesses) specifies the protection key rights.
// SS	1 bit	Shadow stack	When set, the page fault was caused by a shadow stack access.
// SGX	1 bit	Software Guard Extensions	When set, the fault was due to an SGX violation. The fault is unrelated to ordinary paging.
// In addition, it sets the value of the CR2 register to the virtual address which caused the Page Fault.
void exception_page_fault(void);

//	15 (0xF)	-	-	
// No
void exception_reserved(void);

//	16 (0x10)	Fault	#MF	
// No

// The x87 Floating-Point Exception occurs when the FWAIT or WAIT instruction,
// or any waiting floating-point instruction is executed, and the following conditions are true:

// CR0.NE is 1;
// an unmasked x87 floating point exception is pending (i.e. the exception bit in the x87 floating point status-word register is set to 1).
// The saved instruction pointer points to the instruction which is about to be executed when the exception occurred.
// The x87 instruction pointer register contains the address of the last instruction which caused the exception.

// Error Code: The exception does not push an error code. However, exception information is available in the x87 status word register.
void exception_x87_floating_point(void);

//	17 (0x11)	Fault	#AC	
// Yes

// An Alignment Check exception occurs when alignment checking is enabled and an unaligned memory data reference is performed.
// Alignment checking is only performed in CPL 3.

// Alignment checking is disabled by default. To enable it, set the CR0.AM and RFLAGS.AC bits both to 1.

// The saved instruction pointer points to the instruction which caused the exception.
void exception_alignment_check(void);

//	18 (0x12)	Abort	#MC	
// No

// The Machine Check exception is model specific and processor implementations are not required to support it.
// It uses model-specific registers to provide error information. It is disabled by default. To enable it, set the CR4.MCE bit to 1.

// Machine check exceptions occur when the processor detects internal errors, such as bad memory, bus errors, cache errors, etc.

// The value of the saved instruction pointer depends on the implementation and the exception.
void exception_machine_check(void);

//	19 (0x13)	Fault	#XM/#XF	
// No

// The SIMD Floating-Point Exception occurs when an unmasked 128-bit media floating-point exception occurs and the CR4.OSXMMEXCPT bit is set to 1.
// If the OSXMMEXCPT flag is not set, then SIMD floating-point exceptions will cause an Undefined Opcode exception instead of this.

// The saved instruction pointer points to the instruction which caused the exception.

// Error Code: The exception does not push an error code. However, exception information is available in the MXCSR register.
void exception_simd_floating_point(void);

//	20 (0x14)	Fault	#VE	
// No
void exception_virtualization(void);

//	21 (0x15)	Fault	#CP	
// Yes
void exception_control_protection(void);

//	22-27 (0x16-0x1B)	-	-	
// No
void Reserved();

//	28 (0x1C)	Fault	#HV	
// No
void exception_hypervisor_injection(void);

//	29 (0x1D)	Fault	#VC	
// Yes
void exception_vmm_communication(void);

//	30 (0x1E)	Fault	#SX	
// Yes
void exception_security(void);

//	31 (0x1F)	-	-	
// No
void Reserved();

//	-	-	-	
// No

// The Triple Fault is not really an exception, because it does not have an associated vector number.
// Nonetheless, a triple fault occurs when an exception is generated when attempt to call the double fault exception handler.
// It results in the processor resetting. See the main article for more information about possible causes and how to avoid them.

// Selector Error Code
//  31         16   15         3   2   1   0
// +---+--  --+---+---+--  --+---+---+---+---+
// |   Reserved   |    Index     |  Tbl  | E |
// +---+--  --+---+---+--  --+---+---+---+---+
// Length	Name	Description
// E	1 bit	External	When set, the exception originated externally to the processor.
// Tbl	2 bits	IDT/GDT/LDT table	This is one of the following values:
// Value	Description
// 0b00	The Selector Index references a descriptor in the GDT.
// 0b01	The Selector Index references a descriptor in the IDT.
// 0b10	The Selector Index references a descriptor in the LDT.
// 0b11	The Selector Index references a descriptor in the IDT.
// Index	13 bits	Selector Index	The index in the GDT, IDT or LDT.

void Triple_Fault();

//	IRQ 13	Interrupt	#FERR
// No

// In the old days, the floating point unit was a dedicated chip that could be attached to the processor.
// It lacked direct wiring of FPU errors to the processor, so instead it used IRQ 13,
// allowing the CPU to deal with errors at its own leasure. When the 486 was developed and multiprocessor support was added,
// the FPU was embedded on die and a global interrupt for FPUs became undesirable, instead getting an option for direct error handling.
// By default, this method is not enabled at boot for backwards compatibility, but an OS should update the settings accordingly.
void FPU_Error_Interrupt();


void setup_exceptions();


// typedef enum exception
// {

// Division_Error = 0x00,
// Debug = 0x01,
// Non_maskable_Interrupt = 0x02,
// Breakpoint = 0x03,
// Overflow = 0x04,
// Bound_Range_Exceeded = 0x05,
// Invalid_Opcode = 0x06,
// Device_Not_Available = 0x07,
// Double_Fault = 0x08,
// Coprocessor_Segment_Overrun = 0x09,
// Invalid_TSS = 0x0A,
// Segment_Not_Present = 0x0AB,
// Stack_Segment_Fault = 0x0C,
// General_Protection_Fault = 0x0D,
// Page_Fault = 0x0E,
// Reserved_1 = 0x0F,
// x87_Floating_Point_Exception = 0x10,
// Alignment_Check = 0x11,
// Machine_Check = 0x12,
// SIMD_Floating_Point_Exception = 0x13,
// Virtualization_Exception = 0x14,
// Control_Protection_Exception = 0x15,
// Reserved_2 = 0x16,
// Reserved_3 = 0x17,
// Reserved_4 = 0x18,
// Reserved_5 = 0x19,
// Reserved_6 = 0x1A,
// Reserved_7 = 0x1B,
// Hypervisor_Injection_Exception = 0x1C,
// VMM_Communication_Exception = 0x1D,
// Security_Exception = 0x1E,
// Reserved = 0x1F,
// // Triple_Fault = 0x??,
// // FPU_Error_Interrupt = 0x??,
// } exception;
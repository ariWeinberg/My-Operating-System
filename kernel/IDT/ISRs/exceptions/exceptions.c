#include "../../../SCREEN_DRIVER/screen_driver.h"
#include "../../../UTILS/asm_utils.h"

void exception_division_error()
{
    print_string("Error: Division_Error (0x00) exception occured! halting...");
    halt();
}
void exception_debug()
{
    print_string("Error: Debug (0x01) exception occured! halting...");
    halt();
}
void exception_non_maskable_interrupt()
{
    print_string("Error: Non_maskable_Interrupt (0x02) exception occured! halting...");
    halt();
}
void exception_breakpoint()
{
    print_string("Error: Breakpoint (0x03) exception occured! halting...");
    halt();
}
void exception_overflow()
{
    print_string("Error: Overflow (0x04) exception occured! halting...");
    halt();
}
void exception_bound_range_exceeded()
{
    print_string("Error: Bound_Range_Exceeded (0x05) exception occured! halting...");
    halt();
}
void exception_invalid_opcode()
{
    print_string("Error: Invalid_Opcode (0x06) exception occured! halting...");
    halt();
}
void exception_device_not_available()
{
    print_string("Error: Device_Not_Available (0x07) exception occured! halting...");
    halt();
}
void exception_double_fault()
{
    print_string("Error: Double_Fault (0x08) exception occured! halting...");
    halt();
}
void exception_coprocessor_segment_overrun()
{
    print_string("Error: Coprocessor_Segment_Overrun (0x09) exception occured! halting...");
    halt();
}
void exception_invalid_tss()
{
    print_string("Error: Invalid_TSS (0x0A) exception occured! halting...");
    halt();
}
void exception_segment_not_present()
{
    print_string("Error: Segment_Not_Present (0x0B) exception occured! halting...");
    halt();
}
void exception_stack_segment_fault()
{
    print_string("Error: Stack_Segment_Fault (0x0C) exception occured! halting...");
    halt();
}
void exception_general_protection_fault()
{
    print_string("Error: General_Protection_Fault (0x0D) exception occured! halting...");
    halt();
}
void exception_page_fault()
{
    print_string("Error: Page_Fault (0x0E) exception occured! halting...");
    halt();
}
void exception_reserved()
{
    print_string("Error: Reserved (0x0F, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1F) exception occured! halting...");
    halt();
}
void exception_x87_floating_point()
{
    print_string("Error: x87_Floating_Point_Exception (0x10) exception occured! halting...");
    halt();
}
void exception_alignment_check()
{
    print_string("Error: Alignment_Check (0x11) exception occured! halting...");
    halt();
}
void exception_machine_check()
{
    print_string("Error: Machine_Check (0x12) exception occured! halting...");
    halt();
}
void exception_simd_floating_point()
{
    print_string("Error: SIMD_Floating_Point_Exception (0x13) exception occured! halting...");
    halt();
}
void exception_virtualization()
{
    print_string("Error: Virtualization_Exception (0x14) exception occured! halting...");
    halt();
}
void exception_control_protection()
{
    print_string("Error: Control_Protection_Exception (0x15) exception occured! halting...");
    halt();
}
void exception_hypervisor_injection()
{
    print_string("Error: Hypervisor_Injection_Exception (0x1C) exception occured! halting...");
    halt();
}
void exception_vmm_communication()
{
    print_string("Error: VMM_Communication_Exception (0x1D) exception occured! halting...");
    halt();
}
void exception_security()
{
    print_string("Error: Security_Exception (0x1E) exception occured! halting...");
    halt();
}
// void Triple_Fault()
// {
//     print_string("Error: Triple_Fault (0x19) exception occured! halting...");
//     halt();
// }
// void FPU_Error_Interrupt()
// {
//     print_string("Error: FPU_Error_Interrupt (0x1A) exception occured! halting...");
//     halt();
// }


static void kernel_halt(void)
{
    interrupts_disable();
    while (1);
}

#define GDT_SELECTOR 0x08
#define GATE_HANDLED_MASK 0x8E
#define SET_IDT_ENTRY(EXCEPTION, HANDLER) idt_set_entry(EXCEPTION, (uint32_t)HANDLER, GDT_SELECTOR, GATE_HANDLED_MASK, 0);

void setup_exceptions()
{
    SET_IDT_ENTRY(0x00, exception_division_error)
    SET_IDT_ENTRY(0x01, exception_debug)
    SET_IDT_ENTRY(0x02, exception_non_maskable_interrupt)
    SET_IDT_ENTRY(0x03, exception_breakpoint)
    SET_IDT_ENTRY(0x04, exception_overflow)
    SET_IDT_ENTRY(0x05, exception_bound_range_exceeded)
    SET_IDT_ENTRY(0x06, exception_invalid_opcode)
    SET_IDT_ENTRY(0x07, exception_device_not_available)
    SET_IDT_ENTRY(0x08, exception_double_fault)
    SET_IDT_ENTRY(0x09, exception_coprocessor_segment_overrun)
    SET_IDT_ENTRY(0x0A, exception_invalid_tss)
    SET_IDT_ENTRY(0x0B, exception_segment_not_present)
    SET_IDT_ENTRY(0x0C, exception_stack_segment_fault)
    SET_IDT_ENTRY(0x0D, exception_general_protection_fault)
    SET_IDT_ENTRY(0x0E, exception_page_fault)
    SET_IDT_ENTRY(0x0F, exception_reserved)
    SET_IDT_ENTRY(0x10, exception_x87_floating_point)
    SET_IDT_ENTRY(0x11, exception_alignment_check)
    SET_IDT_ENTRY(0x12, exception_machine_check)
    SET_IDT_ENTRY(0x13, exception_simd_floating_point)
    SET_IDT_ENTRY(0x14, exception_virtualization)
    SET_IDT_ENTRY(0x15, exception_control_protection)
    SET_IDT_ENTRY(0x16, exception_reserved)
    SET_IDT_ENTRY(0x17, exception_reserved)
    SET_IDT_ENTRY(0x18, exception_reserved)
    SET_IDT_ENTRY(0x19, exception_reserved)
    SET_IDT_ENTRY(0x1A, exception_reserved)
    SET_IDT_ENTRY(0x1B, exception_reserved)
    SET_IDT_ENTRY(0x1C, exception_hypervisor_injection)
    SET_IDT_ENTRY(0x1D, exception_vmm_communication)
    SET_IDT_ENTRY(0x1E, exception_security)
    SET_IDT_ENTRY(0x1F, exception_reserved)
}


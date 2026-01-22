# general details
1.) entry point is bootloader_entry
2.) output format is binary
3.) target system is i386
4.) we start in 16 bit mode and then switch to 32 bit mode
5.) bootloader start is at segment 0x7c00 offset 0


# test bootloader
1.) zero registers and setup a stack at 0x7A00
2.) print "hello world"
3.) print "hello world - no bios!" without using bios interupts
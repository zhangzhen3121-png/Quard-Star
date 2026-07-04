    .section .text.ENTRY
    .global _start

_start:

    la sp, boot_stack_top
    call os_main
_loop:
    j _loop

    .section .bss.stack
    .global boot_stack_low_bound


boot_stack_low_bound:
    .space 4096*16
    .global boot_stack_top
boot_stack_top:

    .end
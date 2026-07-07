OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    ram(rxai!w) : ORIGIN = 0x80200000, LENGTH = 128M
}

SECTIONS
{
    .text : {
        *(.text .text.*)
    . = ALIGN(0x1000);
    PROVIDE(text_end = .);
    } > ram

    .rodata : {
        *(.rodata .rodata.*)
    } > ram

    .data : {
        . = ALIGN(4096);
        *(.sdata .sdata.*)
        *(.data .data.*)
        PROVIDE(_data_end = .);
    } > ram

    .bss : {
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
    } > ram
    . = ALIGN(0x1000);
    PROVIDE(kernal_end = .);
}

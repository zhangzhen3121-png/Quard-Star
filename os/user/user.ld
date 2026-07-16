OUTPUT_ARCH(riscv)
ENTRY(main)
BASE_ADDRESS = 0x10000;

SECTIONS
{
    . = BASE_ADDRESS;
    .text : {
        *(.text.entry)
        *(.text .text.*)
    }

    . = ALIGN(4k);
    .rodata : {
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
    }   

    . = ALIGN(4k);
    .data : {
        *(.data .data.*)
        *(.sdata .sdata.*)
    }

    . = ALIGN(4k);
    .bss : {
        *(.bss .bss.*)
        *(.sbss .sbss.*) 
    }

    /DISCARD/ : {
        *(.eh_frame)
        *(.debug*)
    }
}
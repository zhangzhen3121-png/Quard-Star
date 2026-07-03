OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY{
   ddr(rxai!w):ORIGIN = 0x80000000, LENGTH = 256M
}

SECTIONS{
    .text : {
        KEEP(*(.text))
    } > ddr
}
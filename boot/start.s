    .section .text
    .global _start
    .type _start,@function

_start:
    csrr    a0, mhartid

    li      t0, 0x0
    beq     t0, a0, _core_0
_loop:
    j       _loop
_core_0:
    li      t0, 0x100
    slli    t0, t0, 20
    li      t1, 'H'
    sb      t1, 0(t0)
    li      t1, 'E'
    sb      t1, 0(t0)
    li      t1, 'L'
    sb      t1, 0(t0)
    li      t1, 'L'
    sb      t1, 0(t0)
    li      t1, 'O'
    sb      t1, 0(t0)
    
    j       _loop

    .end
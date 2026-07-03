    .section .text
    .global _start
    .type _start,@function

_start:
    li    t0, 0x100
    slli  t0, t0, 20      //t1 = 0x10000000
    li    t1, 0x200
    add  t1, t1, t0      //t1 = 0x10000200

    li t0 , 't'
    sd   t0, 0(t1)
    li t0 , 'r'
    sd   t0, 0(t1)
    li t0 , 'u'
    sd   t0, 0(t1)
    li t0 , 's'
    sd   t0, 0(t1)
    li t0 , 't'
    sd   t0, 0(t1)
    li t0 , 'e'
    sd   t0, 0(t1)
    li t0 , 'd'
    sd   t0, 0(t1)
    li t0 , '_'
    sd   t0, 0(t1)
    li t0 , 'd'
    sd   t0, 0(t1)
    li t0 , '0'
    sd   t0, 0(t1)
    li t0 , 'm'
    sd   t0, 0(t1)
    li t0 , 'a'
    sd   t0, 0(t1)
    li t0 , 'i'
    sd   t0, 0(t1)
    li t0 , 'n'
    sd   t0, 0(t1)

_loop:
    j _loop
    .end

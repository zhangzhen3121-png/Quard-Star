    .section .text
    .global _start
    .type _start,@function

_start:
    li    t0, 0x10002
    slli  t1, t0, 12      //t1 = 0x10002000, uart2 THR

    li t0 , 't'
    sb   t0, 0(t1)
    li t0 , 'r'
    sb   t0, 0(t1)
    li t0 , 'u'
    sb   t0, 0(t1)
    li t0 , 's'
    sb   t0, 0(t1)
    li t0 , 't'
    sb   t0, 0(t1)
    li t0 , 'e'
    sb   t0, 0(t1)
    li t0 , 'd'
    sb   t0, 0(t1)
    li t0 , '_'
    sb   t0, 0(t1)
    li t0 , 'd'
    sb   t0, 0(t1)
    li t0 , '0'
    sb   t0, 0(t1)
    li t0 , 'm'
    sb   t0, 0(t1)
    li t0 , 'a'
    sb   t0, 0(t1)
    li t0 , 'i'
    sb   t0, 0(t1)
    li t0 , 'n'
    sb   t0, 0(t1)
    li t0 , '\n'
    sb   t0, 0(t1)

_loop:
    j _loop
    .end

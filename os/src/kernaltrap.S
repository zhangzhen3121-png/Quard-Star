.section .text.trampline
.global _alltrap
.align 4
_alltrap:

    csrrw sp, sscratch, sp
    addi sp, sp, -34*8

    sd x1, 1*8(sp)
    #sd x2, 2*8(sp)
    sd x3, 3*8(sp)
    sd x4, 4*8(sp)
    sd x5, 5*8(sp)
    sd x6, 6*8(sp)
    sd x7, 7*8(sp)
    sd x8, 8*8(sp)
    sd x9, 9*8(sp)
    sd x10, 10*8(sp)
    sd x11, 11*8(sp)
    sd x12, 12*8(sp)
    sd x13, 13*8(sp)
    sd x14, 14*8(sp)
    sd x15, 15*8(sp)
    sd x16, 16*8(sp)
    sd x17, 17*8(sp)
    sd x18, 18*8(sp)
    sd x19, 19*8(sp)
    sd x20, 20*8(sp)
    sd x21, 21*8(sp)
    sd x22, 22*8(sp)
    sd x23, 23*8(sp)
    sd x24, 24*8(sp)
    sd x25, 25*8(sp)
    sd x26, 26*8(sp)
    sd x27, 27*8(sp)
    sd x28, 28*8(sp)
    sd x29, 29*8(sp)
    sd x30, 30*8(sp)
    sd x31, 31*8(sp)

    csrr t0, sstatus
    csrr t1, sepc
    sd t0, 32*8(sp)
    sd t1, 33*8(sp)

    ld t0, 34*8(sp) //skernal_satp
    ld t1, 36*8(sp) //trap_handler

    csrr t2, sscratch
    sd t2, 2*8(sp)
    
    ld sp, 35*8(sp) //skernal_sp

    csrw satp, t0
    sfence.vma

    jr   t1


.global _restore
.align 4
_restore:
    /*返回用户空间时，有两个参数，a0  用户空间的trap_context上下文地址,所有进程用户空间都一样，为次高页帧，
                                a1  返回的用户空间的根页表物理地址，保存在TCB块中
    */
    csrw satp, a1
    sfence.vma
    csrw sscratch, a0
    mv a0, sp

    ld t0, 32*8(sp)
    ld t1, 33*8(sp)

    csrw sstatus, t0
    csrw sepc, t1

    ld x1, 1*8(sp)
    # ld x2, 2*8(sp)
    ld x3, 3*8(sp)
    ld x4, 4*8(sp)
    ld x5, 5*8(sp)
    ld x6, 6*8(sp)
    ld x7, 7*8(sp)
    ld x8, 8*8(sp)
    ld x9, 9*8(sp)
    ld x10, 10*8(sp)
    ld x11, 11*8(sp)
    ld x12, 12*8(sp)
    ld x13, 13*8(sp)
    ld x14, 14*8(sp)
    ld x15, 15*8(sp)
    ld x16, 16*8(sp)
    ld x17, 17*8(sp)
    ld x18, 18*8(sp)
    ld x19, 19*8(sp)
    ld x20, 20*8(sp)
    ld x21, 21*8(sp)
    ld x22, 22*8(sp)
    ld x23, 23*8(sp)
    ld x24, 24*8(sp)
    ld x25, 25*8(sp)
    ld x26, 26*8(sp)
    ld x27, 27*8(sp)
    ld x28, 28*8(sp)
    ld x29, 29*8(sp)
    ld x30, 30*8(sp)
    ld x31, 31*8(sp)

    ld sp, 2*8(sp)
    
    sret

    .end
#include"context.h"
#include"riscv.h"
extern void _alltrap();
extern void _restore(pt_regs* next);

pt_regs* trap_hanlder(pt_regs* cx){
    reg_t cause = r_scause();
    printf("cause:%x \n",cause);
    printf("a7:   %x \n",cx->a7);
    printf("a0:   %x \n",cx->a0);
    printf("a1:   %x \n",cx->a1);
    printf("a2:   %x \n",cx->a2);
    printf("sp:   %x \n",cx->sp);
    printf("sstatus:    %x \n",cx->sstatus);
    printf("sepc:       %x \n",cx->sepc);
    cx->sepc+=8;
    _restore(cx);
    return cx;
}

void trap_init(){
    w_stvec((reg_t)_alltrap);
}
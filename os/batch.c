#include"os.h"
#include"context.h"
#include"riscv.h"


#define USER_STACK_SIZE     4096*2
#define KERNAL_STACK_SIZE   4096*2

uint8_t USER_STACK[USER_STACK_SIZE];
uint8_t KERNAL_STACK[KERNAL_STACK_SIZE];

extern void _restore(pt_regs * cx);
extern void trap_init();

pt_regs tasks;


size_t syscall(size_t id,reg_t arg1,reg_t arg2, reg_t arg3){
    long ret;

    asm volatile(
        "mv a7, %1\n\t"
        "mv a0, %2\n\t"
        "mv a1, %3\n\t"
        "mv a2, %4\n\t"
        "ecall\n\t"
        "mv %0, a0\n\t"
        :"=r"(ret)
        :"r"(id),"r"(arg1),"r"(arg2),"r"(arg3)
        :"a7","a0","a1","a2","memory"
    );
    return ret;
}

void systest(){
    char* test_str = "hello kernal\r\n";
    syscall(__NR_write,1,test_str,strlen(test_str));
    while (1)
    {
        /* code */
    }
}


void app_init_context(){
    
    reg_t u_stack_sp = USER_STACK+USER_STACK_SIZE;
    
    trap_init();

    reg_t s_status = r_sstatus();
    s_status  &= (0U<<8);
    w_sstatus(s_status);

    tasks.sstatus = s_status;
    tasks.sepc = (reg_t)systest;
    tasks.sp = u_stack_sp;

    pt_regs* ctx = KERNAL_STACK+KERNAL_STACK_SIZE-sizeof(pt_regs);
    ctx->sepc = tasks.sepc;
    ctx->sstatus = tasks.sstatus;
    ctx->sp = tasks.sp;
    
    _restore(ctx);
}
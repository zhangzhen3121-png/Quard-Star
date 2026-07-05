#include"context.h"
#include"riscv.h"
#include"os.h"


extern void set_next_trigger();

void __sys_write(size_t fd, const char* buf, size_t len){
    if(fd==1){
        printf(buf);
    }
    else{
        printf("unsupport fd id sys_wirte \r\n");
    }
}

void __sys_yield(){
    schedule();
}


void __SYSCALL(size_t id, reg_t arg1, reg_t arg2, reg_t arg3){
    switch (id)
    {
    case __NR_write:
        __sys_write(arg1, (const char*)arg2, arg3);
        break;
    case __NR_sched_yield:
        __sys_yield();
        break;
    default:
        printf("unsupport syscall id\r\n");
        break;
    }
}

pt_regs* trap_hanlder(pt_regs* cx){
    reg_t cause = r_scause();
    reg_t code = cause & 0xFFF;

    if(cause>>63){
        switch (code)
        {
        case 5:
            /* RTC IRQ */
            set_next_trigger();
            schedule();
            break;
        default:
            printf("undefined interrput code\r\n");
            break;
        }
    }
    else{
        switch (code)
        {
        case 8:
            __SYSCALL(cx->a7,cx->a0,cx->a1,cx->a2);
            cx->sepc+=4;
            break;
        default:
            printf("undefined scause\r\n");
            break;
        }
    }
    return cx;
}


void trap_init(){
    w_stvec((reg_t)_alltrap);
}

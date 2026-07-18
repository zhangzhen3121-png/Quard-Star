#ifndef __CONTEXT_H
#define __CONTEXT_H

#include"types.h"


typedef struct pt_regs_t
{   //在进入用户态时，上下文栈顶指针存放在sscratch寄存器
    reg_t x0;
    reg_t ra;
    reg_t sp;//在进入内核态时保存用户栈指针
    reg_t gp;
    reg_t tp;
    reg_t t0;
    reg_t t1;
    reg_t t2;
    reg_t s0;
    reg_t s1;
    reg_t a0;
    reg_t a1;
    reg_t a2;
    reg_t a3;
    reg_t a4;
    reg_t a5;
    reg_t a6;
    reg_t a7;
    reg_t s2;
    reg_t s3;
    reg_t s4;
    reg_t s5;
    reg_t s6;
    reg_t s7;
    reg_t s8;
    reg_t s9;
    reg_t s10;
    reg_t s11;
    reg_t t3;
    reg_t t4;
    reg_t t5;
    reg_t t6;//31

    reg_t sstatus;
    reg_t sepc; //33

    reg_t kernal_satp;//34
    reg_t kernal_sp;
    reg_t trap_handler
}pt_regs;


typedef struct TaskContext
{
    reg_t ra;
    reg_t sp;
    reg_t s0;
    reg_t s1;
    reg_t s2;
    reg_t s3;
    reg_t s4;
    reg_t s5;
    reg_t s6;
    reg_t s7;
    reg_t s8;
    reg_t s9;
    reg_t s10;
    reg_t s11;
}TaskContext;



#endif

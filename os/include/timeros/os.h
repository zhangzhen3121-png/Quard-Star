#ifndef __OS_H
#define __OS_H

#include<stdarg.h>
#include<stddef.h>

#include"types.h"
#include"context.h"
#include"riscv.h"
#include"task.h"

#define USER_STACK_SIZE     4096*2
#define KERNAL_STACK_SIZE   4096*2


#define __NR_write 64
#define __NR_sched_yield 124



extern void _alltrap();
extern void _restore(pt_regs* next);
extern void __switch(TaskContext* curr_ctx_ptr,TaskContext* next_ctx_ptr);



extern int printk(const char *format, ...);
extern int sbi_print_char(char ch);
extern void trap_init();

/***string.c***/
int strlen(const char* s);
void* memcpy(void* dest, void* src, size_t count);


#endif
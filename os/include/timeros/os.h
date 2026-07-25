#ifndef __OS_H
#define __OS_H

#include<stdarg.h>
#include<stddef.h>

#include"types.h"
#include"context.h"
#include"riscv.h"
// #include"task.h"

#define USER_STACK_SIZE     4096*2
#define KERNAL_STACK_SIZE   4096*2


#define __NR_write 64
#define __NR_read  63
#define __NR_clone 62
#define __NR_sched_yield 124



extern void _alltrap();
extern void _restore(pt_regs* next);
extern void __switch(TaskContext* curr_ctx_ptr,TaskContext* next_ctx_ptr);


extern int printk(const char *format, ...);
extern int sbi_print_char(char ch);
extern void trap_init();


extern int printf(const char *format, ...);
extern size_t syscall(size_t id, reg_t arg1, reg_t arg2, reg_t arg3);
extern size_t sys_write(size_t fd, const char* buf, size_t size);
extern size_t sys_yield(void);



/***string.c***/
extern int strlen(const char* s);
extern void* memcpy(void* dest, void* src, size_t count);
extern void memset(void* addr, char ch, size_t count);


#endif

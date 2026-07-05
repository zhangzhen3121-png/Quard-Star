#include "os.h"

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

size_t sys_write(size_t fd, const const char* buf, size_t size){
    return syscall(__NR_write, fd, buf, size);
}

size_t sys_yield(){
    return syscall(__NR_sched_yield, 0, 0, 0);
}


void delay(volatile int count){
    count*=50000;
    while(count--);
}

void task_1(){

    const char* s = "this is task 1 \r\n";
    size_t len = strlen(s);
    while (1)
    {
        sys_write(1,s,len);
        delay(10000);
        // sys_yield();
    }
    
}

void task_2(){

    const char* s = "this is task 2 \r\n";
    size_t len = strlen(s);
    while (1)
    {
        sys_write(1,s,len);
        delay(10000);
        // sys_yield();
    }
}

void task_3(){
   
    const char* s = "this is task 3 \r\n";
    size_t len = strlen(s);
    while (1)
    {
        sys_write(1,s,len);
        delay(10000);
        // sys_yield();
    }
}

void task_init(){
    task_creat(task_1);
    task_creat(task_2);
    task_creat(task_3);
}

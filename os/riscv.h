#ifndef __RISCV_H
#define __RISCV_H

#include"types.h"

static inline reg_t r_scause(){
    reg_t x;
    asm volatile(
        "csrr %0, scause"
        :"=r"(x)
    );
    return x;
}

static inline reg_t r_stval(){
    reg_t x;
    asm volatile(
        "csrr %0, stval"
        :"=r"(x)
    );
    return x;
}

static inline reg_t r_stvec(){
    reg_t x;
    asm volatile(
        "csrr %0, stvec"
        :"=r"(x)
    );
    return x;
}

static inline reg_t r_sstatus(){
    reg_t x;
    asm volatile(
        "csrr %0, sstatus"
        :"=r"(x)
    );
    return x;
}

static inline reg_t r_sepc(){
    reg_t x;
    asm volatile(
        "csrr %0, sepc"
        :"=r"(x)
    );
    return x;
}

static inline reg_t r_sie(){
    reg_t x;
    asm volatile(
        "csrr %0, sie"
        :"=r"(x)
    );
    return x;
}

static inline reg_t r_mtime(){
    reg_t x;
    asm volatile(
        "rdtime %0"
        :"=r"(x)
    );
    return x;
}

static inline void w_sstatus(reg_t x){
    asm volatile(
        "csrw sstatus, %0"
        : :"r"(x)
    );
}

static inline void w_stvec(reg_t x){
    asm volatile(
        "csrw stvec, %0"
        : :"r"(x)
    );
}

static inline void w_sie(reg_t x){
    asm volatile(
        "csrw sie, %0"
        : :"r"(x)
    );
}

#endif

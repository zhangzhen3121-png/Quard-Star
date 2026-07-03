#include "sbi.h"
#include <stdint.h>

struct sbi_ret sbi_call(long ext, long func, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5){

    struct sbi_ret ret;
    register uintptr_t a0 asm("a0") = arg0;
    register uintptr_t a1 asm("a1") = arg1;
    register uintptr_t a2 asm("a2") = arg2;
    register uintptr_t a3 asm("a3") = arg3; 
    register uintptr_t a4 asm("a4") = arg4;
    register uintptr_t a5 asm("a5") = arg5;

    register uintptr_t a6 asm("a6") = func;
    register uintptr_t a7 asm("a7") = ext;

    asm volatile("ecall"
                 : "+r"(a0), "+r"(a1)
                 : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                 : "memory");

    ret.error = a0;
    ret.value = a1;
    return ret;
}


int sbi_print_char(char ch){
    return sbi_call(SBI_EXT_0_1_CONSOLE_PUTCHAR, 0, ch, 0, 0, 0, 0, 0).value;
}
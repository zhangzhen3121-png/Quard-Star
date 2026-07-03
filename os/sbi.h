#ifndef __SBI_H
#define __SBI_H

enum sbi_ext_id {
    SBI_EXT_0_1_SET_TIMER = 0x0,
    SBI_EXT_0_1_CONSOLE_PUTCHAR = 0x1,
    SBI_EXT_0_1_CONSOLE_GETCHAR = 0x2,
};

struct sbi_ret {
    long error;
    long value;
};

struct sbi_ret sbi_call(long ext, long func, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5);
int sbi_print_char(char ch);

#endif
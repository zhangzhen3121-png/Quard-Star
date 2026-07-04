#ifndef __OS_H
#define __OS_H

#include<stdarg.h>
#include<stddef.h>
#include"types.h"

#define __NR_write 64


extern int printf(const char *format, ...);
extern int sbi_print_char(char ch);
extern void app_init_context();

/***string.c***/
int strlen(const char* s);
void* memcpy(void* dest, void* src, size_t count);


#endif
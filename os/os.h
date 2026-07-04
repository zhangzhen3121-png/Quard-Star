#ifndef __OS_H
#define __OS_H

#include<stdarg.h>
#include<stddef.h>
#include"types.h"



extern int printf(const char *format, ...);
extern int sbi_print_char(char ch);
extern void app_init_context();



#endif
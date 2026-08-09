#ifndef __STRING_H
#define __STRING_H

#include<stddef.h>
#include"types.h"

int strlen(const char* s);
bool cmpstr(const char* s1, const char* s2);
void* memcpy(void* dest, void* src, size_t count);
void memset(void* addr, char ch, size_t count);



#endif
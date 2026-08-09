#include"string.h"

int strlen(const char* s){
    int len = 0;
    while (*(s+len))
    {
        len++;
    }
    return len;
}

bool cmpstr(const char* s1, const char* s2){

    while(*s1 != '\0' && *s2 != '\0'){
        if(*s1 != *s2)return false;
        s1++;
        s2++;
    }

    return true;
}

void* memcpy(void* dest, void* src, size_t count){
    char * ptr = dest;
    while(count--){
        *ptr++ = *((char *)(src++));
    }
    return dest;
}

void memset(void* addr, char ch, size_t count){
    char *ptr = (char*)addr;
    while(count--){
        *ptr++ = ch;
    }
}


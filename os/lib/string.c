#include"os.h"

int strlen(const char* s){
    int len = 0;
    while (*(s+len))
    {
        len++;
    }
    return len;
}

void* memcpy(void* dest, void* src, size_t count){
    char * ptr = dest;
    while(count--){
        *ptr++ = *((char *)(src++));
    }
    return dest;
}
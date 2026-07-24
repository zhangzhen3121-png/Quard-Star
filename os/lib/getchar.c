#include "os.h"

extern size_t sys_read(size_t fd, const char* buf, size_t size); 

char getchar(){
    char buf[1];
    sys_read(0, buf, 1);
    return buf[0];
}



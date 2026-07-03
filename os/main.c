#include "os.h"

void os_main() {
    int a = 1234;
    char* ss = "hello";

    printf("%s %d %x %ld",ss,a,a,(long)a);
}

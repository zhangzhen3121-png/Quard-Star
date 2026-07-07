#include "os.h"
#include "memory.h"

void os_main() {
    printk("hello zz os!\r\n");

    FrameAllocator_init();
    kvminit();
    kvminithart();
    trap_init();
    while (1);
}

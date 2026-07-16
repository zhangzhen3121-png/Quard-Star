#include "os.h"
#include "memory.h"
#include "load.h"

void os_main() {
    printk("hello zz os!\r\n");

    printk("app_num: %d \n",(int)get_app_num());
    FrameAllocator_init();
    kvminit();
    kvminithart();
    trap_init();
    while (1);
}

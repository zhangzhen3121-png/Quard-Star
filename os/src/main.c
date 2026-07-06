#include "os.h"

extern void FrameAllocator_test();
// extern void task_init();
// extern timer_init(reg_t t_ms);
void os_main() {
    printk("hello zz os!\r\n");
    FrameAllocator_test();
    while (1);
    
    // timer_init(500);
    // trap_init();
    // task_init();
    // printk("init done!\r\n");
    // run_first_task();
}

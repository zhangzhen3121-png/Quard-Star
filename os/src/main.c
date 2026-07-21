#include "os.h"
#include "memory.h"
#include "load.h"
#include "task.h"

extern timer_init(reg_t t_ms);

void os_main() {

    printk("hello zz os!\r\n");

    printk("app_num: %d \n",(int)get_app_num());
    FrameAllocator_init();
    kvminit();

  
    load_app(0);
    app_init(0);
    
    load_app(1);
    app_init(1);

    kvminithart();
    trap_init();
    timer_init(500);
    run_first_task();

    while (1);
}

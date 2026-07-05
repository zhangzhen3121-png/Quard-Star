#include "os.h"

extern void task_init();
extern timer_init(reg_t t_ms);
void os_main() {
    printf("hello zz os!\r\n");
    timer_init(500);
    trap_init();
    task_init();
    printf("init done!\r\n");
    run_first_task();
}

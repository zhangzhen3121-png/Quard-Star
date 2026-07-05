#include "os.h"

extern void task_init();

void os_main() {
    printf("hello zz os!\r\n");
    trap_init();
    task_init();
    printf("init done!\r\n");
    run_first_task();
}

#include "os.h"
#include "task.h"



size_t syscall(size_t id, reg_t arg1, reg_t arg2, reg_t arg3) {
    long ret;

    asm volatile(
        "mv a7, %1\n\t"
        "mv a0, %2\n\t"
        "mv a1, %3\n\t"
        "mv a2, %4\n\t"
        "ecall\n\t"
        "mv %0, a0\n\t"
        : "=r"(ret)
        : "r"(id), "r"(arg1), "r"(arg2), "r"(arg3)
        : "a7", "a0", "a1", "a2", "memory"
    );
    return (size_t)ret;
}

size_t sys_write(size_t fd, const char* buf, size_t size) {
    return syscall(__NR_write, (reg_t)fd, (reg_t)buf, (reg_t)size);
}

size_t sys_read(size_t fd, const char* buf, size_t size) {
    return syscall(__NR_read, (reg_t)fd, (reg_t)buf, (reg_t)size);
}

size_t sys_yield(void) {
    return syscall(__NR_sched_yield, 0, 0, 0);
}

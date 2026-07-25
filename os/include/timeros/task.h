#ifndef __TASK_H
#define __TASK_H

#include"context.h"
#include"types.h"
#include"os.h"
#include"memory.h"


#define MAX_TASKS          10
#define USER_STACK_SIZE     4096*2
#define KERNAL_STACK_SIZE   4096*2

// extern PageTable;

typedef enum TaskState{
    Unint,
    Ready,
    Runing,
    Exited,
}TaskState;

typedef struct TaskControlBlock
{
    TaskState task_state;
    TaskContext task_context;
    PageTable pagetable;
    uint64_t pid;
    uint64_t usize;
    uint64_t ustack;
    uint64_t kstack;
    uint64_t entry;
    pt_regs* trap_ctx_pa;

}TaskControlBlock;


uint64_t alloc_pid();
void uvmcopy(PageTable* pt_fa, PageTable* pt_sub, uint64_t usize);

TaskControlBlock* proc_alloc();
void proc_init();
void proc_pagetable(TaskControlBlock* tcb);
void proc_trap(TaskControlBlock* tcb);
void proc_mapstacks(PageTable* kpgtbl);

struct TaskContext tcx_init(reg_t kernal_ptr);

TaskControlBlock* task_creat_pt(int id);
void schedule();
void run_first_task();
uint64_t current_user_token();
PageTable* current_user_pagetable();
void app_init(int id);
TaskControlBlock* task_get_current();
#endif

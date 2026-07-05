#ifndef __TASK_H
#define __TASK_H

#include"context.h"
#include"types.h"
#include"os.h"

#define MAX_TASKS          10
#define USER_STACK_SIZE     4096*2
#define KERNAL_STACK_SIZE   4096*2

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

}TaskControlBlock;



struct TaskContext tcx_init(reg_t kernal_ptr);
void task_creat(void(*task_entry)(void));
void schedule();
void run_first_task();
#endif

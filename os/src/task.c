#include"task.h"
#include"memory.h"

struct TaskControlBlock tasks[MAX_TASKS];
uint8_t UserStacks[MAX_TASKS][USER_STACK_SIZE];
uint8_t KernalStacks[MAX_TASKS][USER_STACK_SIZE];


static uint32_t _top = 0;
static uint32_t _current = 0;

extern StrackFrameAllocator FrameAllocatorImpl;
extern PageTable KernalPAgeTable;


void task_creat(void(*task_entry)(void)){
    
    if(_top<MAX_TASKS){
        pt_regs* ctx = (pt_regs*)(&KernalStacks[_top]+KERNAL_STACK_SIZE-sizeof(pt_regs));
        reg_t user_sp =(reg_t)(&UserStacks[_top]+USER_STACK_SIZE);

        reg_t sstatus = r_sstatus();
        sstatus &= ~(0x1<<8);
        w_sstatus(sstatus);

        ctx->sstatus = sstatus;
        ctx->sepc = (reg_t)task_entry;
        ctx->sp = user_sp;
        
        tasks[_top].task_context = tcx_init((reg_t)ctx);
        tasks[_top].task_state = Ready;
        _top++;
    }
}


struct TaskContext tcx_init(reg_t kernal_ptr){
    struct TaskContext tcx;

    tcx.ra = _restore;
    tcx.sp = kernal_ptr;
    tcx.s0 = 0;
    tcx.s1 = 0;
    tcx.s2 = 0;
    tcx.s3 = 0;
    tcx.s4 = 0;
    tcx.s5 = 0;
    tcx.s6 = 0;
    tcx.s7 = 0;
    tcx.s8 = 0;
    tcx.s9 = 0;
    tcx.s10 = 0;
    tcx.s11 = 0;

    return tcx;
}


void schedule(){
    _current = _current%_top;
    TaskContext* cur_tcx_ptr = &tasks[_current].task_context;
    TaskContext* nex_tcx_ptr = 0x0;
    for(int i=1;i<_top;i++){
        if(tasks[(_current+i)%_top].task_state == Ready){
            tasks[_current].task_state = Ready;
            nex_tcx_ptr = &tasks[(_current+i)%_top].task_context;
            tasks[(_current+i)%_top].task_state = Runing;
            _current += i;
            break;
        }
    }
    if(nex_tcx_ptr){
        __switch(cur_tcx_ptr,nex_tcx_ptr);
    }
    else{
        printk("No ready task !");
    }
}


void run_first_task(){
    tasks[0].task_state = Runing;
    _current  = 0;
    TaskContext* next_ctx_ptr = &tasks[_current].task_context;
    TaskContext curr_ctx_ptr;
    printk("switch \n\n");
    __switch(&curr_ctx_ptr,next_ctx_ptr);
    printk("switch feild \n\n");
}

PhysPageNum kalloc(){
    PhysPageNum ppn = StrackFrameAllocator_alloc(&FrameAllocatorImpl);
    return ppn;
}

void proc_mapstacks(PageTable* kpgtbl){
    for(int i=0;i<MAX_TASKS;i++){
        PhysAddr pa = PhysAddr_from_PhysPageNum(kalloc());
        VirtAddr va = VirtAddr_from_u64(KSTACK(i));
        memory_map(kpgtbl,va,pa,PAGE_SIZE,PTE_R|PTE_W);
    }
}
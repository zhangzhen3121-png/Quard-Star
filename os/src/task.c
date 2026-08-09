#include"task.h"
#include"memory.h"
#include"load.h"

struct TaskControlBlock tasks[MAX_TASKS];

static uint32_t _pid = 0;
static uint32_t _top = 0;
static uint32_t _current = 0;

extern StrackFrameAllocator FrameAllocatorImpl;
extern PageTable KernalPAgeTable;

extern char trampoline[];
extern char strampolinep[];


extern void trap_return();
extern void trap_hanlder();

void exec_init(TaskControlBlock* tcb){

    pt_regs* trap_ctx = tcb->trap_ctx_pa;
    
    reg_t sstatus = r_sstatus();
    sstatus &= ~(1U<<8);
    w_sstatus(sstatus);

    trap_ctx->sstatus = sstatus;
    trap_ctx->sepc = tcb->entry;
    trap_ctx->sp = tcb->ustack;
    trap_ctx->kernal_sp = tcb->kstack;
    trap_ctx->kernal_satp = SET_SATP(KernalPAgeTable.root_ppn.value);
    trap_ctx->trap_handler = (uint64_t)trap_hanlder;
}


void app_init(int id){
    TaskControlBlock* tcb = &tasks[id];
    pt_regs* trap_ctx = tcb->trap_ctx_pa;
    
    reg_t sstatus = r_sstatus();
    sstatus &= ~(1U<<8);
    w_sstatus(sstatus);

    trap_ctx->sstatus = sstatus;
    trap_ctx->sepc = tcb->entry;
    trap_ctx->sp = tcb->ustack;
    trap_ctx->kernal_sp = tcb->kstack;
    trap_ctx->kernal_satp = SET_SATP(KernalPAgeTable.root_ppn.value);
    trap_ctx->trap_handler = (uint64_t)trap_hanlder;

    tcb->task_context = tcx_init((reg_t)tcb->kstack);
    tcb->task_state = Ready;
    tcb->pid = alloc_pid();
    
}



struct TaskContext tcx_init(reg_t kernal_ptr){
    struct TaskContext tcx;
    tcx.ra = trap_return;
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
    // printk("current task: %d  top: %d\n",_current, _top);
    TaskContext* cur_tcx_ptr = &tasks[_current].task_context;
    TaskContext* nex_tcx_ptr = 0x0;
    for(int i=1;i<_top;i++){
        if(tasks[(_current+i)%_top].task_state == Ready){
            // printk("next task: %d\n",(_current+i)%_top);
            tasks[_current].task_state = Ready;
            nex_tcx_ptr = &tasks[(_current+i)%_top].task_context;
            tasks[(_current+i)%_top].task_state = Runing;
            _current += i;
            _current = _current%_top;
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
    printk("run first task  top: %d \n",_top);
    tasks[0].task_state = Runing;
    _current  = 0;
    TaskContext* next_ctx_ptr = &tasks[_current].task_context;
    TaskContext curr_ctx_ptr;
    printk("switch \n\n");
    __switch(&curr_ctx_ptr,next_ctx_ptr);
    printk("switch feild \n\n");
}


uint64_t alloc_pid(){
    int pid = _pid;
    _top++;
    _pid++;
    return pid;
}

void proc_mapstacks(PageTable* kpgtbl){
    for(int i=0;i<MAX_TASKS;i++){
        PhysAddr pa = PhysAddr_from_PhysPageNum(kalloc());
        VirtAddr va = VirtAddr_from_u64(KSTACK(i));
        memory_map(kpgtbl,va,pa,PAGE_SIZE,PTE_R|PTE_W);
        tasks[i].kstack = va.value+PAGE_SIZE;
    }
}

void proc_trap(TaskControlBlock* tcb){

    
    //映射trap上下文页
    PhysAddr pa = PhysAddr_from_PhysPageNum(kalloc());

    if(pa.value==0){
        printk("alloc trap page failed \r\n");
        return;
    }
    memory_map(&tcb->pagetable, VirtAddr_from_u64(TRAPCONTEXT), pa, PAGE_SIZE, PTE_R|PTE_W);

    PageTableEntry * trap_pte = Find_Pte(&tcb->pagetable, VirtPageNum_from_VirtAddr(VirtAddr_from_u64(TRAPCONTEXT)));
    uint8_t trap_pte_flag = trap_pte->bits & 0xFF;
    printk("pagetable %lx trap pte flag: %x\n",tcb->pagetable.root_ppn.value,trap_pte_flag);

    tcb->trap_ctx_pa = (pt_regs*)pa.value;
    memset((void*)pa.value, 0, PAGE_SIZE);
}


void proc_pagetable(TaskControlBlock* tcb){
    
    tcb->pagetable.root_ppn = kalloc();

    //映射trampline跳板页
    memory_map(&tcb->pagetable, VirtAddr_from_u64(TRAMPOLINE), PhysAddr_form_u64((uint64_t)trampoline), PAGE_SIZE, PTE_R|PTE_X);
}


void proc_init(){
    for(int i=0;i<MAX_TASKS;i++){
        tasks[i].task_state = Unint;
    }
}

TaskControlBlock* proc_alloc(){
    
    TaskControlBlock* tcb = tasks;
    for(tcb;tcb<=&tasks[MAX_TASKS-1];tcb++){
        if(tcb->task_state == Unint)break;
    }
    if(tcb > &tasks[MAX_TASKS-1])return NULL;

    proc_pagetable(tcb);
    proc_trap(tcb);
    tcb->pid = alloc_pid();

    return tcb;
}


void uvmcopy(PageTable* pt_fa, PageTable* pt_sub, uint64_t usize){
    uint64_t va = 0;
    uint64_t size = GROUNDUP(usize);

    for(va=0; va<size; va+=PAGE_SIZE){
        PageTableEntry* pte = Find_Pte(pt_fa, VirtPageNum_from_VirtAddr(VirtAddr_from_u64(va)));
        if(pte == 0)continue;
        uint64_t fa_addr = PhysAddr_from_PhysPageNum(PhysPageNum_from_PageTableEntry(*pte)).value;
        uint64_t sub_addr = PhysAddr_from_PhysPageNum(StrackFrameAllocator_alloc(&FrameAllocatorImpl)).value;
        memcpy(sub_addr,fa_addr,PAGE_SIZE);
        memory_map(pt_sub,VirtAddr_from_u64(va),PhysAddr_form_u64(sub_addr),PAGE_SIZE, PteFlag_from_PageTableEntry(*pte));
    }

}



TaskControlBlock* task_creat_pt(int id){

    if(id>=MAX_TASKS||_top>=MAX_TASKS)return NULL;
    
    proc_pagetable(&tasks[id]);
    proc_trap(&tasks[id]);
    printk("creat task %d pegetable: %lx\n",id, tasks[id].pagetable.root_ppn.value);
    //_top++;
    // printk("top %d \n",_top);
    return &tasks[id];
}

TaskControlBlock* task_get_current(){
    return &tasks[_current];
}

uint64_t current_user_token(){
    return PhysAddr_from_PhysPageNum(tasks[_current].pagetable.root_ppn).value;
}

PageTable* current_user_pagetable(){
    return &(tasks[_current].pagetable);
}
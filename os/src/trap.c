#include"context.h"
#include"riscv.h"
#include"os.h"
#include"task.h"
#include"memory.h"


extern void set_next_trigger();


char* physaddr_from_uservritaddr(const char* buf, size_t len){

    PageTable* pt = current_user_pagetable();
    VirtPageNum vpn = VirtPageNum_from_VirtAddr(VirtAddr_from_u64((uint64_t)buf));
    PageTableEntry* pte =  Find_Pte(pt,vpn);
    uint64_t physaddr_off = (uint64_t)buf & 0xFFF;
    char* kbuf = (char*)(PhysAddr_from_PhysPageNum(PhysPageNum_from_PageTableEntry(*pte)).value+physaddr_off);
    return kbuf;
}


void trap_return(){
    // printk("trap return \n");
    set_user_entry();
    uint64_t ctx_va = TRAPCONTEXT;
    uint64_t upt_addr = SET_SATP(task_get_current()->pagetable.root_ppn.value);
    uint64_t restore_va = TRAMPOLINE+(_restore-_alltrap);
    // printk("current task pagetable: %lx\n",task_get_current()->pagetable.root_ppn.value);

    // PageTableEntry * trap_pte = Find_Pte(&task_get_current()->pagetable, VirtPageNum_from_VirtAddr(VirtAddr_from_u64(TRAPCONTEXT)));
    // uint8_t trap_pte_flag = trap_pte->bits & 0xFF;
    // printk("trap pte flag: %x\n",trap_pte_flag);

    asm volatile(
        "fence.i\n\t"
        "mv a0, %0\n\t"
        "mv a1, %1\n\t"
        "jr %2\n\t"
        :
        :"r"(ctx_va),
         "r"(upt_addr),
         "r"(restore_va)
        :"a0","a1"
    );
}


void __sys_write(size_t fd, const char* buf, size_t len){
    if(fd==1){
        // printk("sys_write: ");
        char* kbuf = physaddr_from_uservritaddr(buf,len);
        printk(kbuf);
    }
    else{
        printk("unsupport fd id sys_wirte \r\n");
    }
}


void __sys_read(size_t fd, const char* buf, size_t len){
    if(fd==0){
        char* kbuf = physaddr_from_uservritaddr(buf,len);
        int c;
        while (1)
        {
            c = sbi_read_char();
            if(c!=-1)break;
        }
        kbuf[0] = c;
    }
    else{
        printk("unsupport fd id sys_read \r\n");
    }
}

void __sys_fork(){

    TaskControlBlock* new_tcb = proc_alloc();
    TaskControlBlock* cur_tcb = task_get_current();
    if(!new_tcb){
        cur_tcb->trap_ctx_pa->a0 = -1;
        return;
    }
    uvmcopy(&cur_tcb->pagetable,&new_tcb->pagetable,cur_tcb->usize);
    memcpy(new_tcb->trap_ctx_pa, cur_tcb->trap_ctx_pa, PAGE_SIZE);
    
    new_tcb->usize = cur_tcb->usize;
    new_tcb->entry = cur_tcb->entry;
    new_tcb->task_state = Ready;

    new_tcb->task_context.ra = trap_return;
    new_tcb->task_context.sp = new_tcb->kstack;
    
    new_tcb->trap_ctx_pa->sepc += 4;
    new_tcb->trap_ctx_pa->a0 = 0;
    cur_tcb->trap_ctx_pa->a0 = new_tcb->pid;
}



void __sys_yield(){
    schedule();
}


void __SYSCALL(size_t id, reg_t arg1, reg_t arg2, reg_t arg3){
    switch (id)
    {
    case __NR_write:
        __sys_write(arg1, (const char*)arg2, arg3);
        break;
    case __NR_sched_yield:
        __sys_yield();
        break;
    case __NR_read:
        __sys_read(arg1, (const char*)arg2, arg3);
        break;
    case __NR_clone:
        __sys_fork();
        break;
    default:
        printk("unsupport syscall id\r\n");
        break;
    }
}


void set_user_entry(){
    w_stvec((reg_t)TRAMPOLINE);
}



void trap_hanlder(){
    // printk("enter trap \r\n");
    pt_regs* ctx = task_get_current()->trap_ctx_pa;
    
    reg_t cause = r_scause();
    reg_t code = cause & 0xFFF;

    if(cause>>63){
        switch (code)
        {
        case 5:
            /* RTC IRQ */
            set_next_trigger();
            schedule();
            break;
        default:
            printk("undefined interrput code\r\n");
            break;
        }
    }
    else{
        switch (code)
        {
        case 8:
            __SYSCALL(ctx->a7,ctx->a0,ctx->a1,ctx->a2);
            ctx->sepc+=4;
            break;
        default:
            printk("undefined scause\r\n");
            break;
        }
    }
    trap_return();
}


void trap_init(){
    w_stvec((reg_t)TRAMPOLINE);
}

#include"load.h"
#include"memory.h"
#include"task.h"

extern uint64_t _num_app[];
extern uint8_t trampoline[];

uint8_t flag_to_mapflag(uint8_t flag){
    return ((flag & PF_X)?PTE_X:0)|
           ((flag & PF_R)?PTE_R:0)|
           ((flag & PF_W)?PTE_W:0);
}

int get_app_num(){
    return (int)_num_app[0];
}

APPMATEDATA get_app_data(int id){
    APPMATEDATA data;
    data.addr = _num_app[id+1];
    data.size = _num_app[id+2]-_num_app[id+1];
    return data;
}

void load_app(int id){

    int app_num = get_app_num();

    if(id>=app_num){
        return;
    }

    APPMATEDATA matedata = get_app_data(id);
    elf64_ehdr_t* ehdr_t = matedata.addr;

    if(*(uint32_t*)ehdr_t != ELF_MAG){
        // uint16_t buf = *(uint16_t*)ehdr_t ;
        // printk("char: %x\n",buf);
        printk("app file format is not elf !\n");
        return;
    }

    if(ehdr_t->e_machine != EM_RISCV_64){
        printk("app file Arch is not RISCV_64 !\n");
        return;
    }

    TaskControlBlock* tcb = task_creat_pt(id);
    tcb->entry = ehdr_t->e_entry;

    for(int i=0; i<ehdr_t->e_phnum; i++){
        elf64_phdr* phdr = matedata.addr + ehdr_t->e_phoff + i*ehdr_t->e_phentsize;
        if(phdr->p_type != PT_LOAD)continue;
        uint8_t flag = PTE_U|flag_to_mapflag(phdr->p_flags);
        
        for(uint64_t j=0;j<phdr->p_memsz+PAGE_SIZE-1;j+=PAGE_SIZE){
            PhysPageNum ppn = kalloc();
            memcpy((void*)PhysAddr_from_PhysPageNum(ppn).value, (void*)(matedata.addr+phdr->p_offset), PAGE_SIZE);
            memory_map(&tcb->pagetable, VirtAddr_from_u64(phdr->p_vaddr + j), PhysAddr_from_PhysPageNum(ppn), PAGE_SIZE, flag);
        }

        tcb->ustack = GROUNDUP(phdr->p_vaddr + phdr->p_memsz);
    }

    PhysPageNum stack_ppn = kalloc();
    tcb->ustack +=  2*PAGE_SIZE;
    memory_map(&tcb->pagetable, VirtAddr_from_u64(tcb->ustack-PAGE_SIZE), PhysAddr_from_PhysPageNum(stack_ppn), PAGE_SIZE, PTE_U|PTE_R|PTE_W);

    printk("app %d load success !\n",id);

}
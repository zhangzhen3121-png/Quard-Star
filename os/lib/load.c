#include"load.h"
#include"memory.h"



extern uint64_t _num_app[];
extern char _name_app[];
extern uint8_t trampoline[];

uint8_t flag_to_mapflag(uint8_t flag){
    return ((flag & PF_X)?PTE_X:0)|
           ((flag & PF_R)?PTE_R:0)|
           ((flag & PF_W)?PTE_W:0);
}

int get_app_num(){
    return (int)_num_app[0];
}

char *get_app_name(int id)
{
    if (id < 0 || id >= get_app_num())
        return NULL;

    char *name = _name_app;

    while (id-- > 0) {
        while (*name != '\0')
            name++;
        name++;  // 跳过当前名字末尾的 '\0'
    }

    return name;
}

APPMATEDATA get_app_data(int id){
    APPMATEDATA data;
    data.addr = _num_app[id+1];
    data.size = _num_app[id+2]-_num_app[id+1];
    return data;
}


bool check_elf(elf64_ehdr_t* ehdr_t){
    if(*(uint32_t*)ehdr_t != ELF_MAG){
        // uint16_t buf = *(uint16_t*)ehdr_t ;
        // printk("char: %x\n",buf);
        printk("app file format is not elf !\n");
        return false;
    }

    if(ehdr_t->e_machine != EM_RISCV_64){
        printk("app file Arch is not RISCV_64 !\n");
        return false;
    }
    return true;
}

elf64_ehdr_t* get_elf64_id(int id){
    int app_num = get_app_num();

    if(id < 0 || id>=app_num){
        return 0;
    }

    char* app_name = get_app_name(id);
    printk("%s loading ...\n",app_name);
    APPMATEDATA matedata = get_app_data(id);
    elf64_ehdr_t* ehdr_t = (elf64_ehdr_t *)matedata.addr;
    return ehdr_t;
}


elf64_ehdr_t* get_elf64_name(char* name){
    int app_num = get_app_num();
    int id = 0;
    char* app_name;
    for(id=0; id<app_num; id++){
        app_name = get_app_name(id);
        if(cmpstr(name,app_name))break;
    }
    if(id == app_num)return 0;

    printk("%s loading ...\n",app_name);
    APPMATEDATA matedata = get_app_data(id);
    elf64_ehdr_t* ehdr_t = (elf64_ehdr_t *)matedata.addr;
    return ehdr_t;
}

bool check_name(char* name){
    int app_num = get_app_num();
    int id = 0;
    char* app_name;
    for(id=0; id<app_num; id++){
        app_name = get_app_name(id);
        if(cmpstr(name,app_name))break;
    }
    if(id == app_num)return false;

    return true;
}

void load_segment(TaskControlBlock* tcb, elf64_ehdr_t* ehdr_t){
    uint8_t *elf_data = (uint8_t *)ehdr_t;

    tcb->ustack = 0;

    for(int i=0; i<ehdr_t->e_phnum; i++){
        elf64_phdr* phdr = (elf64_phdr *)(elf_data + ehdr_t->e_phoff +
                                         i * ehdr_t->e_phentsize);
        if(phdr->p_type != PT_LOAD)continue;
        uint8_t flag = PTE_U|flag_to_mapflag(phdr->p_flags);
        uint64_t memsize = GROUNDUP(phdr->p_memsz);
        for(uint64_t j=0; j<memsize; j+=PAGE_SIZE){
            PhysPageNum ppn = kalloc();
            void *page = (void *)PhysAddr_from_PhysPageNum(ppn).value;

            /* kalloc() returns a zeroed page.  Copy only bytes present in the
             * ELF file so the remainder of the last page and .bss stay zero.
             */
            if(j < phdr->p_filesz){
                uint64_t copy_size = phdr->p_filesz - j;
                if(copy_size > PAGE_SIZE)copy_size = PAGE_SIZE;
                memcpy(page, elf_data + phdr->p_offset + j, copy_size);
            }
            memory_map(&tcb->pagetable, VirtAddr_from_u64(phdr->p_vaddr + j), PhysAddr_from_PhysPageNum(ppn), PAGE_SIZE, flag);
        }
        uint64_t segment_end = GROUNDUP(phdr->p_vaddr + phdr->p_memsz);
        if(segment_end > tcb->ustack)tcb->ustack = segment_end;
    }
}

void map_ustack(TaskControlBlock* tcb){
    PhysPageNum stack_ppn = kalloc();
    tcb->ustack +=  2*PAGE_SIZE;
    memory_map(&tcb->pagetable, VirtAddr_from_u64(tcb->ustack-PAGE_SIZE), PhysAddr_from_PhysPageNum(stack_ppn), PAGE_SIZE, PTE_U|PTE_R|PTE_W);
    tcb->usize = tcb->ustack;
}




size_t exec_load(TaskControlBlock* tcb ,char* name){

    elf64_ehdr_t* ehdr_t = get_elf64_name(name);
    if(ehdr_t==0)return 0;
    if(!check_elf(ehdr_t))return 0;

    tcb->entry = ehdr_t->e_entry;

    load_segment(tcb, ehdr_t);

    map_ustack(tcb);

    printk("app %s load success !\n", name);

    return 1;
}

void load_app(int id){

    elf64_ehdr_t* ehdr_t = get_elf64_id(id);
    if(ehdr_t==0)return;
    if(!check_elf(ehdr_t))return;

    TaskControlBlock* tcb = task_creat_pt(id);
    tcb->entry = ehdr_t->e_entry;

    load_segment(tcb, ehdr_t);

    map_ustack(tcb);

 

    printk("app %d load success !\n",id);

}

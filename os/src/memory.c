#include"memory.h"
#include"stack.h"


static StrackFrameAllocator FrameAllocatorImpl;
static PageTable KernalPAgeTable;

extern char text_end[];
extern char kernal_end[];
PhysAddr PhysAddr_form_u64(uint64_t v){
    PhysAddr addr;
    addr.value = v&((1ULL<<PA_WITDH_SV39) -1);
    return addr;
}

VirtAddr VirtAddr_from_u64(uint64_t v){
    VirtAddr addr;
    addr.value = v&((1ULL<<VA_WITDH_SV39) -1);
    return addr;
}

PhysPageNum PhysPageNum_from_u64(uint64_t v){
    PhysPageNum ppn;
    ppn.value = v&((1ULL<<PPN_WIDTH_SV39) -1);
    return ppn;
}

VirtPageNum VirtPageNum_from_u64(uint64_t v){
    VirtPageNum vpn;
    vpn.value = v&((1ULL<<VPN_WIDTH_SV39) -1);
    return vpn;
}


uint64_t u64_from_PhysAddr(PhysAddr pa){
    return pa.value;    
}

uint64_t u64_from_VirtAddr(VirtAddr va){
    return va.value;    
}

uint64_t u64_from_PhysPageNum(PhysPageNum ppn){
    return ppn.value;
}

uint64_t u64_from_VirtPageNum(VirtPageNum vpn){
    return vpn.value;
}


PhysPageNum PhysPageNum_form_PhysAddr(PhysAddr pa){
    PhysPageNum ppn;
    ppn.value = pa.value>>PAGE_SIZE_BIT;
    return ppn;
}


VirtPageNum VirtPageNum_from_VirtAddr(VirtAddr va){
    VirtPageNum vpn;
    vpn.value = va.value>>PAGE_SIZE_BIT;
    return vpn;
}

PhysAddr PhysAddr_from_PhysPageNum(PhysPageNum ppn){
    PhysAddr pa;
    pa.value = ppn.value<<PAGE_SIZE_BIT;
    return pa;
}

VirtAddr VirtAddr_from_VirtPageNum(VirtPageNum vpn){
    VirtAddr va;
    va.value = vpn.value<<PAGE_SIZE_BIT;
    return va;
}

PageTableEntry PageTableEntry_clean(){
    PageTableEntry pte;
    pte.bits = 0;
    return pte;
}

PhysPageNum PhysPageNum_from_PageTableEntry(PageTableEntry pte){
    PhysPageNum ppn;
    ppn.value = (pte.bits>>PTE_FLAG_BIT)&((1ULL<<PTE_PPN_BIT)-1);
    return ppn;
}
PageTableEntry* PageTableEntryPtr_from_PhysPageNum(PhysPageNum ppn){
    uint64_t addr = PhysAddr_from_PhysPageNum(ppn).value;
    return (PageTableEntry*)addr;
}


PageTableEntry PageTableEntry_from_PhysPageNum(PhysPageNum ppn){
    PageTableEntry pte;
    pte.bits = ppn.value<<PTE_FLAG_BIT;
    return pte;
}

bool PageTableEntry_is_empty(PageTableEntry pte){
    uint8_t pte_flag = pte.bits;
    return (pte_flag&PTE_V) == 0;
}

PageTableEntry PageTableEntry_new(PhysPageNum ppn, uint8_t pte_flag){
    PageTableEntry pte = PageTableEntry_from_PhysPageNum(ppn);
    pte.bits |= pte_flag;
    return pte;
} 


void PTindex_form_VirtPageNum(VirtPageNum vpn, uint64_t *index){
    uint64_t v  = vpn.value;
    for(int i=2;i>=0;i--){
        index[i] = v & 0x1FF;
        v>>=9;
    }
}


PageTableEntry* Find_Pte_Creat(PageTable* pt, VirtPageNum vpn){
    uint64_t pt_index[3];
    PTindex_form_VirtPageNum(vpn, pt_index);

    PhysPageNum pt_ppn = pt->root_ppn;
    PageTableEntry* pte_ptr;
    for(int i=0;i<3;i++){
        pte_ptr = &PageTableEntryPtr_from_PhysPageNum(pt_ppn)[pt_index[i]];

        if(PageTableEntry_is_empty(*pte_ptr) && i != 2){
            PhysPageNum frame = StrackFrameAllocator_alloc(&FrameAllocatorImpl);
            *pte_ptr = PageTableEntry_new(frame, PTE_V);
        }
        pt_ppn = PhysPageNum_from_PageTableEntry(*pte_ptr);
    }
    
    return pte_ptr;
}


PageTableEntry* Find_Pte(PageTable* pt, VirtPageNum vpn){
    uint64_t pt_index[3];
    PTindex_form_VirtPageNum(vpn, pt_index);

    PhysPageNum pt_ppn = pt->root_ppn;
    PageTableEntry* pte_ptr;
    for(int i=0;i<3;i++){
        pte_ptr = &PageTableEntryPtr_from_PhysPageNum(pt_ppn)[pt_index[i]];

        if(PageTableEntry_is_empty(*pte_ptr) && i != 2){
            return NULL;
        }
        pt_ppn = PhysPageNum_from_PageTableEntry(*pte_ptr);
    }
    
    return pte_ptr;
}



void PageTable_map(PageTable* root_pt, VirtPageNum vpn, PhysPageNum ppn, uint8_t pte_flag){
    PageTableEntry* pa_pte = Find_Pte_Creat(root_pt, vpn);
    assert(PageTableEntry_is_empty(*pa_pte));
    *pa_pte = PageTableEntry_new(ppn, pte_flag|PTE_V);
}

void PageTable_unmap(PageTable* root_pt, VirtPageNum vpn){
    PageTableEntry* pa_pte = Find_Pte(root_pt, vpn);
    assert(!PageTableEntry_is_empty(*pa_pte));
    *pa_pte = PageTableEntry_clean();
}

void memory_map(PageTable* root_pt, VirtAddr va, PhysAddr pa, uint64_t size, uint8_t flag){
    PhysPageNum ppn = PhysPageNum_form_PhysAddr(pa);
    VirtPageNum vpn = VirtPageNum_from_VirtAddr(va);
    
    uint64_t page_nums = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    while (page_nums--)
    {
        PageTable_map(root_pt,vpn,ppn,flag);
        vpn.value++;
        ppn.value++;
    }
    
}


void memory_unmap(PageTable* root_pt, VirtAddr va, uint64_t size){
    VirtPageNum vpn = VirtPageNum_from_VirtAddr(va);
    uint64_t page_nums = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    while (page_nums--)
    {
        PageTable_unmap(root_pt,vpn);
        vpn.value++;
    }
}

PageTable kvmmake(){
    PageTable pt;
    pt.root_ppn= StrackFrameAllocator_alloc(&FrameAllocatorImpl);
    printk("root pnn alloc \n");
    memory_map(&pt, VirtAddr_from_u64(KernalkBase), PhysAddr_form_u64(KernalkBase), (uint64_t)text_end - KernalkBase, PTE_R | PTE_X | PTE_A | PTE_D);
    printk("kernal text map \n");
    memory_map(&pt, VirtAddr_from_u64((uint64_t)text_end), PhysAddr_form_u64((uint64_t)text_end), PHYSTOP-(uint64_t)text_end, PTE_R | PTE_W | PTE_A | PTE_D);
    printk("data and memory map \n");
    return pt;
}





PhysPageNum StrackFrameAllocator_alloc(StrackFrameAllocator* allocator){
    PhysPageNum ppn;

    if(allocator->recyled._top>=0){
        ppn.value = pop(&allocator->recyled);
    }
    else{
        if(allocator->unused_current<allocator->unused_end){
            ppn.value = allocator->unused_current++;
        }
        else{
            ppn.value = 0;
        }
    }

    uint64_t addr = u64_from_PhysAddr(PhysAddr_from_PhysPageNum(ppn));

    memset((void *)addr,0,PAGE_SIZE);

    return ppn;
}


void StrackFrameAllocator_free(StrackFrameAllocator* allocator, PhysPageNum ppn){
    if(ppn.value>=allocator->unused_current){
        printk("the memory page unused");
        return;
    }
    Stack* recyle = &allocator->recyled;
    if(allocator->recyled._top>=0){
        
        for(int i=0;i<=recyle->_top;i++){
            if(recyle->data[i]==ppn.value){
                printk("the memory page unused");
                return; 
            }
        }
    }

    push(recyle,ppn.value);
}


void StrackFrameAllocator_Init(StrackFrameAllocator* allocator, PhysPageNum ppn_start, PhysPageNum ppn_end){
    allocator->unused_current = ppn_start.value;
    allocator->unused_end = ppn_end.value;
    Stack_init(&allocator->recyled);
}

void FrameAllocator_init(){
    PhysPageNum ppn_start = PhysPageNum_form_PhysAddr(PhysAddr_form_u64(kernal_end));
    PhysPageNum ppn_end = PhysPageNum_form_PhysAddr(PhysAddr_form_u64(PHYSTOP));
    StrackFrameAllocator_Init(&FrameAllocatorImpl,ppn_start,ppn_end);

    printk("physaddr start: %lx\r\n",PhysAddr_form_u64(kernal_end).value);
    printk("physaddr end: %lx\r\n",PhysAddr_form_u64(PHYSTOP).value);
}

void kvminit(){
    KernalPAgeTable = kvmmake();
}

void kvminithart(){
    sfence_vma();
    printk("root ppn: %lx\n", KernalPAgeTable.root_ppn.value);
    printk("satp val: %lx\n", SET_SATP(KernalPAgeTable.root_ppn.value));    
    w_satp(SET_SATP(KernalPAgeTable.root_ppn.value));
    sfence_vma();
    uint64_t satp = r_satp();
    printk("Satp: %lx \n",satp);
}




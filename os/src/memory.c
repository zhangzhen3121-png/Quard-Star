#include"memory.h"
#include"stack.h"


static StrackFrameAllocator FrameAllocatorImpl;


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


void StrackFrameAllocator_Init(StrackFrameAllocator* allocator, PhysPageNum ppn_start, PhysPageNum ppn_end){
    allocator->unused_current = ppn_start.value;
    allocator->unused_end = ppn_end.value;
    Stack_init(&allocator->recyled);
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


void FrameAllocator_test(){
    PhysPageNum ppn_start = PhysPageNum_form_PhysAddr(PhysAddr_form_u64(MEMERY_START));
    PhysPageNum ppn_end = PhysPageNum_form_PhysAddr(PhysAddr_form_u64(MEMERY_END));
    StrackFrameAllocator_Init(&FrameAllocatorImpl,ppn_start,ppn_end);

    printk("physaddr start: %x\r\n",PhysAddr_form_u64(MEMERY_START));
    printk("physaddr end: %x\r\n",PhysAddr_form_u64(MEMERY_END));
    
    PhysPageNum Frame[10];

    for(int i=0;i<5;i++){
      PhysPageNum alloc_ppn = StrackFrameAllocator_alloc(&FrameAllocatorImpl);
      if(alloc_ppn.value>0){
        Frame[i] = alloc_ppn;
        printk("alloc %d page addr: %x \r\n",i,PhysAddr_from_PhysPageNum(alloc_ppn));
      }
    }

    for(int i=0;i<5;i++){
        StrackFrameAllocator_free(&FrameAllocatorImpl,Frame[i]);
        printk("free %d page addr: %x \r\n",i,PhysAddr_from_PhysPageNum(Frame[i]));
    }

    for(int i=0;i<5;i++){
      PhysPageNum alloc_ppn = StrackFrameAllocator_alloc(&FrameAllocatorImpl);
      if(alloc_ppn.value>0){
        Frame[i] = alloc_ppn;
        printk("alloc %d page addr: %x \r\n",i,PhysAddr_from_PhysPageNum(alloc_ppn));
      }
    }

    for(int i=0;i<5;i++){
        StrackFrameAllocator_free(&FrameAllocatorImpl,Frame[i]);
        printk("free %d page addr: %x \r\n",i,PhysAddr_from_PhysPageNum(Frame[i]));
    }
}






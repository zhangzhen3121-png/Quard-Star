#ifndef __MEMORY_H
#define __MEMORY_H

#include"os.h"
#include"types.h"
#include"stack.h"
#include"assert.h"




#define KernalkBase  0x80200000
#define PHYSTOP      0x88200000


#define PAGE_SIZE       0x1000
#define PAGE_SIZE_BIT   0xc

#define PA_WITDH_SV39   56
#define VA_WITDH_SV39   39
#define PPN_WIDTH_SV39  (PA_WITDH_SV39-PAGE_SIZE_BIT)
#define VPN_WIDTH_SV39  (VA_WITDH_SV39-PAGE_SIZE_BIT)

#define SATP_SV39       (8L<<60)
#define SET_SATP(PAGETABLE)        (SATP_SV39|((uint64_t)PAGETABLE))


#define PTE_FLAG_BIT    10
#define PTE_PPN_BIT     44

#define PTE_V (1<<0)    //有效位
#define PTE_R (1<<1)    //可读属性
#define PTE_W (1<<2)    //可写属性
#define PTE_X (1<<3)    //可执行属性
#define PTE_U (1<<4)    //用户访问模式
#define PTE_G (1<<5)    //全局映射
#define PTE_A (1<<6)    //访问标志位
#define PTE_D (1<<7)    //脏位
   

#define MAXVA       (1L << (9 + 9 + 9 + 12 - 1))
#define TRAMPOLINE  (MAXVA-PAGE_SIZE)
#define TRAPCONTEXT (MAXVA-2*PAGE_SIZE)
#define KSTACK(p)   (TRAMPOLINE-((p)+1)*2*PAGE_SIZE)

#define GROUNDUP(p) (((p) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)) 

typedef struct{
    uint64_t value;
}PhysAddr;

typedef struct{
    uint64_t value;
}VirtAddr;

typedef struct{
    uint64_t value;
}PhysPageNum;

typedef struct{
    uint64_t value;
}VirtPageNum;


typedef struct
{
    uint64_t unused_current;
    uint64_t unused_end;
    Stack recyled;

}StrackFrameAllocator;


typedef struct{
    uint64_t bits;
}PageTableEntry;


typedef struct{
    PhysPageNum root_ppn;
}PageTable;



PhysAddr PhysAddr_form_u64(uint64_t v);
VirtAddr VirtAddr_from_u64(uint64_t v);
PhysPageNum PhysPageNum_from_u64(uint64_t v);
VirtPageNum VirtPageNum_from_u64(uint64_t v);
uint64_t u64_from_PhysAddr(PhysAddr pa);
uint64_t u64_from_VirtAddr(VirtAddr va);
uint64_t u64_from_PhysPageNum(PhysPageNum ppn);
uint64_t u64_from_VirtPageNum(VirtPageNum vpn);
PhysPageNum PhysPageNum_form_PhysAddr(PhysAddr pa);
VirtPageNum VirtPageNum_from_VirtAddr(VirtAddr va);
PhysAddr PhysAddr_from_PhysPageNum(PhysPageNum ppn);
VirtAddr VirtAddr_from_VirtPageNum(VirtPageNum vpn);
void StrackFrameAllocator_Init(StrackFrameAllocator* allocator, PhysPageNum ppn_start, PhysPageNum ppn_end);
PhysPageNum StrackFrameAllocator_alloc(StrackFrameAllocator* allocator);
void StrackFrameAllocator_free(StrackFrameAllocator* allocator, PhysPageNum ppn);


PageTableEntry PageTableEntry_clean();
PhysPageNum PhysPageNum_from_PageTableEntry(PageTableEntry pte);
PageTableEntry* PageTableEntryPtr_from_PhysPageNum(PhysPageNum ppn);
PageTableEntry PageTableEntry_from_PhysPageNum(PhysPageNum ppn);
bool PageTableEntry_is_empty(PageTableEntry pte);
PageTableEntry PageTableEntry_new(PhysPageNum ppn, uint8_t pte_flag);
uint8_t PteFlag_from_PageTableEntry(PageTableEntry pte);
void PTindex_form_VirtPageNum(VirtPageNum vpn, uint64_t *index);
PageTableEntry* Find_Pte_Creat(PageTable* pt, VirtPageNum vpn);
PageTableEntry* Find_Pte(PageTable* pt, VirtPageNum vpn);
void PageTable_map(PageTable* root_pt, VirtPageNum vpn, PhysPageNum ppn, uint8_t pte_flag);
void PageTable_unmap(PageTable* root_pt, VirtPageNum vpn);
void memory_map(PageTable* root_pt, VirtAddr va, PhysAddr pa, uint64_t size, uint8_t flag);
void memory_unmap(PageTable* root_pt, VirtAddr va, uint64_t size);



void FrameAllocator_init();
PhysPageNum kalloc();
PageTable kvmmake();
void kvminit();
void kvminithart();
void kfree(PhysPageNum ppn);
void proc_freepagetable(PageTable* root_pt,uint64_t usize);






#endif
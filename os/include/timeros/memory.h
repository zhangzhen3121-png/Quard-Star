#ifndef __MEMORY_H
#define __MEMORY_H

#include"os.h"
#include"types.h"
#include"stack.h"

#define MEMERY_START 0x80800000
#define MEMERY_END   0x80C00000


#define PAGE_SIZE       0x1000
#define PAGE_SIZE_BIT   0xc

#define PA_WITDH_SV39   56
#define VA_WITDH_SV39   39
#define PPN_WIDTH_SV39  (PA_WITDH_SV39-PAGE_SIZE_BIT)
#define VPN_WIDTH_SV39  (VA_WITDH_SV39-PAGE_SIZE_BIT)


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
void FrameAllocator_test();










#endif
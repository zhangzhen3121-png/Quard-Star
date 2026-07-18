#ifndef __LOADER_H
#define __LOADER_H

#include"types.h"

#define Elf64_Half      uint16_t
#define Elf64_Word      uint32_t
#define Elf64_Addr      uint64_t
#define Elf64_Off       uint64_t
#define Elf64_Xword     uint64_t
#define EI_NIDENT       16
#define ELF_MAG         0x464c457fU
#define EM_X86_64	      62	       /* AMD x86-64 architecture */
#define EM_RISCV_64	    0xF3	     /* RISCV-64 architecture */
#define PT_LOAD		    1	

#define PF_X		(1 << 0)	/* Segment is executable */
#define PF_W		(1 << 1)	/* Segment is writable */
#define PF_R		(1 << 2)	/* Segment is readable */

typedef struct 
{
  unsigned char	e_ident[EI_NIDENT]; /* Magic number and other info */       // 1 byte * 16
  Elf64_Half	e_type;		    /* Object file type */                  // 2 bytes
  Elf64_Half	e_machine;	    /* Architecture */
  Elf64_Word	e_version;	    /* Object file version */               // 4 bytes
  Elf64_Addr	e_entry;	    /* Entry point virtual address */       // 8 bytes
  Elf64_Off	  e_phoff;	    /* Program header table file offset */  // 8 bytes
  Elf64_Off	  e_shoff;	    /* Section header table file offset */      
  Elf64_Word	e_flags;	    /* Processor-specific flags */             
  Elf64_Half	e_ehsize;	    /* ELF header size in bytes */
  Elf64_Half	e_phentsize;	    /* Program header table entry size */
  Elf64_Half	e_phnum;	    /* Program header table entry count */
  Elf64_Half	e_shentsize;	    /* Section header table entry size */
  Elf64_Half	e_shnum;	    /* Section header table entry count */
  Elf64_Half	e_shstrndx;	    /* Section header string table index */
}elf64_ehdr_t;

typedef struct
{
  Elf64_Word	p_type;			/* Segment type */               // 4 bytes
  Elf64_Word	p_flags;		/* Segment flags */
  Elf64_Off	    p_offset;		/* Segment file offset */          // 8 bytes
  Elf64_Addr	p_vaddr;		/* Segment virtual address */    // 8 bytes
  Elf64_Addr	p_paddr;		/* Segment physical address */
  Elf64_Xword	p_filesz;		/* Segment size in file */       // 8 bytes
  Elf64_Xword	p_memsz;		/* Segment size in memory */
  Elf64_Xword	p_align;		/* Segment alignment */
} elf64_phdr;


typedef struct 
{
    uint64_t addr;
    uint64_t size;
}APPMATEDATA;


int get_app_num();

APPMATEDATA get_app_data();
void load_app(int id);

#endif
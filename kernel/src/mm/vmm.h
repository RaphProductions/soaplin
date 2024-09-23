#pragma once

#include <stdint.h>
#include <stddef.h>
#include <mm/pmm.h>

extern uint64_t *vmm_kernel_address_space;
extern uint64_t *vmm_current_address_space;

#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)
#define PTE_GET_FLAGS(VALUE) ((VALUE) & ~PTE_ADDR_MASK)

extern uint64_t hhdm_off;
extern uint64_t kernel_phys_addr;
extern uint64_t kernel_virt_addr; 

#define PHYSICAL_TO_VIRTUAL(ptr) ((void*)((uint64_t)ptr) + hhdm_off)
#define VIRTUAL_TO_PHYSICAL(ptr) ((void*)((uint64_t)ptr) - hhdm_off)

typedef char SymbolT[];

extern SymbolT text_start_ld;
extern SymbolT text_end_ld;

extern SymbolT rodata_start_ld;
extern SymbolT rodata_end_ld;

extern SymbolT data_start_ld;
extern SymbolT data_end_ld;

typedef struct vma_region {
	uint64_t virtualAddr;
	uint64_t end;
	uint64_t pages;
	uint64_t flags;
	uint64_t physicalAddr;
	uint64_t refCount;
	struct vma_region *next;
	struct vma_region *prev;
} __attribute__((packed)) vma_region;

typedef struct pagemap {
	uint64_t *topLevel;
	vma_region *vmaHead;
} __attribute__((packed)) pagemap;

#define VMM_PRESENT 0x01
#define VMM_WRITE 0x02
#define VMM_USER 0x04
#define VMM_WRITE_THROUGH 0x08
#define VMM_NO_CACHE 0x10
#define VMM_ACCESSED 0x20
#define VMM_DIRTY 0x40
#define VMM_HUGE_PAGE 0x80
#define VMM_GLOBAL 0x100
#define VMM_EXECUTE 0x200

void vmm_init();
pagemap *vmm_new_pagemap();
void vmm_load_pagemap(pagemap *pm);
void vmm_map(pagemap *pm, uint64_t vaddr, uint64_t paddr, uint64_t flags);
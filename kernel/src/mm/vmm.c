#include "mm/vmm.h"
#include "limine.h"
#include "mm/pmm.h"
#include "mm/memop.h"
#include "sys/log.h"
#include <stddef.h>
#include <sys/string.h>

static volatile struct limine_paging_mode_request pmrq = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 3,
    .mode = LIMINE_PAGING_MODE_X86_64_4LVL
};

static volatile struct limine_kernel_address_request karq = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

pagemap_t *vmm_kernel_pm = NULL;
pagemap_t *vmm_current_pm = NULL;
int vmm_kernel_pm_exists = 0;

pagemap_t *vmm_alloc_pm() {
    pagemap_t *pm = (pagemap_t*)HIGHER_HALF((uint64_t)pmm_request_page());
    memset(pm, 0, PMM_PAGE_SIZE);


    /*if (vmm_kernel_pm_exists) {
        pm->toplevel = (uint64_t*)HIGHER_HALF((uint64_t)pmm_request_page());
        memset(pm->toplevel, 0, PMM_PAGE_SIZE);

        for (int i = 256; i < 512; i++) {
            pm->toplevel[i] = vmm_kernel_pm->toplevel[i];
        }
    } else {
        
    __asm__ volatile("mov %%cr3, %0" : "=r"(pm->toplevel) : : "memory");
    pm->toplevel = HIGHER_HALF(pm->toplevel);
    logln(info, "vmm", "Limine-provided kernel PML4: %p", pm->toplevel);

    }*/

    pm->toplevel = (uint64_t*)HIGHER_HALF((uint64_t)pmm_request_page());
    memset(pm->toplevel, 0, PMM_PAGE_SIZE);

    if (vmm_kernel_pm_exists) {
        for (int i = 256; i < 512; i++) {
            pm->toplevel[i] = vmm_kernel_pm->toplevel[i];
        }
    }

    return pm;
}

void vmm_init() {
    if (pmrq.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
        logln(panic_lg, "vmm", "Soaplin only supports 4-level paging!\n");
        asm("cli; hlt;");
    }
    vmm_kernel_pm = vmm_alloc_pm();
    vmm_kernel_pm_exists = 1;

    uint64_t kphysaddr = karq.response->physical_base;
    uint64_t kvirtaddr = karq.response->virtual_base;

    uint64_t reqs_start = ALIGN_DOWN((uint64_t)reqs_start_ld, PMM_PAGE_SIZE);
    uint64_t reqs_end = ALIGN_UP((uint64_t)reqs_end_ld, PMM_PAGE_SIZE);
    uint64_t text_start = ALIGN_DOWN((uint64_t)text_start_ld, PMM_PAGE_SIZE);
    uint64_t text_end = ALIGN_UP((uint64_t)text_end_ld, PMM_PAGE_SIZE);
    uint64_t rodata_start = ALIGN_DOWN((uint64_t)rodata_start_ld, PMM_PAGE_SIZE);
    uint64_t rodata_end = ALIGN_UP((uint64_t)rodata_end_ld, PMM_PAGE_SIZE);
    uint64_t data_start = ALIGN_DOWN((uint64_t)data_start_ld, PMM_PAGE_SIZE);
    uint64_t data_end = ALIGN_UP((uint64_t)data_end_ld, PMM_PAGE_SIZE);


    logln(info, "vmm", "Mapping .requests section...\n");
    for (uint64_t req = reqs_start; req < reqs_end; req += PMM_PAGE_SIZE) {
        vmm_map(vmm_kernel_pm, req, req - kvirtaddr + kphysaddr, VMM_PRESENT | VMM_WRITABLE);
    }

    logln(info, "vmm", "Mapping .text section...\n");
    for (uint64_t text = text_start; text < text_end; text += PMM_PAGE_SIZE) {
        vmm_map(vmm_kernel_pm, text, text - kvirtaddr + kphysaddr, VMM_PRESENT);
    }

    logln(info, "vmm", "Mapping .rodata section...\n");
    for (uint64_t roData = rodata_start; roData < rodata_end; roData += PMM_PAGE_SIZE)
        vmm_map(vmm_kernel_pm, roData, roData - kvirtaddr + kphysaddr,
                VMM_PRESENT | VMM_NX);

    logln(info, "vmm", "Mapping .data section...\n");
    for (uint64_t data = data_start; data < data_end; data += PMM_PAGE_SIZE)
        vmm_map(vmm_kernel_pm, data, data - kvirtaddr + kphysaddr,
                VMM_PRESENT | VMM_WRITABLE | VMM_NX);

    logln(info, "vmm", "Mapping address from 0x0 to 0x100000000...\n");
    for (uint64_t gb4 = 0; gb4 < 0x100000000; gb4 += PMM_PAGE_SIZE) {
        vmm_map(vmm_kernel_pm, gb4, gb4, VMM_PRESENT | VMM_WRITABLE);
        vmm_map(vmm_kernel_pm, (uint64_t)HIGHER_HALF(gb4), gb4, VMM_PRESENT | VMM_WRITABLE);
    }

    vmm_load_pagemap(vmm_kernel_pm);

    logln(progress, "vmm", "Initialized!\n");
}

void vmm_load_pagemap(pagemap_t *pm) {
    if (!pm)
        return;

    if (!pm->toplevel)
        return;

    logln(progress, "vmm", "thing! %p %p %llx\n", (uint64_t)pm->toplevel, (uint64_t)PHYSICAL((uint64_t)pm->toplevel), pmm_hhdm_off);
    vmm_current_pm = pm;
    __asm__ volatile("mov %0, %%cr3" : : "r"(PHYSICAL(pm->toplevel)) : "memory");
}

static uint64_t *__vmm_get_next_lvl(uint64_t *level, uint64_t entry) {
  if (level[entry] & 1)
    return HIGHER_HALF(PTE_GET_ADDR(level[entry]));
  uint64_t *pml = HIGHER_HALF(pmm_request_page());
  memset(pml, 0, PMM_PAGE_SIZE);
  level[entry] = (uint64_t)PHYSICAL(pml) | 1 | 2 | 3;
  return pml;
}

void vmm_map(pagemap_t *pm, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm->toplevel, pml4_entry);
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry);
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry);

  pml1[pml1_entry] = paddr | flags;
}

void vmm_unmap(pagemap_t *pm, uint64_t vaddr) {
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm->toplevel, pml4_entry);
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry);
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry);

  pml1[pml1_entry] = 0;
}
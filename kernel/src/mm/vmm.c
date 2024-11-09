// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <mm/vmm.h>
#include <mm/memop.h>
#include <sys/log.h>

uint64_t hhdm_off = 0;
uint64_t kernel_phys_addr = 0;
uint64_t kernel_virt_addr = 0;

pagemap *vmm_kernel_pagemap;
pagemap *vmm_current_pagemap;

void vmm_init() {
  logln(info, "vmm", "Creating the kernel page map...\n");
  /// We can't use vmm_create_pagemap, since it requires us to have the kernel
  /// pagemap, which we don't.
  vmm_kernel_pagemap = (pagemap *)PHYSICAL_TO_VIRTUAL(pmm_allocpage());
  memset(vmm_kernel_pagemap, 0, PAGE_SIZE);

  vmm_kernel_pagemap->topLevel =
      (uint64_t *)(pagemap *)PHYSICAL_TO_VIRTUAL(pmm_allocpage());
  memset(vmm_kernel_pagemap->topLevel, 0, PAGE_SIZE);

  vmm_kernel_pagemap->vmaHead =
      (vma_region *)PHYSICAL_TO_VIRTUAL(pmm_allocpage());
  memset(vmm_kernel_pagemap->vmaHead, 0, PAGE_SIZE);

  vmm_kernel_pagemap->vmaHead->next = vmm_kernel_pagemap->vmaHead;
  vmm_kernel_pagemap->vmaHead->prev = vmm_kernel_pagemap->vmaHead;

  uint64_t physBase = kernel_phys_addr;
  uint64_t virtBase = kernel_virt_addr;

  uint64_t textStart = ALIGN_DOWN((uint64_t)text_start_ld, PAGE_SIZE);
  uint64_t textEnd = ALIGN_UP((uint64_t)text_end_ld, PAGE_SIZE);

  uint64_t roDataStart = ALIGN_DOWN((uint64_t)rodata_start_ld, PAGE_SIZE);
  uint64_t roDataEnd = ALIGN_UP((uint64_t)rodata_end_ld, PAGE_SIZE);

  uint64_t dataStart = ALIGN_DOWN((uint64_t)data_start_ld, PAGE_SIZE);
  uint64_t dataEnd = ALIGN_UP((uint64_t)data_end_ld, PAGE_SIZE);

  logln(info, "vmm", "Mapping .text section...\n");
  for (uint64_t text = textStart; text < textEnd; text += PAGE_SIZE)
    vmm_map(vmm_kernel_pagemap, text, text - virtBase + physBase, 1);

  logln(info, "vmm", "Mapping .rodata section...\n");
  for (uint64_t roData = roDataStart; roData < roDataEnd; roData += PAGE_SIZE)
    vmm_map(vmm_kernel_pagemap, roData, roData - virtBase + physBase,
            1 | ((uint64_t)1 << 63));

  logln(info, "vmm", "Mapping .data section...\n");
  for (uint64_t data = dataStart; data < dataEnd; data += PAGE_SIZE)
    vmm_map(vmm_kernel_pagemap, data, data - virtBase + physBase,
            1 | 2 | ((uint64_t)1 << 63));

  logln(info, "vmm", "Mapping address from 0x0 to 0x100000000...\n");
  for (uint64_t gb4 = 0; gb4 < 0x100000000; gb4 += PAGE_SIZE) {
    vmm_map(vmm_kernel_pagemap, gb4, gb4, 1 | 2);
    vmm_map(vmm_kernel_pagemap, (uint64_t)PHYSICAL_TO_VIRTUAL(gb4), gb4, 1 | 2);
  }

  vmm_load_pagemap(vmm_kernel_pagemap);
  logln(progress, "vmm", "Initialized!\n");
}

pagemap *vmm_new_pagemap() {
  pagemap *pageMap = (pagemap *)PHYSICAL_TO_VIRTUAL(pmm_allocpage());
  if (!pageMap) {
    logln(err, "vmm", "Allocation failed!\n");
    return NULL;
  }
  memset(pageMap, 0, PAGE_SIZE);

  pageMap->topLevel = (uint64_t *)PHYSICAL_TO_VIRTUAL(pmm_allocpage());
  if (!pageMap->topLevel) {
    logln(err, "vmm", "Allocation failed!\n");
    pmm_freepage((uint64_t *)VIRTUAL_TO_PHYSICAL(pageMap));
    return NULL;
  }
  memset(pageMap->topLevel, 0, PAGE_SIZE);

  pageMap->vmaHead = (vma_region *)PHYSICAL_TO_VIRTUAL(pmm_allocpage());
  if (!pageMap->vmaHead) { 
    logln(err, "vmm", "Allocation failed!\n");
    pmm_freepage((uint64_t *)VIRTUAL_TO_PHYSICAL(pageMap->topLevel));
    pmm_freepage((uint64_t *)VIRTUAL_TO_PHYSICAL(pageMap));
    return NULL;
  }
  memset(pageMap->vmaHead, 0, PAGE_SIZE);

  pageMap->vmaHead->next = pageMap->vmaHead;
  pageMap->vmaHead->prev = pageMap->vmaHead;

  for (int i = 256; i < 512; i++) {
    pageMap->topLevel[i] = vmm_kernel_pagemap->topLevel[i];
  }

  return pageMap;
}

void vmm_load_pagemap(pagemap *pml4) {
  if (!pml4)
    return;

  vmm_current_pagemap = pml4;
  uint64_t d = (uint64_t)VIRTUAL_TO_PHYSICAL(pml4->topLevel);
  //logln(info, "vmm", "loading pagemap at %llx\n", d);
  __asm__ volatile("mov %0, %%cr3" : : "r"(d) : "memory");
}

static uint64_t *__vmm_get_next_lvl(uint64_t *level, uint64_t entry) {
  if (level[entry] & 1)
    return PHYSICAL_TO_VIRTUAL(PTE_GET_ADDR(level[entry]));
  uint64_t *pml = PHYSICAL_TO_VIRTUAL(pmm_allocpage());
  memset(pml, 0, PAGE_SIZE);
  level[entry] = (uint64_t)VIRTUAL_TO_PHYSICAL(pml) | 1 | 2 | 3;
  return pml;
}
void vmm_map(pagemap *pm, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm->topLevel, pml4_entry);
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry);
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry);

  pml1[pml1_entry] = paddr | flags;
}

void vmm_unmap(pagemap *pm, uint64_t vaddr) {
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm->topLevel, pml4_entry);
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry);
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry);

  pml1[pml1_entry] = 0;
}

#include "mm/vmm.h"
#include "limine.h"
#include "mm/pmm.h"
#include "mm/memop.h"
#include <stddef.h>
#include <sys/string.h>

static volatile struct limine_paging_mode_request pmrq = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .mode = LIMINE_PAGING_MODE_X86_64_4LVL
};

static volatile struct limine_kernel_address_request karq = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

pagemap_t *kernel_pm = NULL;
int kernel_pm_exists = 0;

pagemap_t *vmm_alloc_pm() {
    pagemap_t *pm = (pagemap_t*)HIGHER_HALF(pmm_request_page());
    memset(pm, 0, PMM_PAGE_SIZE);

    pm->toplevel = (uint64_t*)HIGHER_HALF(pmm_request_page());
    memset(pm->toplevel, 0, PMM_PAGE_SIZE);

    if (kernel_pm_exists) {
        for (int i = 256; i < 512; i++) {
            pm->toplevel[i] = kernel_pm->toplevel[i];
        }
    }

    return pm;
}

void vmm_init() {
    kernel_pm = vmm_alloc_pm();
    kernel_pm_exists = 1;

    uint64_t kphysaddr = karq.response->physical_base;
    uint64_t kvirtaddr = karq.response->virtual_base;

    uint64_t text_start = ALIGN_DOWN((uint64_t)text_start_ld, PMM_PAGE_SIZE);
    uint64_t text_end = ALIGN_UP((uint64_t)text_end_ld, PMM_PAGE_SIZE);
    uint64_t rodata_start = ALIGN_DOWN((uint64_t)rodata_start_ld, PMM_PAGE_SIZE);
    uint64_t rodata_end = ALIGN_UP((uint64_t)rodata_end_ld, PMM_PAGE_SIZE);
    uint64_t data_start = ALIGN_DOWN((uint64_t)data_start_ld, PMM_PAGE_SIZE);
    uint64_t data_end = ALIGN_UP((uint64_t)data_end_ld, PMM_PAGE_SIZE);


    // It should raise an exception here, as we didn't setup the page table.
    __asm__ volatile("mov %0, %%cr3" ::"r"((uint64_t)PHYSICAL(kernel_pm->toplevel)) : "memory");
}
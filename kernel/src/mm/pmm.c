#include "mm/pmm.h"
#include "limine.h"
#include "sys/arch/x86_64/io.h"
#include "sys/errhand/panic.h"
#include "sys/log.h"
#include <stddef.h>
#include <stdint.h>
#include <mm/memop.h>
#include <sys/string.h>

// Kevin Alavik: Reference for implementing this PMM (https://github.com/kevinalavik/nekonix). Thanks :heart:

static volatile struct limine_memmap_request memmap = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

uint64_t pmm_hhdm_off;
static pmm_stack_t stack;

void pmm_test() {
    char *ptr = pmm_request_page();
    logln(info, "pmm/test", "Allocated page at %p\n", ptr);

    char *str = "Hello, memory world!\0";
    memcpy(ptr, str, strlen(str));

    logln(info, "pmm/test", "Got string \"%s\" from the page\n", ptr);

    pmm_free_page((void**)&ptr);
    logln(info, "pmm/test", "Freed the page\n");

    logln(info, "pmm/test", "PMM test: SUCCESS.\n");
}

void pmm_init() {
    uint64_t free_pages = 0;
    pmm_hhdm_off = hhdm.response->offset;

    struct limine_memmap_response *mm =  memmap.response;

    // First pass: compute "free_pages"
    for (int i = 0; i < mm->entry_count; i++) {
        logln(info, "pmm", "entry: %p-%p: %d\n", mm->entries[i]->base, mm->entries[i]->base + mm->entries[i]->length, mm->entries[i]->type);

        if (mm->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            free_pages += DIV_ROUND_UP(
                mm->entries[i]->length, PMM_PAGE_SIZE);
        }
    }

    uint64_t array_size = ALIGN_UP(free_pages * 8, 
        PMM_PAGE_SIZE);
        
    for (uint64_t i = 0; i < mm->entry_count; i++) {
        struct limine_memmap_entry *entry = mm->entries[i];
        if (entry->length >= array_size &&
            entry->type == LIMINE_MEMMAP_USABLE) {

            stack.pages = (uintptr_t *)
                entry->base;
            entry->length -= array_size;
            entry->base += array_size;
        }
    }


    for (uint64_t i = 0; i < mm->entry_count; i++)
    {
        struct limine_memmap_entry *entry = mm->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            for (uint64_t j = 0; j < entry->length; j += 
                PMM_PAGE_SIZE)
            {
                stack.pages[stack.idx++] = entry->base + j;
            }
        }
    }

    stack.idx--;

    stack.max = stack.idx;

    logln(info, "pmm", "%dmb available!\n",
        (stack.max * PMM_PAGE_SIZE) / 1024 / 1024);

    pmm_test();
}

void pmm_dump() {
    logln(info, "pmm", "first 10 pages:\n");
    for (uint64_t i = stack.idx; i > (stack.idx - 10); i--) {
        logln(info, "pmm", "  * %p (%d)\n", stack.pages[i], i);
    }

    logln(info, "pmm", "last 10 pages:\n");
    for (uint64_t i = 0; i < 10; i++) {
        logln(info, "pmm", "  * %p (%d)\n", stack.pages[i], i);
    }
}

void *pmm_request_page() {
    if (stack.idx == 0) {
        // The kernel will always crash if we run out of memory.
        panic(out_of_memory, NULL);
    }

    void *ptr = (void *)stack.pages[--stack.idx];
    //logln(info, "pmm", "allocating page at %p\n", ptr);
    return ptr;
}

void pmm_free_page(void *ptr) {
    if (!ptr)
        return;
    
    if (stack.idx >= stack.max) {
        logln(warn, "pmm", "Stack overflow while freeing a page!\n");
        return;
    }

    if ((uint64_t)ptr > pmm_hhdm_off) {
        return;
    } else {
        stack.pages[stack.idx++] = (uint64_t)ptr;
    }
}
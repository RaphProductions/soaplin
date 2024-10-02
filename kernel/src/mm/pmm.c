// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#include <sys/log.h>
#include <mm/pmm.h>

uintptr_t pmm_stack[STACK_SIZE];
uint32_t pmm_stack_top;
pmm_memblk_t *pmm_free_blocks;
uintptr_t pmm_kernel_base = 0;
uintptr_t pmm_kernel_end = 0;
uintptr_t pmm_kernel_size = 0;
memmap_t *memmap_ptr = NULL;
bool pmm_initialized = false;

uint64_t pmm_used_mem;
uint64_t pmm_free_mem;
uint64_t pmm_total_mem;

void pmm_freepage(void *block)
{
    if (!pmm_initialized)
    {
        logln(err, "pmm", "Cannot use PMM functions before PMM initialization.\n");
        return;
    }

    if (pmm_stack_top == STACK_SIZE)
        return;

    pmm_free_mem += PMM_BLOCK_SIZE;
    pmm_used_mem -= PMM_BLOCK_SIZE;

    pmm_stack[pmm_stack_top] = (uintptr_t)block;
    pmm_stack_top++;
}

void *pmm_allocpage()
{
    if (!pmm_initialized)
    {
        logln(err, "pmm", "Cannot use PMM functions before PMM initialization.\n");
        return NULL;
    }

    if (pmm_stack_top == 0)
        return NULL;

    pmm_free_mem -= PMM_BLOCK_SIZE;
    pmm_used_mem += PMM_BLOCK_SIZE;

    pmm_stack_top--;
    return (void *)pmm_stack[pmm_stack_top];
}

void *pmm_alloc(size_t size)
{
    if (!pmm_initialized)
    {
        logln(err, "pmm", "Cannot use PMM functions before PMM initialization.\n");
        return NULL;
    }

    if (size == 0)
        return NULL;

    size += sizeof(pmm_memblk_t);

    pmm_memblk_t *block = pmm_free_blocks;
    pmm_memblk_t *prev_block = NULL;

    while (block)
    {
        if (block->is_free && block->size >= size)
        {
            block->is_free = false;

            if (block->size > size + sizeof(pmm_memblk_t))
            {
                pmm_memblk_t *new_block = (pmm_memblk_t *)((uint8_t *)block + size);
                new_block->size = block->size - size;
                new_block->next = block->next;
                new_block->is_free = true;

                block->size = size;
                block->next = new_block;
            }

            return (uint8_t *)block + sizeof(pmm_memblk_t);
        }

        prev_block = block;
        block = block->next;
    }

    block = (pmm_memblk_t *)pmm_allocpage();
    if (!block)
    {
        return NULL;
    }

    block->size = size;
    block->is_free = false;
    block->next = NULL;

    if (prev_block)
    {
        prev_block->next = block;
    }
    else
    {
        pmm_free_blocks = block;
    }

    return (uint8_t *)block + sizeof(pmm_memblk_t);
}

void pmm_free(void *ptr)
{
    if (!pmm_initialized)
    {
        logln(err, "pmm", "Cannot use PMM functions before PMM initialization.\n");
        return;
    }

    if (!ptr)
    {
        return;
    }

    pmm_memblk_t *block = (pmm_memblk_t *)((uint8_t *)ptr - sizeof(pmm_memblk_t));
    block->is_free = true;

    while (block->next && block->next->is_free)
    {
        block->size += block->next->size;
        block->next = block->next->next;
    }
}

void* pmm_realloc(void* ptr, size_t size) {
    if (!pmm_initialized)
    {
        logln(err, "pmm", "Cannot use PMM functions before PMM initialization.\n");
        return NULL;
    }

    if (size == 0) {
        pmm_free(ptr);
        return NULL;
    }

    void* new_ptr = pmm_alloc(size);
    if (new_ptr == NULL) {
        return NULL;  // Allocation failed
    }

    // Copy data from the old block to the new block
    size_t old_size = 0;
    if (ptr) {
        // Find the size of the old block
        void* temp_ptr = ptr;
        while (*(char*)temp_ptr != '\0') {
            temp_ptr++;
            old_size++;
        }
        old_size++;  // Include the null terminator
        // Copy the data
        for (size_t i = 0; i < old_size; i++) {
            *((char*)new_ptr + i) = *((char*)ptr + i);
        }
    }

    pmm_free(ptr);  // Free the old block

    return new_ptr;
}

void pmm_init() {
    memmap_t *mmap = memmap_ptr;
    pmm_stack_top = 0;

    pmm_initialized = true; // We must say the PMM is initialized here.

    for (uint64_t entry = 0; entry < mmap->entry_count; entry++)
    {
        memmap_entry_t *mentry = mmap->entries[entry];
        
        if (mentry->type != MEMMAP_RESERVED) {
        uint64_t length = mentry->length;
        while (length >= PMM_BLOCK_SIZE)
        {
            pmm_total_mem += PMM_BLOCK_SIZE;
            length -= PMM_BLOCK_SIZE;
        }}

        if (mentry->type == MEMMAP_USABLE)
        {
            uintptr_t addr = mentry->base;
            uint64_t length = mentry->length;

            logln(info, "pmm", "Found an usable memory range: %p-%p\n", addr, addr + length);

            while (length >= PMM_BLOCK_SIZE)
            {
                pmm_freepage((void *)addr);
                addr += PMM_BLOCK_SIZE;
                length -= PMM_BLOCK_SIZE;
            }
        }
    }
    logln(progress, "pmm", "Initialized!\n");
}

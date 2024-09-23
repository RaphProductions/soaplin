#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 512
#define PAGE_FLAGS 0xFFF
#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4
#define PAGE_2MB 0x80
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3FF)
#define GET_PAGE_ADDRESS(x) ((x) & ~0xFFF)

#define DIV_ROUND_UP(x, y) (x + (y - 1)) / y
#define ALIGN_UP(x, y) DIV_ROUND_UP(x, y) * y
#define ALIGN_DOWN(x, y) (x / y) * y

#define PMM_BLOCK_SIZE 4096
#define PMM_MAX_BLOCKS 32768
#define STACK_SIZE PMM_MAX_BLOCKS

#define MEMMAP_USABLE                 0
#define MEMMAP_RESERVED               1
#define MEMMAP_ACPI_RECLAIMABLE       2
#define MEMMAP_BOOTLOADER_RECLAIMABLE 5

typedef struct memmap_entry {
    uint64_t base;
    uint64_t length;
    uint64_t type;
} memmap_entry_t;

typedef struct memmap {
    uint64_t reserved;
    uint64_t entry_count;
    memmap_entry_t **entries;
} memmap_t;

/* Memory block */
typedef struct pmm_memblk
{
    size_t size;
    struct pmm_memblk *next;
    bool is_free;
} pmm_memblk_t;

#ifdef __cplusplus
extern "C" {
#endif

void  pmm_init();
void *pmm_alloc(size_t size);
void *pmm_allocpage();
void  pmm_free(void *ptr);
void  pmm_freepage(void *ptr);
void* pmm_realloc(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif
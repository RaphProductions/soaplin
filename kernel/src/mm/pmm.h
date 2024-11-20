#pragma once

#include <stdint.h>

#define PMM_PAGE_SIZE 4096

#define DIV_ROUND_UP(x, y) (((uint64_t)(x) + ((uint64_t)(y) - 1)) / (uint64_t)(y))
#define ALIGN_UP(x, y) (DIV_ROUND_UP(x, y) * (uint64_t)(y))
#define ALIGN_DOWN(x, y) (((uint64_t)(x) / (uint64_t)(y)) * (uint64_t)(y))

#define HIGHER_HALF(x) (uint64_t)x + pmm_hhdm_off
#define PHYSICAL(x) (uint64_t)x - pmm_hhdm_off

extern uint64_t pmm_hhdm_off;

typedef struct pmm_stack {
    uintptr_t *pages;
    uint64_t idx;
    uint64_t max;
} pmm_stack_t;

void pmm_init();
void pmm_dump();
void *pmm_request_page();
void pmm_free_page(void **ptr);
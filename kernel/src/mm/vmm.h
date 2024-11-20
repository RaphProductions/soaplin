#pragma once

#include <stdint.h>


typedef char sym[];

extern sym text_start_ld;
extern sym text_end_ld;

extern sym rodata_start_ld;
extern sym rodata_end_ld;

extern sym data_start_ld;
extern sym data_end_ld;


typedef struct pagemap {
    uint64_t *toplevel;
} pagemap_t;

extern pagemap_t *kernel_pm;

void vmm_init();
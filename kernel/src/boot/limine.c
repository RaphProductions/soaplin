#include <limine.h>
#include <dev/fb.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <stdint.h>

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_kernel_address_request kaddr_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

extern framebuffer *__framebuf;
extern memmap_t *memmap_ptr;
extern uint64_t hhdm_off;
extern uint64_t kernel_phys_addr;
extern uint64_t kernel_virt_addr;

void limine_populate()
{
    __framebuf = (framebuffer*)framebuffer_request.response->framebuffers[0];
    memmap_ptr = (memmap_t*)memmap_request.response;
    kernel_phys_addr = kaddr_request.response->physical_base;
    kernel_virt_addr = kaddr_request.response->virtual_base;
    hhdm_off = hhdm_request.response->offset;
}
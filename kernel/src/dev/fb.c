#include <dev/fb.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

framebuffer *fb_get(int num) {
    // Work smarter, not harder
    return (framebuffer*)framebuffer_request.response->framebuffers[num];
}


struct limine_framebuffer *fb_get_limine(int num) {

    return framebuffer_request.response->framebuffers[num];
}
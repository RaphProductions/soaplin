#include <dev/fb.h>
#include <limine.h>

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

framebuffer *fb_get() {
    // We are lazy asses
    // What lazy asses do? They use the same structure,
    // with some useless end fields removed, and they casts
    // the original structure to the new!
    return framebuffer_request.response->framebuffers[0];
}
#pragma include

#include <stdint.h>

typedef struct {
    unsigned char size;
    unsigned char shift;
} fb_colormask;

typedef struct {
    uint64_t *base;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    unsigned short bpp;

    void *reserved;

    fb_colormask red_mask;
    fb_colormask green_mask;
    fb_colormask blue_mask;
} framebuffer;
#include <stdint.h>
#include <dev/fb.h>

uint32_t* fb;

// yes i made it using chatgpt :()
// i'm not really a pro in high-level graphics,
// and this is just a test program to test the kernel APIs...

#define FIXED_POINT_SCALE 256
#define PROJECTION_FACTOR (200 * FIXED_POINT_SCALE)
#define MAX_DEPTH 1000
#define NUM_STARS 200

#define SPEED_DIVISOR 100  // Higher divisor for slower speed

// Framebuffer pointer, which should point to the start of the allocated framebuffer memory.
uint32_t* fb;
uint32_t fb_width;
uint32_t fb_height;


// Double buffering setup
uint32_t* front_buffer;
uint32_t* back_buffer;

// Star structure without dynamic allocation
typedef struct {
    int x, y, z;   // Position
    int speed;  // Speed of movement
    int brightness;  // Brightness of the star
} Star;

// Array to hold the stars
Star stars[NUM_STARS];

// Random number generator state
unsigned int rand_state = 1;

// Custom seed function
void srand(unsigned int seed) {
    rand_state = seed;
}

// Custom random function (simple linear congruential generator)
int rand() {
    rand_state = rand_state * 1103515245 + 12345;
    return (rand_state >> 16) & 0x7FFF;
}

// Initialize stars
void init_stars() {
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = rand() % fb_width;
        stars[i].y = rand() % fb_height;
        stars[i].speed = (rand() % 3) + 1;
        stars[i].brightness = 0xFF; // Set stars to white
    }
}

// Set a pixel in the back buffer
void set_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < fb_width && y >= 0 && y < fb_height) {
        back_buffer[y * fb_width + x] = color;
    }
}

// Project 3D star coordinates to 2D screen coordinates
void project_star(const Star* star, int* screen_x, int* screen_y, int* brightness) {
    // Ensure z is never zero to avoid division issues
    int z = (star->z > 1) ? star->z : 1;

    // Calculate perspective projection factor in fixed-point
    int factor = PROJECTION_FACTOR / z;

    // Convert 3D coordinates to 2D screen position, centered on the screen
    *screen_x = fb_width / 2 + (star->x * factor) / FIXED_POINT_SCALE;
    *screen_y = fb_height / 2 + (star->y * factor) / FIXED_POINT_SCALE;

    // Calculate brightness based on depth (closer stars are brighter)
    *brightness = 255;// * (MAX_DEPTH - z) / MAX_DEPTH;
}

// Update the starfield in the back buffer
void update_stars() {
    int width = fb_width;
    int height = fb_height;

    // Clear the back buffer (set to black)
    for (int i = 0; i < width * height; i++) {
        back_buffer[i] = 0x00000000; // Black background
    }

    // Update each star
    for (int i = 0; i < NUM_STARS; i++) {
        // Move star towards the viewer by decreasing z with integer speed
        stars[i].z -= stars[i].speed;

        // If the star goes past the viewer, reset it to a far distance
        if (stars[i].z <= 1) {
            stars[i].x = (rand() % width) - width / 2;
            stars[i].y = (rand() % height) - height / 2;
            stars[i].z = MAX_DEPTH;

            // Set integer speed in fixed-point, scaled for finer control
            stars[i].speed = (rand() % 5 + 1) * FIXED_POINT_SCALE / 150;
        }

        // Project the star's 3D position to 2D screen coordinates
        int screen_x, screen_y, brightness;
        project_star(&stars[i], &screen_x, &screen_y, &brightness);

        // Draw the star as a bright pixel if it’s visible on screen
        if (brightness > 0 && screen_x >= 0 && screen_x < width && screen_y >= 0 && screen_y < height) {
            uint32_t color = (brightness << 16) | (brightness << 8) | brightness; // Grayscale color
            set_pixel(screen_x, screen_y, color);
        }
    }
}

// Swap the front and back buffers
void swap_buffers() {
    // Swap the pointers
    uint32_t* temp = front_buffer;
    front_buffer = back_buffer;
    back_buffer = temp;

    // Point the framebuffer to the new front buffer to display it
    fb = front_buffer;
}

void starssc(framebuffer *fb2) {
  fb = (uint32_t*)fb2->base;
  fb_width = fb2->width;
  fb_height = fb2->height;
  front_buffer = fb;
  back_buffer = fb + fb_width * fb_height; // Point to the next screen's worth of space

  srand(2930983);
  init_stars();

  // The kernel finished it's boot things. Now halt the machine. Interrupts will
  // still be received & handled.
  while (1) {
    update_stars();
    swap_buffers();
  }
}
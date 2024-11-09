#include <arch/x86_64/io.h>

#define PS2_IOPORT 0x60

void i8042_init()
{
    // We don't have ACPI yet, assume the i8042 is here.

    outb(PS2_IOPORT, 0xAD);
    outb(PS2_IOPORT, 0xA7);

    inb(PS2_IOPORT);
}
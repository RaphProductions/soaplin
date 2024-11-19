// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.
#include "dev/serio.h"
#include "limine.h"
#include <sys/acpi.h>
#include <sys/arch/x86_64/gdt.h>
#include <sys/arch/x86_64/idt.h>
#include "sys/acpi/madt.h"
#include "sys/arch/x86_64/pic.h"
#include "sys/log.h"
#include <dev/fb.h>
#include <dev/tty.h>
#include <stdint.h>

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

extern void starssc(framebuffer *fb2);

void kmain_x86_64() {
    serio_init();
    tty_enable();
    tty_printf("Soaplin 0.1 Zephyr (zephyr_rewrite)\n");
    tty_printf("The kernel booted successfully! Wait a bit to get a cool screensaver!\n");
    
    logln(info, "kernel", "Starting kernel initialization process...\n");

    gdt_init();
    idt_init();

    acpi_init();

    if (acpi_available)
        madt_init();
    else
        pic_init();
    pic_unmask_irq(1);

    //asm("int $0x03");
    starssc(fb_get(0));
}

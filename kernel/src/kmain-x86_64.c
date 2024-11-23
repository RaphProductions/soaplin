// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.
#include "dev/serio.h"
#include "limine.h"
#include <sys/acpi.h>
#include <sys/arch/x86_64/gdt.h>
#include <sys/arch/x86_64/idt.h>
#include "mm/vmm.h"
#include "sys/acpi/madt.h"
#include "sys/arch/x86_64/pic.h"
#include "sys/arch/x86_64/pit.h"
#include "sys/arch/x86_64/rtc.h"
#include "sys/log.h"
#include "sys/sched.h"
#include <dev/fb.h>
#include <dev/tty.h>
#include <stdint.h>
#include <mm/pmm.h>

__attribute__((used, section(".limine_requests")))       static volatile LIMINE_BASE_REVISION(0);
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end")))   static volatile LIMINE_REQUESTS_END_MARKER;

extern void starssc(framebuffer *fb2);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_bootloader_info_request blir = {
    .revision = 3,
    .id = LIMINE_BOOTLOADER_INFO_REQUEST
};

void kmain_x86_64() {
    serio_init();

    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        logln(panic_lg, "kernel", "Limine Base Revision is not supported.\n");
        asm("cli; hlt;");
    }

    tty_enable();
    tty_printf("Soaplin 0.1 Zephyr (zephyr_rewrite)\n");
    tty_printf("Booted using %s %s\n", blir.response->name, blir.response->version);
    
    logln(info, "kernel", "Starting kernel initialization process...\n");

    gdt_init();
    idt_init();

    acpi_init();

    if (acpi_available)
        madt_init();
    else {
        pic_init();
        pit_init(1000); // 1000 times per second.
        pic_unmask_irq(1);
    }

    pmm_init();
    vmm_init();

    rtc_init();

    time t = rtc_get();
    logln(info, "kernel", "Today is %d/%d/%d %d:%d:%d\n",
        t.day_of_month,
        t.month,
        t.year + 2000,
        t.hours,
        t.minutes,
        t.seconds
    );

    sched_init();
    pit_enable();

    logln(info, "kernel", "Soaplin has been successfully initialized!\n");

    while (1)
        asm("hlt");
}

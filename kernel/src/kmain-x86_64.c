#include <dev/serio.h>
#include <limine.h>
#include <dev/tty.h>
#include <sys/log.h>
#include <arch/arch.h>

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

void kmain_x86_64()
{
    serio_init();
    tty_enable();

    logln(info, "kernel", "Soaplin v0.1 - Zephyr (Milestone 1)\n");

    arch_init_stage1();

    //asm("int $3");

    // The kernel finished it's boot things. Now halt the machine. Interrupts will still be received & handled.
    while (1)
        asm("hlt");
}
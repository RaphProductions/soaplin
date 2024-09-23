#include <dev/serio.h>
#include <dev/tty.h>
#include <sys/log.h>
#include <arch/arch.h>
#include <boot/limine.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

void kmain_x86_64()
{
    limine_populate();
    serio_init();
    tty_enable();

    logln(info, "kernel", "Soaplin v0.1 - Zephyr (Milestone 1)\n");

    arch_init_stage1();
    pmm_init();
    vmm_init();

    //asm("int $3");

    // The kernel finished it's boot things. Now halt the machine. Interrupts will still be received & handled.
    while (1)
        asm("hlt");
}
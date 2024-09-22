#include <arch/arch.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/interrupts.h>
#include <sys/log.h>

void arch_init_stage1()
{
    gdt_init();
    idt_init();
    logln(progress, "arch", "Stage 1 initialization complete\n");
}
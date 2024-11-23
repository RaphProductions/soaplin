#include "mm/pmm.h"
#include "mm/vmm.h"
#include "sys/arch/x86_64/pic.h"
#include "sys/log.h"
#include "sys/sched.h"
#include <stdint.h>
#include <sys/arch/x86_64/idt.h>
#include <sys/arch/x86_64/io.h>
#include <sys/errhand/panic.h>

int pit_millis = 0;
int pit_secs = 0;

void exception_handler(registers_t *regs) {

    if (regs->int_no < 32) {
        panic(kmode_cpu_exception, regs);
    }

    if (regs->int_no == 1 + 32)
    {
        if (inb(0x60) & 0x80)
        {
            pic_ack(regs->int_no - 32);
            return;
        }

        logln(info, "ints", "Keyboard\n");
    }

    else if (regs->int_no == 0x80 - 32 || regs->int_no == 32) {
        /*uint64_t *mem = pmm_request_page();
        mem[0] = 0xFEEB; 

        pagemap_t *pm = vmm_alloc_pm();
        vmm_map(pm, 0x1000, (uint64_t)mem, VMM_PRESENT);

        logln(info, "kernel", "%p", pm->toplevel);
        regs->rip = 0x1000;
        vmm_load_pagemap(pm);*/
        
        if (pit_millis == 9999)
        {
            pit_secs++;
            pit_millis = 0;
        }
        else
            pit_millis++;

        schedule(regs);
    }

    //logln(info, "arch/ints", "Received interrupt %d\n", regs->int_no);
    pic_ack(regs->int_no - 32);
}
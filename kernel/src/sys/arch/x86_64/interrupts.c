#include "sys/arch/x86_64/pic.h"
#include "sys/log.h"
#include <sys/arch/x86_64/idt.h>
#include <sys/arch/x86_64/io.h>
#include <sys/errhand/panic.h>

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
    }

    logln(info, "arch/ints", "Received interrupt %d\n", regs->int_no);
    pic_ack(regs->int_no - 32);
}
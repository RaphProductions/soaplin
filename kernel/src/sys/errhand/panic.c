#include "sys/log.h"
#include <sys/arch/x86_64/idt.h>

int panic_cnt = 0;

char *gdt_seg_desc[65] = {
    "Null segment",
    "16-bit code segment",
    "16-bit data segment",
    "32-bit code segment",
    "32-bit data segment",
    "64-bit code segment",
    "64-bit data segment",
    "User-mode code segment", // 0x38
    "User-mode data segment" // 0x40
};

char *err_msg[2] = {
    "kmode_cpu_exception",
    "out_of_memory"
};

void panic(int err, registers_t *regs) {
    
    panic_cnt++;

    if (regs) {
        logln(panic_lg, "kernel", "%s%s (int_no: %d, rip: %p, cs: %x: %s, ss: %x: %s)\n", panic_cnt > 1 ? "NESTED " : "", err_msg[err], regs->int_no, regs->rip, regs->cs, 
        gdt_seg_desc[regs->cs == 0 ? 0 : regs->cs / 8], regs->ss, gdt_seg_desc[regs->ss == 0 ? 0 : regs->ss / 8]);
    } else {
        logln(panic_lg, "kernel", "%s%s\n", panic_cnt > 1 ? "NESTED " : "", err_msg[err]);
    }

    if (panic_cnt == 1) {
        //logln(panic_lg, "kernel", regs->rip * regs->cs * regs->err_code);
        logln(panic_lg, "kernel", "\n");
        
        if (regs) {
            logln(panic_lg, "kernel", "--- Registers\n");
            logln(panic_lg, "kernel", "R15: %lx, R14: %lx, R13: %lx, R12: %lx, R11: %lx, R10: %lx\n", regs->r15, regs->r14, regs->r13, regs->r12, regs->r11, regs->r10);
            logln(panic_lg, "kernel", "R9: %lx, R8: %lx, RAX: %lx, RDI: %lx, RSI: %lx, RDX: %lx\n", regs->r9, regs->r8, regs->rax, regs->rdi, regs->rsi, regs->rdx);
            logln(panic_lg, "kernel", "RCX: %lx, RBX: %lx, RDX: %lx, RSP: %lx, RBP: %lx, RIP: %lx\n", regs->rcx, regs->rbx, regs->rdx, regs->rsp, regs->rbp, regs->rip);
            logln(panic_lg, "kernel", "CS: %x, SS: %x, RFLAGS: %d, INT: %d, ERR: %lx\n", regs->cs, regs->ss, regs->rflags, regs->int_no, regs->err_code);
        
            logln(panic_lg, "kernel", "--- Stackframe\n");
            stackframe_t *stk;
            __asm__ volatile ("mov %%rbp, %0" : "=r"(stk));
            for (uint64_t i = 0; i < 10 && stk; ++i) {
                logln(panic_lg, "kernel", "  %lx\n", stk->rip);
                stk = stk->rbp;
            }
            
            logln(panic_lg, "kernel", "  (end of stackframe)\n");
        }
    }
    else {
        logln(panic_cnt, "kernel", "Looks like the panic handler caused another panic...\n");
        logln(panic_cnt, "kernel", "For safety reasons, we will shut up and halt the PC.\n");
    }
    __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}
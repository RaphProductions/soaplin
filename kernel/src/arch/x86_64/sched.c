// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#ifdef __x86_64__

#include <core/sched.h>
#include <sys/log.h>

uint64_t pid_counter = 0;
process *current_process = NULL;
process *process_list = NULL;
process *idle_process = NULL;

int sched_idle()
{
    logln(info, "idle", "Hello!\n");

    while (1)
        ;;

    return 0;
}

void sched_init()
{
    current_process = (process *)pmm_alloc(sizeof(process));
    current_process->pm = vmm_new_pagemap();
    current_process->next = current_process;
    current_process->pid = -1;
    process_list = current_process;

    sched_new("idle", sched_idle);
}

void __sched_entry(int (*entry)()) // Todo: this should be handled by the LibC 
{
    // Having a "bootstrapper" is necessary since most of programs are exiting by returning. This handles the situation.
    int result = entry();
    logln(info, "sched", "Process %d exited with exit code %d!\n", current_process->pid, result);

    sched_kill(current_process);
    __asm__ volatile("int $32");
}

process *sched_new(char *name, int (*entry_point)())
{
    process *new_proc = (process *)pmm_alloc(sizeof(process));
    memcpy(new_proc->name, name, PROCESS_NAME_SIZE);
    new_proc->stack_ptr = (uint64_t)pmm_alloc(4096) + 4095;
    new_proc->pm = vmm_new_pagemap();
    new_proc->pid = pid_counter;

    new_proc->regs.rip = (uint64_t)__sched_entry;
    new_proc->regs.rdi = (uint64_t)entry_point;
    new_proc->regs.cs = 0x08;
    new_proc->regs.rflags = 0x202;
    new_proc->regs.rsp = new_proc->stack_ptr;
    new_proc->regs.ss = 0x10;

    new_proc->next = process_list->next;
    process_list->next = new_proc;

    pid_counter++;
    
    logln(info, "sched", "created process \"%s\": PID: %d, RIP: %p, Pagemap: %p%s\n", new_proc->name, new_proc->pid, new_proc->regs.rip, new_proc->pm, new_proc->pm ? "" : " (kernel mode)");

    return new_proc;
}

void sched_kill(process *proc)
{
    process *prev_proc = process_list;

    while (prev_proc->next != proc)
    {
        prev_proc = prev_proc->next;
    }

    prev_proc->next = proc->next;

    pmm_free((void *)proc->stack_ptr - 4095);
    pmm_free(proc);
}

extern void sched_ctxswitch(stackframe *regs);

void schedule(stackframe *regs)
{
    memcpy(current_process->regs, regs, sizeof(stackframe));
    current_process = current_process->next;
    memcpy(regs, current_process->regs, sizeof(stackframe));

    vmm_load_pagemap(current_process->pm);
    //sched_ctxswitch(regs); 
}

#endif

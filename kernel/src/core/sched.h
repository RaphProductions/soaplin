// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#pragma once

#include <arch/arch.h>
#include <mm/vmm.h>

#define PROCESS_NAME_SIZE 128

typedef struct process
{
    char name[PROCESS_NAME_SIZE];
    stackframe regs;
    uint64_t stack_ptr;
    struct Process *next;
    pagemap *pm;
    int pid;
} process;

extern process *current_process;
extern process *process_list;

void sched_init();
process *sched_new(char *name, int (*entry_point)());
void sched_kill(process *proc);
void schedule(stackframe *regs);

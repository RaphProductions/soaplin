// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <arch/arch.h>
#include <boot/limine.h>
#include <stddef.h>
#include <dev/serio.h>
#include <dev/tty.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <sys/log.h>
#include <stdarg.h>
#include <core/syscall/syscalls.h>
#include <core/sched.h>
#include <core/lock/spinlock.h>

void *execute_syscall(uint64_t num, ...)
{
    void *params[6];

    va_list arg_list;
    va_start(arg_list, num);

    for (int j = 0; j < 6; j++)
    {
        void *param = va_arg(arg_list, void *);
        if (param == NULL)
            params[j] = 0;
        else
            params[j] = param;
    }

    __asm__ volatile(
        "movq %0, %%rax;"
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
        "movq %4, %%r10;"
        "movq %5, %%r8;"
        "movq %6, %%r9;"
        "int $0x80;"
        :
        : "r"((uint64_t)num), "r"((uint64_t)params[0]), "r"((uint64_t)params[1]), "r"((uint64_t)params[2]), "r"((uint64_t)params[3]), "r"((uint64_t)params[4]), "r"((uint64_t)params[5])
        : "rax", "rdi", "rsi", "rdx", "r10", "r8");

    va_end(arg_list);
    void *result;
    __asm__ volatile("" : "=a"(result) : : "memory");
    return result;
}

void proc1_thread2() {
  logln(info, "proc1", "hi from second thread\n");

  while (1)
    ;;
}

void proc1_thread1() {
  //logln(info, "proc1", "hi from first thread\n");
  execute_syscall(1, 1, "Hello World from Proc1 (thread 0) !\n", 35);
  //sched_new_thread(sched_current_process, "proc1_t2", proc1_thread2);

  while (1)
    ;;
}

void kmain_x86_64() {

  limine_populate();
  serio_init();
  tty_enable();

  logln(info, "kernel", "Soaplin v0.1 - Zephyr (Milestone 1)\n");
  logln(info, "kernel", "Copyright (C) 2024 Sipaa Projects\n");
  logln(info, "kernel", "Soaplin is FOSS software distrubuted under the MIT License\n");

  arch_init_stage1();
  syscall_init();

  pmm_init();
  vmm_init();
  logln(progress, "kernel", "Memory management initialized\n");

  //__asm__ volatile("int $3");

  arch_init_stage2();

  logln(warn, "kernel", "Due to something being broken in the kernel's user mode\n");
  logln(warn, "kernel", "implementation, all processes with the same privilege as\n");
  logln(warn, "kernel", "the kernel. Please be careful on what you run here. Thanks\n\n");

  logln(info, "kernel", "The kernel booted successfully.\n");

  sched_process *proc1p = sched_new("Proc1");
  sched_new_thread(proc1p, "proc1p1", proc1_thread1);

  //sched_new_thread(proc1p, proc1_thread1);

  /*sched_thread *currth = sched_threads_list;
  while (currth != NULL) {
    logln(info, "kernel", "Name: %s, TID: %d, Parent: %s (%d)\n", currth->name, currth->tid, currth->parent->name, currth->parent->pid);
    currth = currth->next;
  }*/

  arch_init_fini();

  // The kernel finished it's boot things. Now halt the machine. Interrupts will
  // still be received & handled.
  while (1)
    __asm__ volatile("hlt");
}

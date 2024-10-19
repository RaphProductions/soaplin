// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <arch/arch.h>
#include <boot/limine.h>
#include <dev/serio.h>
#include <dev/tty.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <sys/log.h>

void kmain_x86_64() {

  limine_populate();
  serio_init();
  tty_enable();

  logln(info, "kernel", "Soaplin v0.1 - Zephyr (Milestone 1)\n");
  logln(info, "kernel", "Copyright (C) 2024 Sipaa Projects\n");
  logln(info, "kernel", "Soaplin is FOSS software distrubuted under the MIT License\n");

  arch_init_stage1();

  pmm_init();
  vmm_init();
  logln(progress, "kernel", "Memory management initialized\n");

  //__asm__ volatile("int $3");

  arch_init_stage2();

  logln(info, "kernel", "The kernel booted successfully.\n");

  // The kernel finished it's boot things. Now halt the machine. Interrupts will
  // still be received & handled.
  while (1)
    __asm__ volatile("hlt");
}

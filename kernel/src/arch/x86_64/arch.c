// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <arch/arch.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/pit.h>
#include <core/sched.h>
#include <sys/log.h>

void arch_init_stage1() {
  gdt_init();
  idt_init();
  logln(progress, "arch", "Stage 1 initialization complete\n");
}

void arch_init_stage2() {
  pit_init(1000);
  sched_init();
  logln(progress, "arch", "Stage 2 initialization complete\n");
}

void arch_init_fini() {
  pit_enable();
}
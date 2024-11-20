#pragma once

#include <sys/arch/x86_64/idt.h>

#define kmode_cpu_exception 0
#define out_of_memory 1

void panic(int err, registers_t *regs);
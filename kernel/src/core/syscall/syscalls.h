#pragma once

#include <arch/arch.h>
#include <stdbool.h>

typedef void(*syscall_func)(stackframe *stackframe);

#define SYSCALL_MAX_SLOTS 512

extern syscall_func syscalls[SYSCALL_MAX_SLOTS];

bool syscall_set(int syscallnum, syscall_func f);
void syscall_init();
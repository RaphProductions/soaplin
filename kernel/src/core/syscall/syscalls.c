#include <core/syscall/syscalls.h>
#include <sys/log.h>
#include <stddef.h>

syscall_func syscalls[SYSCALL_MAX_SLOTS];

bool syscall_set(int syscallnum, syscall_func f) {
    if (syscallnum >= SYSCALL_MAX_SLOTS)
        logln(err, "syscalls", "\"syscallnum\" is too big! Ensure it's below %d\n", SYSCALL_MAX_SLOTS);

    if (syscalls[syscallnum] != NULL)
        logln(err, "syscalls", "Cannot replace already set syscalls.\n");

    syscalls[syscallnum] = f;
}

extern void syscall_write(stackframe *sf);
extern void syscall_getpid(stackframe *sf);

void syscall_init() {
    // Initialize the syscall table - set all syscalls to NULL.
    for (int i = 0; i < SYSCALL_MAX_SLOTS; i++) {
        syscalls[i] = NULL;
    }

    // Add the kernel's default syscalls
    syscall_set(1, syscall_write);
    syscall_set(2, syscall_getpid);
    
    logln(info, "syscalls", "System calls initialized.\n");
}
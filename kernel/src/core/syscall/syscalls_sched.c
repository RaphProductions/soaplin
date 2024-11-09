#include <core/sched.h>
#include <core/syscall/syscalls.h>

void syscall_getpid(stackframe *sf) {
    // Just set the return value to the process' PID
    sf->rax = sched_current_process->pid;
}
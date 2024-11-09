#include <core/syscall/syscalls.h>
#include <dev/tty.h>

void syscall_write(stackframe *sf) {
    int fd = sf->rdi;
    char *buf = (char*)sf->rsi;
    int count = sf->rdx;

    if (!buf) {
        sf->rax = 0;
        return;
    }

    if (fd == 1) {
        for (int i = 0; i < count; i++) {
            tty_print(&buf[i], 1);
        }
    }
}
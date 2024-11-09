// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <dev/tty.h>
#include <sys/log.h>
#include <sys/panic.h>

char *ex_msgs[] = {"UNKNOWN", "KMODE_CPU_EXCEPTION", "DEADLOCKED_KERNEL_COMPONENT"};
char *explainations[] = {
    "We can't give any info: this panic cause is unknown to us.",
    "Your CPU thrown an exception interrupt due to it being in a bad state.",
    "A component of the operating system kernel entered a deadlocked state."
};

void panic(int cause, void *args)
{
    logln(info, "kernel", "panic! %s\n", ex_msgs[cause]);

    // tty_set_bg(0x0000FF);
    // tty_set_fg(0xFFFFFF);
    // tty_set_margin(0);
    tty_clear();

    tty_printf("  ,-.,--.   ,--.,-.   \n");
    tty_printf(" / .' \\  `.'  / '. \\  \n");
    tty_printf("|  |   .'    \\   |  | \n");
    tty_printf("|  |  /  .'.  \\  |  | \n");
    tty_printf(" \\ '.'--'   '--'.' /  \n");
    tty_printf("  `-'           `-'   \n");

    tty_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
               "!!!!!!!!!!\n\n");
    tty_printf("A problem has been detected and your Soaplin-based OS has been "
               "shut down\n");
    tty_printf("to prevent damage to your computer.\n\n");

    tty_printf("%s\n\n", explainations[cause > MAX ? 0 : cause]);

    tty_printf("If this is the first time you've seen this Stop error screen,\n");
    tty_printf("restart your computer. if this screen appears again, follow\n");
    tty_printf("these steps:\n\n");

    tty_printf("Check to make sure any new hardware or software is properly "
               "installed.\n");
    tty_printf("If this is a new installation, ask your hardware or software "
               "manufacturer\n");
    tty_printf("for any kernel patches you might need.\n\n");

    tty_printf(
        "If problems continue, you might try debugging Soaplin\n");
    tty_printf("using a tool such as GNU Debug Bridge (gdb), or LLVM Debug Bridge (lldb).\n");

    tty_printf("Technical information: \n");
    tty_printf("*** STOP: %s (Arg1: %d)\n\n", ex_msgs[cause > MAX ? 0 : cause],
               (int *)args != NULL ? *(int *)args : -1);
    tty_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
               "!!!!!!!!!!\n\n");

    asm("cli");
    while (1)
        asm("hlt");
}

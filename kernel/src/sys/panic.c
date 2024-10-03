// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#include <sys/panic.h>
#include <sys/log.h>
#include <dev/tty.h>

char *ex_msgs[] = {
    "UNKNOWN",
    "KMODE_CPU_EXCEPTION"
};

void panic(int cause, void *args)
{
    //tty_set_bg(0x0000FF);
    //tty_set_fg(0xFFFFFF);
    //tty_set_margin(0);
    tty_clear();


    tty_printf("  ,-.,--.   ,--.,-.   \n");
    tty_printf(" / .' \\  `.'  / '. \\  \n");
    tty_printf("|  |   .'    \\   |  | \n");
    tty_printf("|  |  /  .'.  \\  |  | \n");
    tty_printf(" \\ '.'--'   '--'.' /  \n");
    tty_printf("  `-'           `-'   \n");

    tty_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
    tty_printf("A problem has been detected and your Soaplin-based OS has been shut down\n");
    tty_printf("to prevent damage to your computer.\n\n");

    tty_printf("If this is the first time you've seen this Stop error screen,\n");
    tty_printf("restart your computer. if this screen appears again, follow\n");
    tty_printf("these steps:\n\n");

    tty_printf("Check to make sure any new hardware or software is properly installed.\n");
    tty_printf("If this is a new installation, ask your hardware or software manufacturer\n");
    tty_printf("for any kernel patches you might need.\n\n");

    tty_printf("If problems continue, disable or remove any newly installed hardware\n");
    tty_printf("or software. Disable BIOS memory options such as caching or shadowing.\n");
    tty_printf("If you need to use Safe Mode to remove or disable components, restart\n");
    tty_printf("your computer, wait till the Limine boot menu appears, and then\n"); 
    tty_printf("select the Safe Mode entry and press ENTER.\n\n");

    tty_printf("Technical information: \n");
    tty_printf("*** STOP: %s (Arg1: %d)\n\n", ex_msgs[cause], (int*)args != NULL ? *(int*)args : -1);
    tty_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");

    asm("cli");
    while (1)
        asm("hlt");
}

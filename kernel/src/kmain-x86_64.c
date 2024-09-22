#include <dev/serio.h>
#include <limine.h>
#include <dev/tty.h>
#include <sys/log.h>

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

void kmain_x86_64()
{
    serio_init();
    tty_enable();

    logln(info, "kernel", "Soaplin v0.1 - Zephyr (Milestone 1)\n");

    logln(info, "kernel", "Hello logger! %x\n", 0x10);
    logln(warn, "kernel", "Hello logger! %x\n", 0x10);
    logln(panic, "kernel", "Hello logger! %x\n", 0x10);
    logln(err, "kernel", "Hello logger! %x\n", 0x10);
    logln(progress, "kernel", "Hello logger! %x\n", 0x10);

    while (1)
        ;;
}
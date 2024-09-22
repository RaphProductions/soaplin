#include <dev/tty.h>
#include <sys/log.h>
#include <stdint.h>
#include <stdarg.h>
#include <tpf/npf.h>
#include <sys/string.h>
#include <dev/serio.h>

uint32_t fg_lt[] = {
    0xf7d514,
    0x147ef7,
    0x750202,
    0xd10000,
    0x027512
};

char *nm_lt[] = {
    "warn",
    "info",
    "panic",
    "err",
    "progress"
};

void logln(logtype lt, char *cmp, char *msg, ...)
{
    tty_printf("%s - ", cmp);
    tty_set_fg(fg_lt[lt]);
    tty_printf(nm_lt[lt]);
    tty_reset_col();

    tty_print(": ", 2);

    serio_printf("%s - %s: ", cmp, nm_lt[lt]);

    char buf[2048];
    va_list lst;
    va_start(lst, msg);

    npf_vsnprintf(buf, 2048, msg, lst);

    va_end(lst);

    int i = strlen(buf);
    tty_print(buf, i);
    serio_print(buf, i);
}
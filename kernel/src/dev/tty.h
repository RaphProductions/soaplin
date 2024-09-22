#pragma once

#include <stddef.h>
#include <stdint.h>

void tty_set_fg(uint32_t fg);
void tty_set_bg(uint32_t bg);
void tty_reset_col();
void tty_enable();
void tty_disable();
void tty_print(char *str, size_t len);
void tty_printf(char *format, ...);
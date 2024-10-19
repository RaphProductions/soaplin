// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include <dev/serio.h>
#include <arch/x86_64/io.h>
#include <stdarg.h>
#include <sys/string.h>
#include <tpf/npf.h>

#define PORT 0x3f8

int serio_init() {
  outb(PORT + 1, 0x00); // Disable all interrupts
  outb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
  outb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
  outb(PORT + 1, 0x00); //                  (hi byte)
  outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
  outb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
  outb(PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
  outb(PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial
                        // returns same byte)

  // Check if serial is faulty (i.e: not same byte as sent)
  if (inb(PORT + 0) != 0xAE) {
    return 1;
  }

  // If serial is not faulty set it in normal operation mode
  // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
  outb(PORT + 4, 0x0F);

  serio_print("=============================\n", 30);
  serio_print("  Soaplin Kernel Logs:START  \n", 30);
  serio_print("=============================\n", 30);

  return 0;
}

int is_transmit_empty() { return inb(PORT + 5) & 0x20; }

void serio_print(char *buf, int len) {
  for (int i = 0; i < len; i++) {
    while (is_transmit_empty() == 0)
      ;
    outb(PORT, buf[i]);
  }
}

void serio_printf(char *format, ...) {
  char buf[2048];
  va_list lst;
  va_start(lst, format);

  npf_vsnprintf(buf, 2048, format, lst);

  va_end(lst);

  serio_print(buf, strlen(buf));
}

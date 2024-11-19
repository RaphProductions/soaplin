// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);
void io_wait(void);

#ifdef __cplusplus
}
#endif
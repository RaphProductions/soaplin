// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#pragma once

#include <stdint.h>

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void idt_init();

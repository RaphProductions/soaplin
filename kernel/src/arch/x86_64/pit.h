// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#pragma once

#include <stdint.h>

void pit_init(uint32_t frequency);
void pit_enable();
void pit_disable();
void pit_sleep(uint32_t seconds);
void pit_sleepms(uint32_t milliseconds);
void pit_usleep(uint32_t milliseconds);

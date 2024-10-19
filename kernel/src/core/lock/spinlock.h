// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#pragma once

#include <stdbool.h>

typedef struct {
  bool locked;
} spinlock;

void spinlock_acquire(spinlock* l);
void spinlock_release(spinlock* l);
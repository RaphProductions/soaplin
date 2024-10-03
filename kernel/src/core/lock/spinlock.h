// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#pragma once

typedef int spinlock;

void spinlock_acquire(spinlock *sp);
void spinlock_release(spinlock *sp);

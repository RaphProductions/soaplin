// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#pragma once

#define UNKNOWN 0
#define KMODE_CPU_EXCEPTION 1

void panic(int cause, void *args);

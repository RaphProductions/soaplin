// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#pragma once

int serio_init();
void serio_print(char *buf, int len);
void serio_printf(char *buf, ...);

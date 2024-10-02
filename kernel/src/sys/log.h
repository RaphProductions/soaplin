// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of the MIT License.

#pragma once

typedef enum {
    warn,
    info,
    panic_lg, // To prevent any symbol confusion with panic().
    err,
    progress,
    len
} logtype;

void logln(logtype lt, char *cmp, char *msg, ...);

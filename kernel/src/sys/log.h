#pragma once

typedef enum {
    warn,
    info,
    panic,
    err,
    progress,
    len
} logtype;

void logln(logtype lt, char *cmp, char *msg, ...);
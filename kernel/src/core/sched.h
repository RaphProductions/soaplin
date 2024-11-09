// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#pragma once

#include "arch/x86_64/_arch.h"
#include <arch/arch.h>
#include <mm/vmm.h>

#define PROCESS_NAME_SIZE 128

typedef enum { READY, RUNNING, DEAD } sched_status;
typedef struct __sched_process sched_process;

typedef struct __sched_thread {

  size_t tid;
  stackframe context;
  sched_status status;
  char name[PROCESS_NAME_SIZE];

  sched_process *parent;
  struct __sched_thread *next;

} sched_thread;

typedef struct __sched_process {

  char name[PROCESS_NAME_SIZE];
  int pid;
  sched_thread* threads;
  struct __sched_process *next;

} sched_process;

extern sched_process *sched_processes_list;
extern sched_process *sched_current_process;
extern sched_thread *sched_threads_list;

void sched_init();
sched_process *sched_new(char *name);
sched_thread *sched_new_thread(sched_process *proc, char *name, void (*function)());
void schedule(stackframe *sf);
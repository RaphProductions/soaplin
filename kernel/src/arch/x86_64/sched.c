// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.

#include "mm/pmm.h"
#include "sys/log.h"
#include <stdint.h>
#ifdef __x86_64__

#include <core/sched.h>
#include <mm/memop.h>
#include <sys/string.h>

sched_process *sched_processes_list;
sched_process *sched_current_process;
sched_thread *sched_current_thread;
size_t next_free_pid = 0;

static sched_process *idle_process;
static uint64_t sched_alloc_stack() { return (uint64_t)pmm_alloc(4096) + 4095; }

static void sched_idle() {
  while (true)
    ;
  ;
}

void sched_init() {
  sched_current_process = (sched_process *)pmm_alloc(sizeof(sched_process));

  memcpy(sched_current_process->name, "kernel", strlen("kernel"));
  sched_current_process->pid = -1;
  sched_current_process->next = NULL;
  sched_processes_list = sched_current_process;

  sched_current_thread = (sched_thread *)pmm_alloc(sizeof(sched_thread));

  memcpy(sched_current_thread->name, "kernel", strlen("kernel"));
  sched_current_thread->tid = -1;
  sched_current_thread->next = NULL;
  sched_current_process->threads = sched_current_thread;

  idle_process = sched_new("idle");
  sched_new_thread(idle_process, "idle", sched_idle);
}

size_t next_thread_id = 0;

sched_thread *sched_new_thread(sched_process *proc, char *name,
                               void (*function)()) {
  sched_thread *thread = (sched_thread *)pmm_alloc(sizeof(sched_thread));
  if (proc->threads == NULL)
    proc->threads = thread;
  else {
    for (sched_thread *scan = proc->threads; scan != NULL; scan = scan->next) {
      if (scan->next != NULL)
        continue;
      scan->next = thread;
      break;
    }
  }

  int len = strlen(name);
  memcpy(thread->name, name, len >= 100 ? 100 : len);

  thread->parent = proc;
  thread->tid = next_thread_id++;
  thread->status = READY;
  thread->next = NULL;
  thread->context.ss = 0x10;
  thread->context.rsp = sched_alloc_stack();
  thread->context.rflags = 0x202;
  thread->context.cs = 0x08;
  thread->context.rip = (uint64_t)function;
  thread->context.rbp = 0;

  logln(info, "sched", "new thread %d\n", thread->tid);

  return thread;
}

sched_process *sched_new(char *name) {
  sched_process *process = (sched_process *)pmm_alloc(sizeof(sched_process));

  int len = strlen(name);
  memcpy(process->name, name, len >= 100 ? 100 : len);
  process->pid = next_free_pid++;
  process->threads = NULL;
  process->next = NULL;

  sched_current_process->next = process;

  return process;
}

void schedule(stackframe *sf) {
  sched_process *prev_process;
  sched_thread *prev_thread;

  memcpy(&sched_current_thread->context, sf, sizeof(stackframe));
  sched_current_thread->status = READY;

  while (1) {
    prev_process = sched_current_process;
    prev_thread = sched_current_thread;

    if (sched_current_process->next != NULL) {
      sched_current_process = sched_current_process->next;
    } else {
      sched_current_process = sched_processes_list;
    }

    sched_current_thread = sched_current_process->threads;

    while (sched_current_thread != NULL) {
      if (sched_current_thread->status == DEAD) {
        sched_current_thread = sched_current_thread->next;
        continue;
      }

      sched_current_thread->status = RUNNING;
      break;
    }

    if (sched_current_thread != NULL &&
        sched_current_thread->status == RUNNING) {
      break;
    }
  }

  //logln(info, "sched", "choosed thread %d from process %s %d\n",
  //      sched_current_thread->tid, sched_current_process->name,
  //      sched_current_process->pid);

  memcpy(sf, &sched_current_thread->context, sizeof(stackframe));
}

#endif

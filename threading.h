#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "syscall.h"

typedef void (*kproc_t)(void *);

void init_threading();
void PROC_run();
void PROC_create_kthread(kproc_t entry_point, void *args);
void kexit();

inline void yield(void) {
    asm volatile ("mov %0, %%rdi" : : "ND"(SYSCALL_SCHED_YIELD));
    asm volatile ("int $0x80");
}

#endif

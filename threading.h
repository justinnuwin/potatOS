#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdint.h>

#include "syscall.h"

typedef void (*kproc_t)(void *);

struct KThread {
    uint64_t rax, rbx, rcx, rdx, rdi, rsi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t fs, gs;
    // uint64_t cs, ss, ds, es, fs, gs;
    uint64_t ret_rip, ret_cs, ret_rflags, ret_rsp, ret_ss;
    struct KThread *next, *prev;
    int stack_number;
    void *stack;
    kproc_t entry_point;
    void *args;
    int pid;
} __attribute__ ((packed));

struct KThread *current_thread, *next_thread;

void init_threading();
void PROC_run();
struct KThread *PROC_create_kthread(kproc_t entry_point, void *args);
void kexit();

inline void yield(void) {
    asm volatile ("mov %0, %%rdi" : : "ND"(SYSCALL_SCHED_YIELD));
    asm volatile ("int $0x80");
}

#endif

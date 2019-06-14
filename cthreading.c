#include "threading.h"

typedef void (*kproc_t)(void *);

extern "C" struct cKThread {
    uint64_t rax, rbx, rcx, rdx, rdi, rsi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t fs, gs;
    // uint64_t cs, ss, ds, es, fs, gs;
    uint64_t ret_rip, ret_cs, ret_rflags, ret_rsp, ret_ss;
    struct cKThread *next, *prev;
    int stack_number;
    void *stack;
    kproc_t entry_point;
    void *args;
    int pid;
} __attribute__ ((packed));

extern "C" struct KThread *cPROC_create_kthread(kproc_t entry_point, void *args) {
    return PROC_create_kthread(entry_point, args);
}

extern "C" void ckexit() {
    kexit();
}


extern "C" void cyield(void) {
    asm volatile ("mov %0, %%rdi" : : "ND"(SYSCALL_SCHED_YIELD));
    asm volatile ("int $0x80");
}

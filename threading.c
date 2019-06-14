#include "threading.h"

#include "kmalloc.h"
#include "interrupt.h"
#include "string.h"
#include "kernel.h"
#include "page_table.h"

#define MAX_NUM_THREADS 512      // Set by number of PTL3 entries reserved for kernel stacks

struct KThread {
    uint64_t rax, rbx, rcx, rdx, rdi, rsi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t fs, gs;
    // uint64_t cs, ss, ds, es, fs, gs;
    uint64_t ret_rip, ret_cs, ret_rflags, ret_rsp, ret_ss;
    struct KThread *next;
    int stack_number;
    void *stack;
    kproc_t entry_point;
    void *args;
} __attribute__ ((packed));

struct KThread *current_thread, *next_thread;

struct KThread *threads[MAX_NUM_THREADS] = {0};    // Ignore index 0

void PROC_create_kthread(kproc_t entry_point, void *args) {
    int stack_number;
    for (stack_number = 0; stack_number < MAX_NUM_THREADS; stack_number++) {
        if (!threads[stack_number])
            break;
    }
    struct KThread *thread = (struct KThread *)kmalloc(sizeof(*thread));
    threads[stack_number] = thread;
    thread->stack_number = stack_number;
    thread->stack = (void *)((((uint64_t)1 << 39) | (((uint64_t)stack_number + 1) << 30)) - 1);
    thread->ret_rsp = (uint64_t)thread->stack;
    thread->ret_cs = 8;
    thread->entry_point = entry_point;
    thread->ret_rip = (uint64_t)entry_point;
    thread->args = args;
    if (!current_thread) {
        current_thread = thread;
        current_thread->next = thread;
    } else {
        thread->next = current_thread->next;
        current_thread->next = thread;
    }
}

extern "C" void sys_call_isr_wrapper(void);
void init_threading() {
    register_isr(sys_call_isr_wrapper, 0x80);
    PROC_create_kthread(kernel_loop, NULL);
}

void PROC_run() {
    yield();
}

void kexit() {
    // current_thread is only NULL when exiting
    // next_thread is only used when exiting
    struct KThread *this_thread = current_thread;
    next_thread = current_thread->next;
    current_thread = NULL;
    free_stack(this_thread->stack_number);
    threads[this_thread->stack_number] = NULL;
    kfree(this_thread);
    yield();
}

inline void PROC_reschedule() {
    if (current_thread)
        current_thread = current_thread->next;
    else
        current_thread = next_thread->next;
}

inline void save_context(struct KThread *thread, void *sp) {
    memcpy(current_thread, sp, 21 * sizeof(uint64_t));
}

inline void load_context(struct KThread *next_thread, void *sp) {
    memcpy(sp, next_thread, 21 * sizeof(uint64_t));
}

void context_switch(void *rsp) {
    if (current_thread) {
        save_context(current_thread, rsp);
        load_context(current_thread->next, rsp);
    } else {
        load_context(next_thread, rsp);
    }
    PROC_reschedule();
}

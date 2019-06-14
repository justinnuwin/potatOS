#include "threading.h"

#include "kmalloc.h"
#include "interrupt.h"
#include "string.h"
#include "kernel.h"

#define MAX_NUM_THREADS 512      // Set by number of PTL3 entries reserved for kernel stacks

struct KThread {
    uint64_t rax, rbx, rcx, rdx, rdi, rsi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t fs, gs;
    // uint64_t cs, ss, ds, es, fs, gs;
    uint64_t rbp, rsp;
    uint64_t rip, ret_cs, rflags, ret_rsp, ret_ss;
    struct KThread *next;
    void *stack;
    kproc_t entry_point;
    void *args;
} __attribute__ ((packed));

struct KThread *current_thread;

struct KThread *threads[MAX_NUM_THREADS] = {0};    // Ignore index 0

void PROC_create_kthread(kproc_t entry_point, void *args) {
    int stack_number;
    for (stack_number = 0; stack_number < MAX_NUM_THREADS; stack_number++) {
        if (!threads[stack_number])
            break;
    }
    struct KThread *thread = (struct KThread *)kmalloc(sizeof(*thread));
    threads[stack_number] = thread;
    thread->stack = (void *)((((uint64_t)1 << 39) | ((uint64_t)stack_number << 30)) - 1);
    thread->ret_rsp = (uint64_t)thread->stack;
    thread->ret_cs = 8;
    thread->entry_point = entry_point;
    thread->rip = (uint64_t)entry_point;
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
    if (current_thread) {
        yield();
    }
}

void kexit();

extern "C" void sys_call_interrupt_handler(void *rsp) {
    // Save current context
    memcpy(current_thread, rsp, 23 * sizeof(uint64_t));
    // Load next context
    current_thread->next->rbp = current_thread->rbp;
    current_thread->next->rsp = current_thread->rsp;
    memcpy(rsp, current_thread->next, 23 * sizeof(uint64_t));
    current_thread = current_thread->next;
}
